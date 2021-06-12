#ifndef REQUEST_PARSER_H
#define REQUEST_PARSER_H

#include "netLayer/Buffer.h"
#include "netLayer/tcp/TcpConnection.h"
#include "base/log/Logging.h"
#include "redis.h"
#include "ZLibString.h"
#include <unordered_map>
#include <vector>

using namespace tank;
using namespace tank::net;

class Parser {
public:
    Parser(TcpConnectionPtr &conn, Buffer *request);

    Parser();

    ~Parser();

    void setConn(const TcpConnectionPtr &conn, Buffer *request);

    //void parserRequest(std::vector<string>& vrst);
    void parserRequest();

    void connectRedis();

    void retryInLoop();

    char *findCRLF(char *begin, char *end) const {
        // FIXME: replace with memmem()?
        char *crlf = std::search(begin, end, kCRLF, kCRLF + 2);
        return crlf == end ? nullptr : crlf;
    }

    void responseRequest(const string &g_file, const string &mime);

    //void responseRequest(string g_file, const string mime,const string connectionFlag,Parser* parser);
    Buffer response_;
private:
    string not_found();//404
    string not_autho();//403
    string unimplemented();//501
    string version_not_support();//505

    std::weak_ptr<TcpConnection> conn_;
    std::unordered_map<string, string> paramMap_;
    Buffer *content_ = nullptr;
    ccx::Redis *redis_ = nullptr;
    CZlibMgr *g_zlib = nullptr;
    EventLoop *curLoop_ = nullptr;
    long long count_ = 0;

    const char *kCRLF = "\r\n";
};

#endif