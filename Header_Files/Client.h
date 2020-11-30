#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <iostream>

#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sstream> 
#include <string.h>

#include <sys/types.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

class Client
{
    private:

        struct sockaddr_in _Server_Address;

        bool _Tcp_Flag;
        bool _Udp_Flag;
        int _Port_No;
        string _Server_Ip;
        int _Client_Socket;

    //FUNCTIONS
    private:

        void _Def_Server_Address();

    public:

        Client();

        ~Client();

        //Mandatory
        void Set_Tcp();

        //Mandatory
        void Set_Udp();

        //Mandatory
        void Set_Port(int);

        void Set_Ip(string);

        //Mandatory
        void Create_Socket();

        //Mandatory
        void Connect();

        void Send(string);

        string Receive();
};

#endif