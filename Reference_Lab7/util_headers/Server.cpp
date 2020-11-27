#include "Server.h"

Server::Server(int max_clients)
{
    _Tcp_Flag = false;
    _Udp_Flag = false;

    _Server_Ip = "";

    assert(max_clients > 0);

    _Max_Clients = max_clients;

    _Client_Fds = new int[max_clients]; 
}

Server::~Server()
{
    close(_Master_Socket);

    if (_Max_Clients == 1)
        delete _Client_Fds ;
    else
        delete [] _Client_Fds ;        
}

void Server::Set_Tcp()
{
    _Tcp_Flag = true;
    _Initialize_Client_FDs();
}

void Server::_Initialize_Client_FDs()
{
    for(int i=0 ; i < _Max_Clients ; i++ )
    {
        _Client_Fds[i] = 0 ;
    }
}

void Server::Set_Udp()
{
    _Udp_Flag = true;
}

void Server::Set_Port(int p)
{
    assert(p > 0 && p < 65353);

    _Port_No = p;
}

void Server::Set_Ip(string ip)
{
    _Server_Ip = ip;
}

void Server::Create_Socket()
{
    if (_Tcp_Flag == true){
        _Master_Socket = socket(AF_INET, SOCK_STREAM, 0);
    }
    else if (_Udp_Flag == true){
        _Master_Socket = socket(AF_INET, SOCK_DGRAM, 0);
    }

    _Def_Server_Address();

    assert(_Master_Socket > -1);

}

void Server::_Def_Server_Address()
{
    if(_Udp_Flag == true){
        bzero(&_Server_Address, sizeof(_Server_Address));
    }

    _Server_Address.sin_family = AF_INET; //IPV4 FAMILY

    if (_Server_Ip == "")
        _Server_Address.sin_addr.s_addr = INADDR_ANY;
    else
        _Server_Address.sin_addr.s_addr = inet_addr(_Server_Ip.c_str());

    _Server_Address.sin_port = htons(_Port_No);
}

void Server::Bind()
{
    assert ( bind(_Master_Socket, (struct sockaddr *)&_Server_Address, sizeof(_Server_Address)) >= 0 );
    cout << "Listening on Port " << _Port_No << endl;
}

void Server::Listen(int no)
{
    assert ( listen(_Master_Socket, no) >= 0 );
}

int Server::Accept()
{
    int len = sizeof(_Client_Socket) ;
    return accept(_Master_Socket, (struct sockaddr*)&_Client_Socket , (socklen_t *) &len);
}

void Server::Send(string message, int tcp_cli_sock)
{
    if (tcp_cli_sock == -1)
        tcp_cli_sock = _Current_Client_Fd ;

    if (_Tcp_Flag == true){
        send(tcp_cli_sock, message.c_str(), message.length(), 0);

    }
}

void Server::Send(string message, struct sockaddr_in &udp_cli_sock)
{
    if (_Udp_Flag == true){
        sendto(_Master_Socket, message.c_str(), message.length(), 0,
               (struct sockaddr *)&udp_cli_sock, sizeof(udp_cli_sock));
    }
}

string Server::Receive(int client, int index)
{
    char buff[1000] = {};

    if(_Tcp_Flag == true){

        //get data from current tcp client
        if (client == -1)
            client = _Current_Client_Fd ;

        read(client, &buff, sizeof(buff) );

        //somebody disconnected, close socket and mark its pos 0 for reuse
        if (string(buff) == "exit")
        {
            close( client ) ;
            _Client_Fds[index] = 0 ;
        }
    }

    else if (_Udp_Flag == true){
        int len = sizeof(_Client_Socket);
        int return_val = recvfrom(_Master_Socket, &buff, sizeof(buff), 0, (struct sockaddr*)&_Client_Socket, (socklen_t *)&len);
        assert(return_val != -1);
    }

    return string(buff);
}

struct sockaddr_in & Server::Get_Client_Socket()        
{
    return _Client_Socket;
}

int Server::Get_Client_Port()
{
    return (int) ntohs(_Client_Socket.sin_port);
}

string Server::Get_Client_IP()
{
    return inet_ntoa(_Client_Socket.sin_addr);
}

void Server::Clear_Socket_Set()
{
    FD_ZERO(&_Readfd);
}

void Server::Add_Master_Socket()
{
    FD_SET( _Master_Socket, &_Readfd );
    _Max_Sd = _Master_Socket ;
}

void Server::Add_Child_FDs()
{
    int _Temp_Sd ;

    for(int i=0 ; i < _Max_Clients ; i++ )
    {
        _Temp_Sd = _Client_Fds[i];

        //if valid socket descriptor then add to read set
        if(_Temp_Sd > 0)
            FD_SET( _Temp_Sd, &_Readfd );

        //rechoose the highest file descriptor number, we need it for select call
        if (_Temp_Sd > _Max_Sd)
            _Max_Sd = _Temp_Sd ;
    }
}

void Server::Select()
{
    Clear_Socket_Set();
    Add_Master_Socket();
    Add_Child_FDs();

    //Wait Infinitely For New Connection
    assert( select( _Max_Sd + 1, &_Readfd, NULL, NULL, NULL ) != -1 ) ;

    //checking if new connection on master socket
    if(FD_ISSET( _Master_Socket, &_Readfd ))
    {
        _Current_Client_Fd = Accept();

        assert(_Current_Client_Fd != -1);

        cout << "New Client Connected To Server\n" ;

        //Sending Welcome Message to Newly Connected Client
        string message = "You have been Successfully Connected to Server";
        Send(message);

        //add new socket to client sockets
        for(int i=0 ; i<_Max_Clients ; i++)
        {
            //empty position
            if( _Client_Fds[i] == 0 )
            {
                _Client_Fds[i] = _Current_Client_Fd ;
                break;
            }
        }
    }
}

int Server::Get_Client_FD(int index)
{
    //get current client
    if(index == -1)
        return _Current_Client_Fd ;

    assert(index > -1 && index < _Max_Clients ) ;

    //get specific client
    return _Client_Fds[index] ;
}

int Server::Get_Max_Clients()
{
    return _Max_Clients ;
}

bool Server::Check_FD_Set(int client)
{
    return FD_ISSET( client, &_Readfd  ) ;
}
