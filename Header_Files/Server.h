#ifndef _SERVER_H_
#define _SERVER_H_

#include <iostream>

#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include <string.h>
#include <sstream> 

#include <sys/types.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

using namespace std;

class Server
{
    //VARIABLES
    private:

        struct sockaddr_in _Server_Address;
        struct sockaddr_in * _Client_Socket;

        bool _Tcp_Flag;
        bool _Udp_Flag;
        int _Port_No;
        string _Server_Ip;

        int * _Client_Fds, _Max_Clients, _Master_Socket, _New_Socket, _Max_Sd ;
        int * _Client_Ports ;
        int _Currently_Connected_Index ;

        //current client file descriptor
        int _Current_Client_Fd;

        //set of socket descriptors  
        fd_set _Readfd;   

    //FUNCTIONS
    private:

        //For Server Socket Details
        void _Def_Server_Address();

        //It initializes all the File Descriptors to 0 So they No Client is Connected on Them
        void _Initialize_Client_FDs();

        //It Clears the set of Socket File Descriptors
        void Clear_Socket_Set();

        //It Adds Master Socket to Set and Assign Max FD to Master Socket
        void Add_Master_Socket();

        //It Adds All the Child FDs to Socket FD Set
        void Add_Child_FDs();

        //For Tcp Connections to Connect a Client, Returns the Client Socket FD
        int Accept();
        
    //MANDATORY FUNCTIONS
    public:

        //1 is default maximum clients
        Server(int = 1);

        ~Server();

        //Set Connection Type to TCP
        void Set_Tcp();

        //Set Connection Type to UDP
        void Set_Udp();

        //Set Port No
        void Set_Port(int);

        //Set IP (OPT)
        void Set_Ip(string);

        //For Creating Server Socket
        void Create_Socket();

        //Binding Socket for TCP Connection
        void Bind();

        //Listening on Socket For Tcp Connections, Parameter is Maximum No of Users That Can Connect to Server (Default 1)
        void Listen(int = 1);

        //For Concurrent Tcp Requests, Parameter is TimeoutValue (0 for Infinite Wait)
        //Returns 0 For New Connection
        //Returns 1 For Old Connection
        //Returns 2 For Timeout on Socket
        int Select(int);

        //Returns Server IP Address
        string Get_Server_IP() ;

        //Returns Server Port
        string Get_Server_Port() ;

    //UTILITY FUNCTIONS
    public:

        //Checks if Client is Mapped on this File Descriptor
        bool Check_FD_Set(int);

        //TCP SEND : second parameter tells which client to send
        void Send(string, int = -1);

        //UDP SEND : second parameter tells which client to send
        void Send(string,struct sockaddr_in &);
        
        //This Function is Used to Receive Messages from Clients
        string Receive(int = -1, int = -1);

        //This Returns Required Client Socket
        struct sockaddr_in & Get_Client_Socket(int);        

        //This Returns Mapped File Descriptor for Given Client (Default for Current Connected Client)
        int Get_Client_FD(int = -1);

        //This is Used to Get the Maximum Clients Limit
        int Get_Max_Clients();

        //This is Used to Get Connected Client Port
        int Get_Client_Port(int = -1);

        //This is Used to Get Connected Client IP
        string Get_Client_IP(int = -1);

        //Used to Get Client File Descriptor From Their Port
        int Get_Fd_By_Port(int);
};

#endif