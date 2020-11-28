#include "../Header_Files/Server.h"
#include "../Header_Files/Client.h"
#include "../Header_Files/RoutingTable.h"

#include <pthread.h>

using namespace std;

//GLOBAL VARIABLES
bool Mutex = false;
Client * C ;
RoutingTable R ;
Server * S ;

//FUNCTION PROTOTYPES
Server *Setup_Server_Connection();
Client *Setup_Client_Connection();
void Connect_To_Proxy_Server();
void Add_To_Routing_Table();
void * Rec_From_Proxy_Server(void * args);
void Wait_For_Connection();

int main()
{
    C = Setup_Client_Connection();

    //Establishing Initial Connection with Proxy Server
    Connect_To_Proxy_Server();

    //Receive Calls from Proxy Server
    pthread_t thread1 ;
    pthread_create(&thread1, NULL, Rec_From_Proxy_Server, NULL);

    S = Setup_Server_Connection();

    //Waiting For Connections With Client
    Wait_For_Connection();

    //Deallocating Memory
    delete S ;
    delete C ;

    return 0 ;
}

Server *Setup_Server_Connection()
{
    Server *S = new Server(5);

    S->Set_Tcp();

    int choice;
    string ip;
    int port;
    bool loop = true;
    while(loop)
    {
        cout << "Which Server Do You Want to Create (1,2,4) : ";
        cin >> choice ;
        loop = false;

        switch(choice)
        {
            case 1 : ip = "127.0.10.1";
                     port = 5001;
                     break;
            case 2 : ip = "127.0.10.2";
                     port = 5002;
                     break;
            case 4 : ip = "127.0.10.4";
                     port = 5004;
                     break;
            default: cout << "\nWrong Option Try Again\n" ;
                     loop = true;
                     break;
        }
    }

    S->Set_Port(port);
    S->Set_Ip(ip);
    S->Create_Socket();

    S->Bind();
    S->Listen(3);

    return S;
}

Client *Setup_Client_Connection()
{
    Client * C = new Client;

    C->Set_Tcp();
    C->Set_Port(5003);
    C->Set_Ip("127.0.10.3");
    C->Create_Socket();
    C->Connect();

    return C;
}

void Connect_To_Proxy_Server()
{
    string opener = "";
    C->Send(opener);
    cout << "Proxy Server Connection Response : " << C->Receive() << "\n" ;
}

void Add_To_Routing_Table()
{
    //generate a message to send to proxy server
    string rtr_message = "0" + to_string(S->Get_Client_Port()) + " " + S->Get_Server_IP() + " " + S->Get_Server_Port() ;

    C->Send(rtr_message) ;
}

//Thread To Receive Messages from Proxy Server
void * Rec_From_Proxy_Server(void * args)
{
    while(1)
    {
        //Wait for Receive From Proxy Server
        string message = C->Receive() ;

        //Proxy Server is in Contact, Aqquire Lock
        Mutex = true;

        //DO WORK
        cout << message << endl;

        //Release Lock
        Mutex = false;
    }
}

void Wait_For_Connection()
{
    int Temp_sd;
    string response, message;
    int max_clients = S->Get_Max_Clients() ;

    int ret_val ;

    while(1)
    {
        //Lock is Not Aqquired
        while(Mutex)
        {}

        ret_val = S->Select();

        //new client has been connected so add it to routing table
        if (ret_val == 0)
            Add_To_Routing_Table();

        //checking if existing client sent message
        for(int i=0 ; i< max_clients ; i++)
        {
            //Lock is Not Aqquired
            while(Mutex)
            {}

            Temp_sd = S->Get_Client_FD(i) ;

            if (S->Check_FD_Set(Temp_sd))
            {                 
                response = S->Receive(Temp_sd, i) ;

                if(response == "exit")
                    continue;

                cout << "Client IP(" << S->Get_Client_IP() << ") Port(" << S->Get_Client_Port() << ") : " << response << "\n" ;

                cout << "Server : " ;
                getline(cin,message) ;

                S->Send(message, Temp_sd);
            }
        }
    }
}