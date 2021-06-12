#include "requestParser.h"
#include "configLoad.h"
#include "netLayer/event/Channel.h"
#include "base/log/Logging.h"

#include <sys/stat.h>
#include <unistd.h>
#include <utility>
#include <vector>
#include <unordered_map>
#include <yaml-cpp/yaml.h>
#include <cstdio>
#include <fcntl.h>
#include <cerrno>
#include <fstream>

#define ISspace(x) isspace((int)(x))

std::unordered_map<string, int> g_zlibMap;

Parser::Parser(TcpConnectionPtr &conn, Buffer *request)
        : content_(request),
          conn_(std::move(conn)) {
}

Parser::Parser() {
    redis_ = new ccx::Redis(g_redis_ip, g_redis_port);
    redis_->isRedisConnectWell_ = redis_->Connect();
    LOG_DEBUG << "connect to redis [" << redis_->isRedisConnectWell_ << "]";
    if (redis_->isRedisConnectWell_ == true)
        g_zlib = new CZlibMgr();
    LOG_DEBUG << "construct parser successfully";
}

Parser::~Parser() {
    if (redis_->isRedisConnectWell_)
        redis_->disConnect();
    delete redis_;
    if (g_zlib != nullptr)
        delete g_zlib;
    LOG_DEBUG << "destruct parser successfully";
}

void Parser::setConn(const TcpConnectionPtr &conn, Buffer *request) {
    conn_ = conn;
    content_ = request;
}

void Parser::parserRequest() {
    string request = content_->retrieveAllAsString();
    //printf("%s\n",request.c_str());
    int size = (int) request.length();
    int i = 0;
    int j = 0;
    char method[16];
    memset(&method, 0, 16);
    string mime;
    string base_path = g_resource_base_path;
    string index_path = base_path + g_resource_index_path;
    while (!ISspace(request[j]) && (i < sizeof(method) - 1)) {
        method[i] = request[j];
        i++;
        j++;
    }
    method[i] = '\0';

    if (strcasecmp(method, "GET") != 0) {
        LOG_INFO << "[501] unknow method for http request: " << method;
        goto Down;
    }
    char url[255];
    memset(&url, 0, 255);
    i = 0;
    //将method后面的后边的空白字符略过
    while (ISspace(request[j]) && (j < request.length()))
        j++;
    //继续读取request-URL
    while (request[j] != ' ' && (i < sizeof(url) - 1) && (j < size)) {
        url[i] = request[j];
        i++;
        j++;
    }
    url[i] = '\0';

    char path[255];
    //以上已经将起始行解析完毕
    //url中的路径格式化到path
    memset(&path, 0, 255);
    sprintf(path, (base_path + "%s").c_str(), url);

    //如果path只是一个目录，默认设置为首页index.html
    if (path[strlen(path) - 1] == '/')
        strcpy(path, index_path.c_str());

    LOG_INFO << "{" << conn_.lock()->getContextkv("requestId") << "}request: " << string(path);
    conn_.lock()->setContextkv("file", string(path));
    struct stat st;
    if (stat(path, &st) == -1) {
        LOG_INFO << "[404] not found";
        conn_.lock()->setWriteCompleteCallback(WriteCompleteCallback());//cancel writeCOmpleteCallback
        string res;
        res = not_found();
        response_.append(res);
        conn_.lock()->send(&response_);
        conn_.lock()->forceCloseWithDelay(1);
        return;
    }
    if (!(st.st_mode & S_IRUSR) &&
        !(st.st_mode & S_IRGRP) &&
        !(st.st_mode & S_IROTH)) {
        //have no authorization to read re.
        LOG_INFO << "[403] not authentication";
        conn_.lock()->setWriteCompleteCallback(WriteCompleteCallback());//cancel writeCOmpleteCallback
        string res;
        res = not_autho();
        response_.append(res);
        conn_.lock()->send(&response_);
        conn_.lock()->forceCloseWithDelay(1);
        return;
    }
    if ((st.st_mode & S_IFMT) == S_IFDIR)
        strcpy(path, index_path.c_str());

    int k;
    k = (int) strlen(path) - 1;
    while (path[k] != '.' && k > -1) {
        mime.insert(mime.begin(), path[k]);
        --k;
    }
    if (mimeType.find(mime) == mimeType.end()) {
        LOG_INFO << "[501] unknow file type for http request: " << mime;
        goto Down;
    }

    while (request[j] == ' ') ++j;

    //paramMap_.erase(paramMap_.begin(),paramMap_.end())
    char *tail;
    char *begin;
    char *crlf;
    tail = &request[size];
    begin = &request[j];
    crlf = findCRLF(begin, tail);
    if (crlf) {
        string version;
        version = string(crlf - 3, crlf);
        if (string(begin, crlf - 3) != "HTTP/" || (version != "1.0" && version != "1.1" && version != "2.0")) {
            conn_.lock()->setWriteCompleteCallback(WriteCompleteCallback());//cancel writeCOmpleteCallback
            LOG_INFO << "[505] version not support: " << version;
            string res;
            res = version_not_support();
            response_.append(res);
            conn_.lock()->send(&response_);
            conn_.lock()->forceCloseWithDelay(1);
            return;
        }
        paramMap_["version"] = version;
        begin = crlf + 2;
    } else
        goto Down;

    char *colon;
    while (begin < tail) {
        crlf = findCRLF(begin, tail);
        if (crlf) {
            colon = std::find(begin, crlf, ':');
            if (colon < crlf) {
                paramMap_[string(begin, colon)] = string(colon + 2, crlf);
                begin = crlf + 2;
            } else {
                if (crlf + 2 == tail)
                    break;//last empty line ==> finish parser http header.
                else
                    goto Down;
            }
        } else {
            goto Down;
        }
    }
    bool finally;
    finally = false;
    //if(paramMap_.find("Connection")==paramMap_.end()){
    if (finally) {
        Down:
        LOG_INFO << "[501] unknow HTTP header";
        conn_.lock()->setWriteCompleteCallback(WriteCompleteCallback());//cancel writeCOmpleteCallback
        string res = unimplemented();
        response_.append(res);
        conn_.lock()->send(&response_);
        conn_.lock()->forceCloseWithDelay(1);
        return;
    }
    conn_.lock()->setContextkv("connectionFlag", paramMap_["Connection"]);
    responseRequest(path, mime);
}

