#include "../Header_Files/Client.h"

#include <pthread.h>

using namespace std;

//GLOBAL VARIABLES
Client *C ;
bool Mutex = false;
bool Closed = true;

//FUNCTION PROTOTYPES
Client *Setup_Client_Connection();
void * Receive_Messages(void *);
void Send_Messages();
bool Does_Exist(string, string);

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
        string rec = C->Receive();

        //server connection message
        if (first)
        {
            first = false;
            cout << "Server Connection Response : " ;
        }

        else if (!Closed)
        {
            //Wrong Destination port Numeer,
            if(Does_Exist(rec,"No Client Exists"))
            {
                Closed = true;
                cout << rec << endl;
                Mutex = true;
                continue;
            }
        
            //Removing Message Code from Message
            rec.erase(0,1);
            //Extracting Source Port from Message
            stringstream ss(rec);
            string temp;
            ss >> temp ;
            cout << "Client (" << temp << ") Response : " ;
            //Removing Destination Port
            ss >> temp ;
            //Extracting Message
            ss >> temp ;
            rec = temp ;
        }

        //connection close message
        if (!Closed && Does_Exist(rec,"closed"))
        {
            Closed = true;
        }

        cout << rec << endl;

        Mutex = true ;
    }
}

void Send_Messages()
{
    while(1)
    {
        //for first server message
        sleep(1) ;

        cout<<"SELECT MESSAGE CODE:"<<endl;
        cout<<"1 :  Request routing table"<<endl;
        cout<<"2 :  Communicate With Client By Port Number"<<endl;
        cout<<"3 :  Request from DNS SERVER"<<endl;
        cout<<"4 :  Exit Client\n" ;
        cout<<"YOUR CHOICE : " ;

        int choice;
        cin>>choice;

        string base_code = to_string(choice);
        
            
        //INCASE OF CLIENT TO CLIENT COMMUNICATION
        if(choice == 2 ){

            string PORT;
            cout<<"ENTER PORT NUMBER OF CLIENT TO CONNECT TO : ";
            cin>>PORT;
            cin.ignore();

            Closed = false;

            base_code = "2" + PORT;

            while(!Closed){
                
                string msg;
                cout << "Enter Message to Send To Client : " ;
                getline(cin, msg);

                msg = base_code + " " + msg;

                C->Send(msg);

                if (Does_Exist(msg,"closed")){
                    Closed = true ;
                    break;
                }

                Mutex = false ;
                while(!Mutex){}
            }
        }

        else if(choice == 1)
        {
            C->Send(base_code) ;

            Mutex = false;
            while(!Mutex){}
        }

        //do later for number 3
        else if (choice == 3)
        {
            string msg;
            getline(cin, msg);
            msg = base_code + msg;
            C->Send(msg);
            if (msg == "exit")
            {
                return ;
            }
        }
        
        else if (choice == 4)
        {
            string msg = "exit";
            C->Send(msg);
            return;
        }

        //wrong choice
        else
        {
            cout << "\nWrong Choice Try Again\n";
            continue;
        }
        
    }
}

bool Does_Exist(string str1, string str2)
{
    size_t no_serial = str1.find(str2);

    if (no_serial != string::npos){
        return true;
    }

    else{
        return false;
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