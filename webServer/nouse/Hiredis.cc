#include "Hiredis.h"

#include "base/log/Logging.h"
#include "netLayer/event/Channel.h"
#include "netLayer/event/eventloop/EventLoop.h"
#include "netLayer/socket/SocketsOps.h"

#include <hiredis/async.h>
#include <vector>

using namespace muduo;
using namespace muduo::net;
using namespace hiredis;

static void dummy(const std::shared_ptr<Channel>&)
{
}

Hiredis::Hiredis(EventLoop* loop, string ip, int port)
  : loop_(loop),
    serverAddr_(new InetAddress(ip,port)),
    context_(NULL),
    isConnect(false)
{
    LOG_INFO << "redis: " << serverAddr_->toIpPort();
}

Hiredis::~Hiredis()
{
  LOG_DEBUG << this;
  assert(!channel_ || channel_->isNoneEvent());
  ::redisAsyncFree(context_);
  delete serverAddr_;
}

bool Hiredis::isConnected() const{
    return isConnect;
}
bool Hiredis::connected() const
{
    return channel_ && context_;
}

const char* Hiredis::errstr() const
{
  assert(context_ != NULL);
  return context_->errstr;
}

void Hiredis::connect()
{
  assert(!context_);

  context_ = ::redisAsyncConnect(serverAddr_->toIp().c_str(), serverAddr_->toPort());

  context_->ev.addRead = addRead;
  context_->ev.delRead = delRead;
  context_->ev.addWrite = addWrite;
  context_->ev.delWrite = delWrite;
  context_->ev.cleanup = cleanup;
  context_->ev.data = this;

  setChannel();

  assert(context_->onConnect == NULL);
  assert(context_->onDisconnect == NULL);
  ::redisAsyncSetConnectCallback(context_, connectCallback);
  ::redisAsyncSetDisconnectCallback(context_, disconnectCallback);
}

void Hiredis::disconnect()
{
  if (connected())
  {
    LOG_DEBUG << this;
    ::redisAsyncDisconnect(context_);
  }
}

int Hiredis::fd() const
{
  assert(context_);
  return context_->c.fd;
}

void Hiredis::setChannel()
{
  LOG_DEBUG << this;
  assert(!channel_);
  channel_.reset(new Channel(loop_, fd()));
  channel_->setReadCallback(std::bind(&Hiredis::handleRead, this, _1));
  channel_->setWriteCallback(std::bind(&Hiredis::handleWrite, this));
}

void Hiredis::removeChannel()
{
  LOG_DEBUG << this;
  channel_->disableAll();
  channel_->remove();
  loop_->queueInLoop(std::bind(dummy, channel_));
  channel_.reset();
}

void Hiredis::handleRead(muduo::Timestamp receiveTime)
{
  LOG_TRACE << "receiveTime = " << receiveTime.toString();
  ::redisAsyncHandleRead(context_);
}

void Hiredis::handleWrite()
{
  if (!(context_->c.flags & REDIS_CONNECTED))
  {
    removeChannel();
  }
  ::redisAsyncHandleWrite(context_);
}

/* static */ Hiredis* Hiredis::getHiredis(const redisAsyncContext* ac)
{
  Hiredis* hiredis = static_cast<Hiredis*>(ac->ev.data);
  assert(hiredis->context_ == ac);
  return hiredis;
}

void Hiredis::logConnection(bool up) const
{
  InetAddress localAddr(sockets::getLocalAddr(fd()));
  InetAddress peerAddr(sockets::getPeerAddr(fd()));

  LOG_INFO << localAddr.toIpPort() << " -> "
           << peerAddr.toIpPort() << " is "
           << (up ? "UP" : "DOWN");
}

/* static */ void Hiredis::connectCallback(const redisAsyncContext* ac, int status)
{
  LOG_TRACE;
  getHiredis(ac)->connectCallback(status);
}

void Hiredis::connectCallback(int status)
{
  if (status != REDIS_OK)
  {
      LOG_WARN << context_->errstr << " failed to connect to " << serverAddr_->toIpPort();
    isConnect = false;
  }
  else
  {
      isConnect = true;
    logConnection(true);
    setChannel();
  }

  if (connectCb_)
  {
    connectCb_(this, status);
  }
}

/* static */ void Hiredis::disconnectCallback(const redisAsyncContext* ac, int status)
{
  LOG_TRACE;
    //abort();
  getHiredis(ac)->disconnectCallback(status);
}

void Hiredis::disconnectCallback(int status)
{
  logConnection(false);
  removeChannel();

  if (disconnectCb_)
  {
    disconnectCb_(this, status);
  }
}

void Hiredis::addRead(void* privdata)
{
  LOG_TRACE;
  Hiredis* hiredis = static_cast<Hiredis*>(privdata);
  hiredis->channel_->enableReading();
}

void Hiredis::delRead(void* privdata)
{
  LOG_TRACE;
  Hiredis* hiredis = static_cast<Hiredis*>(privdata);
  hiredis->channel_->disableReading();
}

void Hiredis::addWrite(void* privdata)
{
  LOG_TRACE;
  Hiredis* hiredis = static_cast<Hiredis*>(privdata);
  hiredis->channel_->enableWriting();
}

void Hiredis::delWrite(void* privdata)
{
  LOG_TRACE;
  Hiredis* hiredis = static_cast<Hiredis*>(privdata);
  hiredis->channel_->disableWriting();
}

void Hiredis::cleanup(void* privdata)
{
  Hiredis* hiredis = static_cast<Hiredis*>(privdata);
  LOG_DEBUG << hiredis;
}

void Hiredis::command2(const CommandCallback& cb, char* unpress, unsigned long unLen,string md5)
{
    if (!connected()) return ;

    LOG_TRACE;
    CommandCallback* p = new CommandCallback(cb);

    const char* argv[3];
    argv[0]="set";
    argv[1]=&md5[0];
    argv[2]=unpress;
    size_t argvlen[3]={3,32,unLen};
    int ret = :: redisAsyncCommandArgv(context_, commandCallback, p, 3,argv,argvlen);
    //return ret;
}
int Hiredis::command(const CommandCallback& cb, muduo::StringArg cmd, ...)
{
  if (!connected()) return REDIS_ERR;

  LOG_TRACE;
  CommandCallback* p = new CommandCallback(cb);
  va_list args;
  va_start(args, cmd);
  int ret = ::redisvAsyncCommand(context_, commandCallback, p, cmd.c_str(), args);
  va_end(args);
  return ret;
}

/* static */ void Hiredis::commandCallback(redisAsyncContext* ac, void* r, void* privdata)
{
  redisReply* reply = static_cast<redisReply*>(r);
  CommandCallback* cb = static_cast<CommandCallback*>(privdata);
  getHiredis(ac)->commandCallback(reply, cb);
}

void Hiredis::commandCallback(redisReply* reply, CommandCallback* cb)
{
  (*cb)(this, reply);
  delete cb;
}

