#include "RoutingTable.h"

RoutingTable::RoutingTable(){}

void RoutingTable::Add_To_Routing_Table(string data, bool directlycon, int CServerPort)
{
    //split around spaces
    istringstream ss(data);

    string temp;
    string name ;

    ss >> name ;
    _Server_Name.push_back(name);

    ss >> temp ;
    _Client_Port.push_back(temp);

    ss >> temp ;
    _Server_IP.push_back(temp);

    ss >> temp ;
    _Server_Port.push_back(temp);

    _Directly_Connected.push_back(directlycon);

    _Clients_Server_Port.push_back(CServerPort) ;

    bool already = false;
    for(int i=0 ; i<_Perm_Server_Port.size() ; i++ )
    {
        if(CServerPort == _Perm_Server_Port[i])
            already = true ;
    }

    if(!already)
    {
        _Perm_Server_Port.push_back(CServerPort) ;
        _Perm_Server_Name.push_back(name) ;
    }
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

            auto itr5 = _Clients_Server_Port.begin() + i ;
            _Clients_Server_Port.erase(itr5) ;

            auto itr6 = _Server_Name.begin() + i ;
            _Server_Name.erase(itr6) ;

            break;
        }
    }
}

string RoutingTable::Get_RoutingTable()
{
    string rt = "\n********************************************************\n";
    rt += "Server Name\tServer Ip\tServer Port\tClient Port\tDirectly Connected\n";

    for(int i=0 ; i<_Client_Port.size() ; i++)
    {
        rt += _Server_Name[i] + "\t" + _Server_IP[i] + "\t" + _Server_Port[i] + "\t" + _Client_Port[i] + "\t" + to_string(_Directly_Connected[i]) + "\n" ;
    }

    rt += "********************************************************\n";

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

int RoutingTable::Get_ProxyServer_Client_Port(string dport)
{
    for(int i=0 ; i<_Client_Port.size() ; i++)
    {
        if(dport == _Client_Port[i])
            return _Clients_Server_Port[i] ;
    }

    return -1 ;
}

string RoutingTable::Get_ServerName_From_Port(int port)
{
    for(int i=0 ; i<_Perm_Server_Name.size() ; i++)
    {
        if(port == _Perm_Server_Port[i])
            return _Perm_Server_Name[i] ;
    }

    cout << "CRITICAL SYSTEM FAILURE1\n" ;
    exit(1) ;
}

int RoutingTable::Get_ServerPort_From_Name(string name)
{
    for(int i=0 ; i<_Perm_Server_Port.size() ; i++)
    {
        if(name == _Perm_Server_Name[i])
            return _Perm_Server_Port[i];
    }

    cout << "CRITICAL SYSTEM FAILURE2\n" ;
    exit(1) ;
}
