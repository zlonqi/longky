#include "configLoad.h"

#include <yaml-cpp/yaml.h>
#include <openssl/md5.h>
#include <sstream>

string g_nginx_ip;
string g_webServer_ip;
int g_webServer_port;
int g_webServer_poolThreads;
int g_webServer_loopThreads;
double g_conn_expire;
int g_maxConnections;

string g_resource_base_path;
string g_resource_index_path;

string g_log_level;
string g_log_path;
long long g_log_maxSize;
int g_log_rollDaySeconds;
int g_log_rollZoneTime;
int g_log_IOFrequency;

string g_heartBeat_ip;
string g_heartBeat_switch;
int g_heartBeat_port;
int g_heartBeat_frequency;
std::unordered_map<string,string> mimeType;

string g_redis_ip;
int g_redis_port;
float g_redis_rate;
double g_stop_after;
vector<string> g_redis_vip;
//hiredis::Hiredis* g_hiredis;
//redisConnector* g_redis;

//EventLoop* g_loop;
std::unordered_map<string,string> urlTomd5;

std::string strToHex(unsigned char* str, std::string separator = "")
{
    const std::string hex = "0123456789ABCDEF";
    std::stringstream ss;

    for (std::string::size_type i = 0; i < 16; ++i)
        ss << hex[str[i] >> 4] << hex[str[i] & 0xf] << separator;

    return ss.str();
}

bool init(string& conf_path){
    MD5_CTX ctx;
    unsigned char outmd[16];
    memset(outmd,0,sizeof(outmd));
    MD5_Init(&ctx);

    bool no_error = true;
    try {
        YAML::Node config = YAML::LoadFile(conf_path);
        g_nginx_ip = config["webServer"]["ip"].as<string>();
        g_webServer_port = config["webServer"]["port"].as<int >();
        g_webServer_poolThreads = config["webServer"]["poolThreads"].as<int >();
        g_webServer_loopThreads = config["webServer"]["loopThreads"].as<int >();
        g_nginx_ip = config["webServer"]["nginxIP"].as<string>();
        g_stop_after = config["webServer"]["aliveTime"].as<double>();
        g_conn_expire = config["webServer"]["connIdleTime"].as<double>();
        g_maxConnections = config["webServer"]["maxConnections"].as<int>();

        g_log_level = config["log"]["level"].as<string>();
        g_log_path = config["log"]["path"].as<string>();
        g_log_maxSize = config["log"]["maxSize"]["G"].as<long long>()*1024*1024*1024
                        + config["log"]["maxSize"]["M"].as<long long>()*1024*1024
                        + config["log"]["maxSize"]["K"].as<int>()*1024
                        + config["log"]["maxSize"]["B"].as<int>();
        g_log_rollZoneTime = config["log"]["rollTime"]["zoneTime"].as<int>();
        g_log_rollDaySeconds = config["log"]["rollTime"]["hour"].as<int>()*3600
                               + config["log"]["rollTime"]["minute"].as<int>()*60
                               + config["log"]["rollTime"]["second"].as<int>();
        g_log_IOFrequency = config["log"]["IOFrequency"].as<int>();

        g_heartBeat_switch = config["heartBeat"]["switch"].as<string>();
        g_heartBeat_ip = config["heartBeat"]["server"]["ip"].as<string>();
        g_heartBeat_port = config["heartBeat"]["server"]["port"].as<int>();
        g_heartBeat_frequency = config["heartBeat"]["frequency"].as<int>();

        g_resource_base_path = config["resource"]["basePath"].as<string>();
        g_resource_index_path = config["resource"]["indexPath"].as<string>();

        for(YAML::const_iterator i = config["mime"].begin(); i!=config["mime"].end(); ++i)
            mimeType[i->first.as<string>()] = i->second.as<string>();

        g_redis_ip = config["redis"]["server"]["ip"].as<string>();
        g_redis_port = config["redis"]["server"]["port"].as<int>();
        g_redis_rate = config["redis"]["rate"].as<float>();

        for(YAML::const_iterator i = config["redis"]["vip"].begin(); i!=config["redis"]["vip"].end(); ++i){
            string url= i->second.as<string>();
            g_redis_vip.emplace_back(url);
            memset(outmd,0,sizeof(outmd));
            MD5_Init(&ctx);
            MD5_Update(&ctx,url.c_str(),url.length());
            MD5_Final(outmd,&ctx);
            string md5 = strToHex(outmd,"") ;
            urlTomd5[url] = md5;
        }
        if(config["zlibMap"]){
            for(YAML::const_iterator i = config["zlibMap"].begin(); i != config["zlibMap"].end(); ++i)
                g_zlibMap[i->first.as<string>()] = i->second.as<int>();
        }
    }catch (exception &e) {
        cout << e.what() << endl;
        no_error = false;
    }

    return no_error;
}
