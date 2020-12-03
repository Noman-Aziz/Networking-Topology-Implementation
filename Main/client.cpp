#include "../Header_Files/Client.h"

#include <pthread.h>

using namespace std;

//GLOBAL VARIABLES
Client *C ;
bool Mutex = false;
bool Closed = true;
string Dport ;

//FUNCTION PROTOTYPES
Client *Setup_Client_Connection();
void * Receive_Messages(void *);
void Send_Messages();
bool Does_Exist(string, string);
void Input_Validation(int &);

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

        //server connection successfull message
        if (first)
        {
            first = false;
            cout << "Server Connection Response : " ;
        }

        //Received Message From Another Client
        if(rec[0] == '2')
        {
            //(C2C First Time)
            if (Closed)
            {
                cout << "\n" ;
            }

            //PATHH Occupied (RECEIVED FROM PROXYSERVER)
            else if(Does_Exist(rec," Sorry, Link is Occupied"))
            {
                stringstream ss(rec.erase(0,1)) ;
                string temp ;

                cout << "Received Message From Server : " ; 
                
                //removing source and destination port from message
                ss >> temp ;
                ss >> temp ;

                while(ss >> temp)
                    cout << temp << " " ;
                cout << "\n" ;

                Closed = true;
                Mutex = true;
                Dport = "";
                continue;                
            }

            //Path Occupied (RECEIVED FROM SERVER)
            else if(Does_Exist(rec,"Link is Occupied"))
            {
                cout << "Received Message From Server : " << rec.erase(0,1) << "\n" ;
                Closed = true;
                Mutex = true;
                Dport = "";
                continue;
            }

            //Wrong Destination port Number
            else if (!Closed && (Does_Exist(rec,"No Client Exists") || Does_Exist(rec,"Messages to Yourself")))
            {
                Closed = true;
                cout << "Received Message From Server : " << rec.erase(0,1) << "\n";
                Mutex = true;
                Dport = "";
                continue;
            }

            //C2C Message (Conn Closed Message)
            if (!Closed && Does_Exist(rec,"closed"))
            {
                Closed = true;
                cout << "Received Message From Client " << Dport << " : Connection Closed\n" ;
                Dport = "";
                Mutex = true ;
                continue;
            }

            string temp = rec ;
            
            //removing code from message
            temp.erase(0,1) ;
            stringstream ss(temp);
            string response;

            //seperating dport and response from message
            ss >> Dport ;
            ss >> response ;
            
            cout << "Received Message From Client " << Dport << " : " ;
            while(ss >> response)
                cout << response << " " ;
            cout << "\n" ;
        }

        //dns server msg
        else if(rec[0] == '3')
        {
            rec.erase(0,2) ;

            stringstream ss(rec) ;
            string temp ;
            ss >> temp ;

            while(ss >> temp)
                cout << temp << " ";
            cout << endl ;
        }

        //Other Types of Messages
        else
        {
            cout << rec << "\n" ;
        }
        
        Mutex = true ;
    }
}

void Input_Validation(int & input)
{
    cin.clear ();
    cin.ignore( 50 , '\n' );
    cout << "Choice Must Be Integer, Try Again\n";
    cin >> input;
}

void Send_Messages()
{
    while(1)
    {
        Dport = "" ;

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

        while(!cin)
            Input_Validation(choice) ;

        string base_code = to_string(choice) ;

        //INCASE OF CLIENT TO CLIENT COMMUNICATION
        if(choice == 2 ){

            //First Time Sending Message
            if (Dport == "")
            {
                cout<<"Enter Port No of Client to Connect To : ";
                cin>>Dport;
            }

            cin.ignore();

            string base_code = "2";

            Closed = false;

            base_code = "2" + Dport;

            while(!Closed){
                
                string msg;
                cout << "Enter Message to Send To Client " << Dport << " ('closed' to stop): " ;
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

        //Dns server request
        else if (choice == 3)
        {
            string msg;
            cout << "Enter Name Website To Get Its Ip-Address (lowercase) : " ;
            cin >> msg ;
            cin.ignore() ;
            msg = base_code + msg;
            C->Send(msg);

            //Waiting For Response
            Mutex = false;
            while(!Mutex){}
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