#include "Client.h"

Client::Client()
{
    _Tcp_Flag = false;
    _Udp_Flag = false;

    _Server_Ip = "";
}

Client::~Client()
{
    close(_Client_Socket);
}

void Client::Set_Tcp()
{
    _Tcp_Flag = true;
}

void Client::Set_Udp()
{
    _Udp_Flag = true;
}

void Client::Set_Port(int p)
{
    assert(p > 0 && p < 65353);

    _Port_No = p;
}

void Client::Set_Ip(string ip)
{
    _Server_Ip = ip;
}

void Client::Create_Socket()
{
    if (_Tcp_Flag == true){
        _Client_Socket = socket(AF_INET, SOCK_STREAM, 0);
    }

    else if (_Udp_Flag == true){
        _Client_Socket = socket(AF_INET, SOCK_DGRAM, 0);
    }

    assert(_Client_Socket > -1);

    _Def_Server_Address();
}

void Client::_Def_Server_Address()
{
    if(_Udp_Flag == true){
        bzero(&_Server_Address, sizeof(_Server_Address));
    }

    _Server_Address.sin_family = AF_INET; //IPV4 FAMILY

    if (_Server_Ip == ""){
        _Server_Address.sin_addr.s_addr = INADDR_ANY;
    }
    else{
        _Server_Address.sin_addr.s_addr = inet_addr(_Server_Ip.c_str());
    }

    _Server_Address.sin_port = htons(_Port_No);
}

void Client::Connect()
{
    int ret ;
    ret = connect(_Client_Socket, (struct sockaddr *) &_Server_Address, sizeof(_Server_Address));

    assert(ret == 0);
}

void Client::Send(string message)
{
    if (_Tcp_Flag == true){
        send(_Client_Socket, message.c_str(), message.length(), 0);
    }

    else if (_Udp_Flag == true){
        sendto(_Client_Socket, message.c_str(), message.length(), 0,
               (struct sockaddr *)NULL, sizeof(_Server_Address));
    }
}

string Client::Receive()
{
    char buff[255] = {};

    if(_Tcp_Flag == true){
        int return_val = recv(_Client_Socket, &buff, sizeof(buff), 0);
        assert(return_val != -1);
    }

    else if (_Udp_Flag == true){
        int return_val = recvfrom(_Client_Socket, &buff, sizeof(buff), 0, (struct sockaddr*)NULL, NULL);
        assert(return_val != -1);
    }

    return string(buff);
}