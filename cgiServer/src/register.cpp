#include <iostream>
#include <string>

#include <cgicc/Cgicc.h>

#include <mysql++/mysql++.h>

#include "../configLoad.h"

using namespace std;
using namespace mysqlpp;
using namespace cgicc;

string g_webServer_nginxIP;
string g_db_server;
string g_db_user;
string g_db_pswd;
string g_db_dbName;

bool init(){
    bool no_error = true;
    try {
        YAML::Node config = YAML::LoadFile("../config.yaml");
        g_webServer_nginxIP = config["webServer"]["nginxIP"].as<string>();
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
    form_iterator userName = formData.getElement("user");

    form_iterator passwd= formData.getElement("pw");

    form_iterator email= formData.getElement("mail");

    Connection con(true);
    try {
        con.connect(g_db_dbName.c_str(), g_db_server.c_str(), g_db_user.c_str(), g_db_pswd.c_str());
        //cout << "Connected to database\n";
        //string s = "SELECT * FROM user_pwd WHERE id=1";
        string s = "INSERT into user_pwd(userName,pswd,email) VALUES('"
                +**userName +"','"+**passwd +"','" + **email + "');";
        Query q = con.query();
        q << s;
        q.execute();
    } catch (Exception &e) {
        cout << e.what() << endl;
    }
    con.disconnect();

    cout << "HTTP/1.0 302 Found\r\n";
    cout << "Location:http://";
    cout << g_webServer_nginxIP;
    cout << "/index.html\r\n";
    cout << "Content-type:text/html;charset=utf-8 \r\n\r\n";
    cout << "<html>\n";
    cout << "<body>\n";
    cout << "<br/>\n";
    cout << "</body>\n";
    cout << "</html>\n";
    return 0;
}
