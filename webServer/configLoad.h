#include <iostream>
#include <string>
#include <unordered_map>
#include "Hiredis.h"
#include "redisConnector.h"
#include "../net/EventLoop.h"
using namespace std;

extern string g_nginx_ip;
extern string g_webServer_ip;
extern int g_webServer_port;
extern int g_webServer_poolThreads;
extern int g_webServer_loopThreads;

extern string g_resource_base_path;
extern string g_resource_index_path;

extern string g_log_level;
extern string g_log_path;
extern long long g_log_maxSize;
extern int g_log_rollDaySeconds;
extern int g_log_rollZoneTime;
extern int g_log_IOFrequency;

extern string g_heartBeat_switch;
extern string g_heartBeat_ip;
extern int g_heartBeat_port;
extern int g_heartBeat_frequency;

extern string g_conf_path;
extern std::unordered_map<string,string> mimeType;

extern string g_redis_ip;
extern int g_redis_port;
extern float g_redis_rate;
extern vector<string> g_redis_vip;
extern std::unordered_map<string,string> urlTomd5;
extern std::unordered_map<string,int> g_zlibMap;
extern double g_stop_after;
extern double g_conn_expire;
extern int g_maxConnections;
//extern hiredis::Hiredis* g_hiredis;
//extern EventLoop* g_loop;
//extern redisConnector* g_redis;
//extern std::unordered_map<EventLoop*,redisConnector*> g_redisMap;
bool init(string& conf_path);
