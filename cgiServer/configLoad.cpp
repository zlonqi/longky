#include "configLoad.h"
#include <yaml-cpp/yaml.h>

string g_nginx_ip;
int g_cgi_port;
int g_cgi_poolThreads;
int g_cgi_loopThreads;

string g_webServer_nginxIP;

string g_resource_base_path;
string g_resource_index_path;

string g_log_level;
string g_log_path;
long long g_log_maxSize;
int g_log_rollDaySeconds;
int g_log_rollZoneTime;
int g_log_IOFrequency;

string g_heartBeat_ip;
int g_heartBeat_port;
int g_heartBeat_frequency;


string g_db_server;
string g_db_user;
string g_db_pswd;
string g_db_dbName;

bool init(string &conf_path) {
    bool no_error = true;
    try {
        YAML::Node config = YAML::LoadFile(conf_path);
        g_nginx_ip = config["cgiServer"]["ip"].as<string>();
        g_cgi_port = config["cgiServer"]["port"].as<int>();
        g_cgi_poolThreads = config["cgiServer"]["poolThreads"].as<int>();
        g_cgi_loopThreads = config["cgiServer"]["loopThreads"].as<int>();
        g_webServer_nginxIP = config["webServer"]["nginxIP"].as<string>();

        g_log_level = config["log"]["level"].as<string>();
        g_log_path = config["log"]["path"].as<string>();
        g_log_maxSize = config["log"]["maxSize"]["G"].as<long long>() * 1024 * 1024 * 1024
                        + config["log"]["maxSize"]["M"].as<long long>() * 1024 * 1024
                        + config["log"]["maxSize"]["K"].as<int>() * 1024
                        + config["log"]["maxSize"]["B"].as<int>();
        g_log_rollZoneTime = config["log"]["rollTime"]["zoneTime"].as<int>();
        g_log_rollDaySeconds = config["log"]["rollTime"]["hour"].as<int>() * 3600
                               + config["log"]["rollTime"]["minute"].as<int>() * 60
                               + config["log"]["rollTime"]["second"].as<int>();
        g_log_IOFrequency = config["log"]["IOFrequency"].as<int>();

        g_heartBeat_ip = config["heartBeat"]["server"]["ip"].as<string>();
        g_heartBeat_port = config["heartBeat"]["server"]["port"].as<int>();
        g_heartBeat_frequency = config["heartBeat"]["frequency"].as<int>();

        g_resource_base_path = config["resource"]["basePath"].as<string>();
        g_db_server = config["dataBase"]["server"].as<string>();
        g_db_user = config["dataBase"]["user"].as<string>();
        g_db_pswd = config["dataBase"]["pswd"].as<string>();
        g_db_dbName = config["dataBase"]["dbName"].as<string>();
    } catch (exception &e) {
        cout << e.what() << endl;
        no_error = false;
    }

    return no_error;
}