void Parser::responseRequest(const string &g_file, const string &mime) {
    /*int fp = open(g_file, O_RDONLY);
    struct stat st;
    fstat(fp,&st);
    if(fp){
        sendfile64(conn_.lock()->channel_->fd(),fp,NULL,st.st_size);
    }
    close(fp);
    conn_.lock()->shutdown();*/

    errno = 0;
    bool isVIP = false;
    string md5;

    bool getFromRedis = false;
    if (find(g_redis_vip.begin(), g_redis_vip.end(), g_file) != g_redis_vip.end()) {
        isVIP = true;
    }
    string body;
    int fileSize = 0;
    bool fileCorrect = true;
    if (isVIP) {
        //ccx::Redis redis(g_redis_ip,g_redis_port);
        if (redis_->isRedisConnectWell_) {
            md5 = urlTomd5[g_file];
            string s;
            try {
                s = redis_->exeGET(const_cast<char *>(md5.c_str()));
            } catch (int e) {
                //LOG_WARN << e.what();
                if (e == 0)
                    goto ReConn;
            }
            bool error = false;
            if (s.empty() || g_zlibMap[md5] == 0) {
                FILE *fp = nullptr;
                fp = fopen(g_file.c_str(), "rb");
                if (fp) {
                    struct stat st{};
                    fstat(fp->_fileno, &st);
                    int size = st.st_size;
                    fileSize = size;
                    std::unique_ptr<char[]> plainString(new char[size]);
                    int nread = ::fread(plainString.get(), 1, size, fp);
                    unsigned long compressLen = size;
                    //char *comPressString = (char *) malloc((int)((float)size*g_redis_rate));
                    std::unique_ptr<char[]> comPressString(new char[(int) ((float) size * g_redis_rate) + 1]);
                    g_zlib->Compress(plainString.get(), comPressString.get(), compressLen);
                    g_zlibMap[md5] = size;
                    {
                        YAML::Node config = YAML::LoadFile(g_conf_path);
                        ofstream fout(g_conf_path);
                        config["zlibMap"][md5] = size;
                        fout << config;
                        fout.close();
                    }
                    body = string(plainString.get(), plainString.get() + size);
                    getFromRedis = true;
                    conn_.lock()->setWriteCompleteCallback(WriteCompleteCallback());//cancel writeCOmpleteCallback
                    try {
                        redis_->exeSET(const_cast<char *>(md5.c_str()), comPressString.get(), (int) compressLen);
                        LOG_INFO << "[successfully] post data to redis [md5]" << md5 <<
                                 " postCompressedSize/preCompressedSize[rate] " << (float) compressLen / (float) size;
                    } catch (int e) {
                        //LOG_WARN << e.what();
                        if (e == 1)
                            goto ReConn;
                    }
                } else {
                    printf("Error [%s]\n", (char *) strerror(errno));
                    fileCorrect = false;
                }
                fclose(fp);
                fp = nullptr;
            } else {
                int size = g_zlibMap[md5];
                fileSize = size;
                std::unique_ptr<char[]> g_unComPressContent(new char[size]);
                error = g_zlib->UnCompress(s.c_str(), s.length(), g_unComPressContent.get(), size);
                if (error) {
                    LOG_INFO << "<" << conn_.lock()->getContextkv("requestId")
                             << "> [successfully] got data from redis [md5]" << md5;
                    body = string(g_unComPressContent.get(), g_unComPressContent.get() + size);
                } else {
                    LOG_INFO << "<" << conn_.lock()->getContextkv("requestId")
                             << "> [Fail] unCompress data from redis [md5]" << md5;
                    redis_->isRedisConnectWell_ = false;
                }
                //printf("%s\n",g_unComPressContent);
                if (error)
                    getFromRedis = true;
                conn_.lock()->setWriteCompleteCallback(WriteCompleteCallback());//cancel writeCOmpleteCallback
            }
        } else {
            ReConn:
            LOG_INFO << "redis connect [Fail]";
            redis_->isRedisConnectWell_ = false;
            if (g_zlib) {
                delete g_zlib;
                g_zlib = nullptr;
            }
            curLoop_ = conn_.lock()->getLoop();
            curLoop_->runInLoop(std::bind(&Parser::retryInLoop, this));
        }
    }
    string res;
    if (!getFromRedis) {
        FILE *fp = ::fopen(g_file.c_str(), "rb");
        if (fp) {
            conn_.lock()->setContextfp(fp);
            struct stat st{};
            fstat(fp->_fileno, &st);
            int size = st.st_size;
            fileSize = size;
            // inefficient!!!
            const int kBufSize = 1024 * 1024;
            char iobuf[kBufSize];
            memset(&iobuf, 0, kBufSize);
            ::setbuffer(fp, iobuf, sizeof iobuf);

            char buf[kBufSize];
            memset(&buf, 0, sizeof kBufSize);
            size_t nread = 0;

            nread = ::fread(buf, 1, sizeof(buf), fp);
            if (nread <= 0) {
                goto Sdown;
            } else {
                body.append(buf, nread);
            }
        } else {
            fileCorrect = false;
        }
    }
    if (!fileCorrect) {
        conn_.lock()->forceClose();
        return;
    }
    res = "HTTP/";
    res += paramMap_["version"];
    res += " 200 OK\r\n";
    res += "Connection:";
    res += paramMap_["Connection"];
    res += "\r\n";
    res += "Content-type:";
    res += mimeType[mime];
    res += ";charset=utf-8";
    res += "\r\n";
    if (paramMap_["Connection"] != "close") {
        res += "Content-Length:";
        res += std::to_string(fileSize);
    }
    res += "\r\n\r\n";
    response_.append(res);
    response_.append(body);
    conn_.lock()->send(&response_);
    if (getFromRedis && paramMap_["Connection"] == "close")
        Sdown:
        conn_.lock()->shutdown();
}

