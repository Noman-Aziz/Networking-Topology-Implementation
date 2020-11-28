#include "../Header_Files/Client.h"

#include <pthread.h>

using namespace std;

//GLOBAL VARIABLES
Client *C ;

//FUNCTION PROTOTYPES
Client *Setup_Client_Connection();
void * Receive_Messages(void *);
void Send_Messages();

int main()
{
    C = Setup_Client_Connection();

    pthread_t thread1 ;
    pthread_create(&thread1, NULL, Receive_Messages, NULL);

    Send_Messages() ;

    return 0;
}

void * Receive_Messages(void * args)
{
    bool first = true;

    for (;;)
    {
        //server connection message
        if (first)
        {
            first = false;
            cout << "Server Connection Response : " ;
        }

        string rec = C->Receive();
        cout << rec << endl;
    }
}

void Send_Messages()
{
    while(1)
    {
        string msg;
        getline(cin, msg);
        C->Send(msg);
        if (msg == "exit")
        {
            return ;
        }
    }
}

Client *Setup_Client_Connection()
{
    Client *C = new Client;

    int choice;
    string ip;
    int port;
    bool loop = true;
    while (loop)
    {
        cout << "Which Server Do You Want to Connect To (1,2,4) : ";
        cin >> choice;
        loop = false;

        switch (choice)
        {
        case 1:
            ip = "127.0.10.1";
            port = 5001;
            break;
        case 2:
            ip = "127.0.10.2";
            port = 5002;
            break;
        case 4:
            ip = "127.0.10.4";
            port = 5004;
            break;
        default:
            cout << "\nWrong Option Try Again\n";
            loop = true;
            break;
        }
    }

    C->Set_Ip(ip);
    C->Set_Port(port);
    C->Set_Tcp();
    C->Create_Socket();
    C->Connect();

    return C;
}