#ifndef _ROUTINGTABLE_H_
#define _ROUTINGTABLE_H_

#include <vector>
#include <bits/stdc++.h>
#include <sstream> 

using namespace std;

class RoutingTable
{
    private:

        vector<string> _Client_Port ;
        vector<string> _Server_IP ;
        vector<string> _Server_Port ;
        vector<string> _Server_Name ;
        vector<bool> _Directly_Connected ;
        vector<int> _Hops ;

        //This is For the Proxy Server, It Tells The Proxy Server about the Server Port on Which The CLient Exists since The Port for Server is different in case of ProxyServer and Clients
        vector<int> _Clients_Server_Port ;

        //PERMENANT SERVER NAMES AND PORTS (DOESNT DELETED WHEN ROUTING TABLE GETS DELETED / USED IN PROXYSERVER)
        vector <string> _Perm_Server_Name ;
        vector <int> _Perm_Server_Port ;

    public:

        RoutingTable();

        //Parameter is "Client_Port Server_Ip Server_Port", is Client Directly Connected
        //Third Parameter is For ProxyServer (Default 0), It tells about the Server Port on ProxyServer Side
        void Add_To_Routing_Table(string, bool, int = 0);

        //Parameter is "Client_Port"
        void Delete_From_Routing_Table(string);     

        //Returns Routing Table
        string Get_RoutingTable() ;

        //Does client Exist
        bool Does_Client_Exist(string);

        //Is Client Directly Connected
        bool Is_Directly_Connected(string) ;

        //Used to Get ProxyServer Client's Port wrt Server's Client Port
        //Parameter is Client's Destination Port
        int Get_ProxyServer_Client_Port(string) ;

        //Used in ProxyServer to Get Server Name
        string Get_ServerName_From_Port(int) ;

        //Get ServerPort Against Its Name (Used in Proxy Server)
        int Get_ServerPort_From_Name(string) ;
};

#endif