#include "../base/log/Logging.h"
#include "../tcpSocket/TcpServer.h"
#include "../tcpSocket/EventLoop.h"

#include <unordered_map>
#include <vector>
using namespace muduo;
using namespace muduo::net;
std::unordered_map<string,string> map;
std::vector<TcpConnectionPtr> vec;
std::unordered_map<TcpConnectionPtr,string> identityMap;
std::unordered_map<string,time_t> timeMap;
void onConnection(const TcpConnectionPtr& conn){
    time_t now = ::time(nullptr);
    //conn->setContext(now);
    vec.emplace_back(conn);
}
void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp time){
    time_t now = ::time(nullptr);
    //conn->setContext(now);
    string status = "ON";
    string identity = buf->retrieveAllAsString();
    map[identity] = status;
    identityMap[conn] = identity;
    timeMap[identity] = now;
}

void displayStatus(){
    time_t now = ::time(nullptr);
    for(auto conn:vec){
        time_t last_time = timeMap[identityMap[conn]];
        int diff = now - last_time;
        if(diff >= 10 + 2)
            map[identityMap[conn]] = "OFF";
    }
    system("clear");
    for(auto node:map){
        if(node.second == "ON")
            printf("%s : ""\e[0;32m""%s""\e[0m""\tlastest HeartBeat : %s\n",
                   node.first.c_str(),node.second.c_str(),ctime(&timeMap[node.first]));
        else if(node.second == "OFF" && node.first != "")
            printf("%s : ""\e[0;31m""%s""\e[0m""\tlastest HeartBeat : %s\n",
                   node.first.c_str(),node.second.c_str(),ctime(&timeMap[node.first]));
    }
}
int main(){
    EventLoop loop;
    InetAddress addr(static_cast<uint16_t >(8088));
    TcpServer server(&loop,addr,"HeartBeatChecker",muduo::net::TcpServer::kReusePort);
    server.setConnectionCallback(std::bind(onConnection,_1));
    server.setMessageCallback(std::bind(onMessage,_1,_2,_3));
    server.start();

    loop.runEvery(5,displayStatus);
    muduo::Logger::setLogLevel(muduo::Logger::INFO);
    loop.loop();
    return 0;
}
