#ifndef _ROUTINGTABLE_H_
#define _ROUTINGTABLE_H_

#include <vector>
#include <bits/stdc++.h>

using namespace std;

class RoutingTable
{
    private:

        vector<string> _Client_Port ;
        vector<string> _Server_IP ;
        vector<string> _Server_Port ;
        vector<bool> _Directly_Connected ;

    public:

        RoutingTable();

        //Parameter is "Client_Port Server_Ip Server_Port", is Client Directly Connected
        void Add_To_Routing_Table(string, bool);

        //Parameter is "Client_Port"
        void Delete_From_Routing_Table(string);     

        //Returns Routing Table
        string Get_RoutingTable() ;

        //Does client Exist
        bool Does_Client_Exist(string);

        //Is Client Directly Connected
        bool Is_Directly_Connected(string) ;
};

#endif