#include "RoutingTable.h"

RoutingTable::RoutingTable(){}

void RoutingTable::Add_To_Routing_Table(string data, bool directlycon)
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

    _Directly_Connected.push_back(directlycon);
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

            auto itr4 = _Directly_Connected.begin() + i ;
            _Directly_Connected.erase(itr4) ;

            break;
        }
    }
}

string RoutingTable::Get_RoutingTable()
{
    string rt = "\nClient Port\tServer Ip\tServer Port\n";

    for(int i=0 ; i<_Client_Port.size() ; i++)
    {
        rt += _Client_Port[i] + "\t" + _Server_IP[i] + "\t" + _Server_Port[i] + "\n" ;
    }

    return rt ;
}

bool RoutingTable::Does_Client_Exist(string cport)
{
    for(int i=0 ; i<_Client_Port.size() ; i++)
    {
        if(_Client_Port[i] == cport)
            return true;        
    }

    return false;
}

bool RoutingTable::Is_Directly_Connected(string cport)
{
    for(int i=0 ; i<_Client_Port.size() ; i++)
    {
        if(cport == _Client_Port[i] && _Directly_Connected[i])
            return true;
    }

    return false;
}