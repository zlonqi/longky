#ifndef FASTCGI_FASTCGI_H
#define FASTCGI_FASTCGI_H

#include "netLayer/tcp/TcpConnection.h"
#include "base/thread/Mutex.h"
#include "type.h"
#include <map>

using tank::string;

// one FastCgiCodec per TcpConnection
// both lighttpd and nginx do not implement multiplexing,
// so there is no concurrent requests of one connection.
class FastCgiCodec : tank::noncopyable
{
 public:
  //typedef std::map<string, string> ParamMap;
  typedef std::function<void (const tank::net::TcpConnectionPtr& conn,
                              ParamMap&,
                              tank::net::Buffer*)> Callback;

  explicit FastCgiCodec(const Callback& cb)
    : cb_(cb),
      gotRequest_(false),
      keepConn_(false)
  {
  }

  void onMessage(const tank::net::TcpConnectionPtr& conn,
                 tank::net::Buffer* buf,
                 tank::Timestamp receiveTime)
  {
    parseRequest(buf);
    if (gotRequest_)
    {
      cb_(conn, params_, &stdin_);
      stdin_.retrieveAll();
      paramsStream_.retrieveAll();
      params_.clear();
      gotRequest_ = false;
      if (!keepConn_)
      {
        conn->shutdown();
      }
    }
  }

  static void respond(std::string &out, tank::net::Buffer* response);

 private:
  struct RecordHeader;
  bool parseRequest(tank::net::Buffer* buf);
  bool onBeginRequest(const RecordHeader& header, const tank::net::Buffer* buf);
  void onStdin(const char* content, uint16_t length);
  bool onParams(const char* content, uint16_t length);
  bool parseAllParams();
  uint32_t readLen();

  static void endStdout(tank::net::Buffer* buf);
  static void endRequest(tank::net::Buffer* buf);

  Callback cb_;
  bool gotRequest_;
  bool keepConn_;
  tank::net::Buffer stdin_;
  tank::net::Buffer paramsStream_;
  ParamMap params_;

  const static unsigned kRecordHeader;
};

#endif  //FASTCGI_FASTCGI_H
