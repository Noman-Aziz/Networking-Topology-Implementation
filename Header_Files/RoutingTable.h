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
        vector<bool> _Directly_Connected ;

        //This is For the Proxy Server, It Tells The Proxy Server about the Server Port on Which The CLient Exists since The Port for Server is different in case of ProxyServer and Clients
        vector<int> _Clients_Server_Port ;

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
};

#endif