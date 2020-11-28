#include "RoutingTable.h"

RoutingTable::RoutingTable(){}

void RoutingTable::Add_To_Routing_Table(string data)
{
    //split around spaces
    istringstream ss(data);

    string temp;

    ss >> temp ;
    _Client_Port.push_back(temp);

    ss >> temp ;
    _Server_IP.push_back(temp);

    ss >> temp ;
    _Server_Port.push_back(temp);
}

void RoutingTable::Delete_From_Routing_Table(string port)
{
    for(int i=0 ; i < _Client_Port.size() ; i++ )
    {
        if (_Client_Port[i] == port)
        {
            auto itr = _Client_Port.begin() + i ;
            _Client_Port.erase(itr);

            auto itr2 = _Server_IP.begin() + i ;
            _Server_IP.erase(itr2);

            auto itr3 = _Server_Port.begin() + i ;
            _Server_Port.erase(itr3) ;

            break;
        }
    }
}