string Parser::not_found() {
    string res = "HTTP/1.1 404 NOT FOUND\r\n";
    res += "Content-Type: text/html\r\n\r\n";
    res += "<HTML><TITLE>404 Not Found</TITLE>\r\n";
    res += "<BODY>The server could not fulfill\r\n";
    res += "your request because the resource specified\r\n";
    res += "is unavailable or nonexistent.\r\n";
    res += "</BODY></HTML>\r\n";

    return res;
}

string Parser::not_autho() {
    string res = "HTTP/1.1 403 Forbidden\r\n";
    res += "Content-Type: text/html\r\n\r\n";
    res += "<HTML><TITLE> 403 Forbidden </TITLE>\r\n";
    res += "<BODY><P> 403 Forbidden \r\n";
    res += "the resource you accessed you have no permission for it \r\n";
    res += "</BODY></HTML>\r\n";

    return res;
}

string Parser::unimplemented() {
    string res = "HTTP/1.1 501 Method Not Implemented\r\n";
    res += "Content-Type: text/html\r\n\r\n";
    res += "<HTML><HEAD><TITLE>Method Not Implemented\r\n";
    res += "</TITLE></HEAD>\r\n";
    res += "<BODY><P>HTTP request method not supported.\r\n";
    res += "</BODY></HTML>\r\n";

    return res;
}

string Parser::version_not_support() {
    string res = "HTTP/1.1 505 version not support\r\n";
    res += "Content-Type: text/html\r\n\r\n";
    res += "<HTML><HEAD><TITLE>version not suppord\r\n";
    res += "</TITLE></HEAD>\r\n";
    res += "<BODY><P>HTTP request version not supported.\r\n";
    res += "</BODY></HTML>\r\n";

    return res;
}

void Parser::retryInLoop() {
    if (++count_ == LLONG_MAX)
        count_ = 0;
    if (count_ % 5 == 1) {
        srand((unsigned) time(nullptr));
        curLoop_->runAfter(rand() % 10 + 1, std::bind(&Parser::connectRedis, this));
    }
}

void Parser::connectRedis() {
    if (!redis_->isRedisConnectWell_) {
        redis_->isRedisConnectWell_ = redis_->Connect();
        LOG_DEBUG << "connect to redis [" << redis_->isRedisConnectWell_ << "]";
        if (redis_->isRedisConnectWell_ && !g_zlib)
            g_zlib = new CZlibMgr();
        else
            curLoop_->runInLoop(std::bind(&Parser::retryInLoop, this));
    }
}
