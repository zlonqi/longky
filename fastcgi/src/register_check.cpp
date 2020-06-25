#include <iostream>
#include <string>

#include <cgicc/Cgicc.h>

#include <mysql++/mysql++.h>

#include "../configLoad.h"

using namespace std;
using namespace mysqlpp;
using namespace cgicc;

string g_db_server;
string g_db_user;
string g_db_pswd;
string g_db_dbName;

bool init(){
    bool no_error = true;
    try {
        YAML::Node config = YAML::LoadFile("/root/web/CLionProject/muduo/fastcgi/config.yaml");
        g_db_server = config["dataBase"]["server"].as<string>();
        g_db_user = config["dataBase"]["user"].as<string>();
        g_db_pswd = config["dataBase"]["pswd"].as<string>();
        g_db_dbName = config["dataBase"]["dbName"].as<string>();
    }catch (exception &e) {
        cout << e.what() << endl;
        no_error = false;
    }
    return no_error;
}

int main ()
{
    if(!init()){
        printf("configuration error, syntax or path");
        return 0;
    }

    Cgicc formData;
    form_iterator userName = formData.getElement("userName");

    bool exsit = false;
    Connection con(true);
    try {
        con.connect(g_db_dbName.c_str(), g_db_server.c_str(), g_db_user.c_str(), g_db_pswd.c_str());
        string s = "SELECT * from user_pwd where userName='"
                   + **userName  + "';";
        Query q = con.query(s);
        if (q.store().size() > 0)
            exsit = true;
    } catch (Exception &e) {
        cout << e.what() << endl;
    }
    con.disconnect();

    cout << "HTTP/1.0 200 OK\r\n";
    cout << "Content-type:text/html;charset=utf-8 \r\n\r\n";
    if(exsit){
       cout << "yes";
    }else{
        cout << "no";
    }
    return 0;
}