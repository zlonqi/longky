#include "Hiredis.h"
#include "../net/Buffer.h"
#include "../net/EventLoop.h"
#include "../net/InetAddress.h"
#include "../net/TcpConnection.h"
#include "ZLibString.h"
#include <string>
#include <vector>

using namespace muduo::net;
class redisConnector{
public:
    redisConnector(EventLoop* loop, const TcpConnectionPtr& conn,string ip, int port, int rate);
    ~redisConnector();
    void start();

    static string redisReplyToString(const redisReply* reply);
    static string toString(long long value);
    static void connectCallback(hiredis::Hiredis* c, int status);
    void setCallback(hiredis::Hiredis* c, redisReply* reply);

    void getCallback(hiredis::Hiredis* c, redisReply* reply);
    void run(hiredis::Hiredis* hiredis, const char* file, const char* md5);
    static void disconnectCallback(hiredis::Hiredis* c, int status);
    char* returnBody();

    void exeSET();
    void setConn(const TcpConnectionPtr& conn);
    void setFile(string file);
    void setMD5(string md5);
    void setParams(std::vector<string> p){
        params_ = p;
    }
    hiredis::Hiredis* Hiredis_;
    std::vector<string> params_;
private:
    //InetAddress serverAddr_;
    std::weak_ptr<TcpConnection> conn_;
    const char* argv[3];
    Buffer response_;

    bool flag = true;
    string redisResult = "";
    string g_file = "";
    string g_md5 = "";
    int g_rate;
    char* g_unComPressContent = nullptr;
    char* comPressString = nullptr;
    unsigned long compressLen = 0;
    CZlibMgr* g_zlib;
};
