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

    public:

        RoutingTable();

        //Parameter is "Client_Port Server_Ip Server_Port"
        void Add_To_Routing_Table(string);

        //Parameter is "Client_Port"
        void Delete_From_Routing_Table(string);     
};

#endif