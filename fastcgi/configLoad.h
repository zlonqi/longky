#include <iostream>
#include <string>
using namespace std;

extern string g_nginx_ip;
extern int g_cgi_port;
extern int g_cgi_poolThreads;
extern int g_cgi_loopThreads;

extern string g_webServer_nginxIP;

extern string g_resource_base_path;
extern string g_resource_index_path;

extern string g_log_level;
extern string g_log_path;
extern long long g_log_maxSize;
extern int g_log_rollDaySeconds;
extern int g_log_rollZoneTime;
extern int g_log_IOFrequency;

extern string g_heartBeat_ip;
extern int g_heartBeat_port;
extern int g_heartBeat_frequency;


extern string g_db_server;
extern string g_db_user;
extern string g_db_pswd;
extern string g_db_dbName;

