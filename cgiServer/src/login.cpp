#include "../configLoad.h"

#include <iostream>
#include <string>

#include <cgicc/Cgicc.h>
#include<yaml-cpp/yaml.h>

#include <mysql++/mysql++.h>

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

    bool redirection= true;
    Connection con(true);
    try {
        con.connect(g_db_dbName.c_str(), g_db_server.c_str(), g_db_user.c_str(), g_db_pswd.c_str());
        string s = "SELECT * from user_pwd where userName='"
                   +**userName +"' and pswd='"+**passwd + "';";
        Query q = con.query(s);
        if(q.store().size() > 0)
            redirection = false;
    } catch (exception &e) {
        cout << e.what() << endl;
    }
    con.disconnect();

    if(redirection){
        string url = "Location:http://" + g_webServer_nginxIP + "/pages/logIn.html\r\n";
        cout << "HTTP/1.0 302 Found\r\n";
        cout << url;
        cout << "Content-type:text/html;charset=utf-8 \r\n\r\n";
    }else{
        string url = "<img src='http://" + g_webServer_nginxIP + "/pages/images/login/programer_wall_5.png' width='100%' height='100%'/>";
        cout << "HTTP/1.0 200 OK\r\n";
        cout << "Content-type:text/html;charset=utf-8 \r\n\r\n";
        cout << "<html>\n";
        cout << "<head>\n";
        cout << "<meta charset='utf-8'>\n";
        cout << "<title>private base</title>\n";
        cout << "<style type='text/css'>\n";
        cout << "*{margin:0;padding:0}\n";
        cout << "div{position:relative;}\n";
        cout << "div p{position:absolute;left:10px;top:10px;color:#CC33CC;font-size:medium;font-family:'微软雅黑'}\n";
        cout << "div span{position:absolute;left:360px;top:10px;color:#7CFC00;font-size:medium;font-family:'微软雅黑'}\n";
        cout << "</style>\n";
        cout << "</head>\n";
        cout << "<body>\n";
        cout << "<div>\n";
        cout << url;
        cout << "<p>Login success! welcome to your Newland! </p>\n";
        cout << "<span>";
        cout << **userName;
        cout << "</span>\n";
        cout << "</div>\n";
        cout << "</body>\n";
        cout << "</html>\n";


        /*cout << "<div style='background-color: #000000;width=100%'>";
        cout << "<center style='color: #CC33CC;font-size: medium;font-family:'微软雅黑''>Login Success! Welcome to the Newland, ";
        cout << **userName;
        cout << "!</center>";
        cout << "</div>\n";
        cout << "<div id='Layer1' style='position:absolute; left:0px; top:22px; width:100%; height:100%'>";
        cout << url;
        cout << "</div>\n";*/
    }

    return 0;
}
