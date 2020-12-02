#include "../Header_Files/Server.h"
#include "../Header_Files/Client.h"
#include "../Header_Files/RoutingTable.h"

#include <pthread.h>

using namespace std;

//GLOBAL VARIABLES
bool Mutex = false;
bool Connection_Occupied = false ;
int Connection_Occupied_By = 0;
bool Internally_Connected = false ;
string Server_Letter ;
int DNS_Server_Port ;

Client *C;
RoutingTable R;
Server *S;

//FUNCTION PROTOTYPES
Server *Setup_Server_Connection();
Client *Setup_Client_Connection();
void Connect_To_Proxy_Server();
void Add_To_Routing_Table(string);
void *Rec_From_Proxy_Server(void *args);
void Wait_For_Connection();
void Check_Response_Type(string, int, int);
void Delete_From_Routing_Table(int);
bool Does_Exist(string, string);

int main()
{
    C = Setup_Client_Connection();

    //Establishing Initial Connection with Proxy Server
    Connect_To_Proxy_Server();

    //Receive Calls from Proxy Server
    pthread_t thread1;
    pthread_create(&thread1, NULL, Rec_From_Proxy_Server, NULL);

    S = Setup_Server_Connection();

    //Waiting For Connections With Client
    Wait_For_Connection();

    //Deallocating Memory
    delete S;
    delete C;

    return 0;
}

Server *Setup_Server_Connection()
{
    Server *S = new Server(5);

    S->Set_Tcp();

    int choice;
    string ip;
    int port;
    bool loop = true;
    while (loop)
    {
        cout << "Which Server Do You Want to Create (1,2,4) : ";
        cin >> choice;
        loop = false;

        switch (choice)
        {
        case 1:
            ip = "127.0.10.1";
            port = 5001;
            Server_Letter = "1" ;
            break;
        case 2:
            ip = "127.0.10.2";
            port = 5002;
            Server_Letter = "2" ;
            break;
        case 4:
            ip = "127.0.10.4";
            port = 5004;
            Server_Letter ="4" ;
            break;
        default:
            cout << "\nWrong Option Try Again\n";
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
    Client *C = new Client;

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
    cout << "Proxy Server Connection Response : " << C->Receive() << "\n";
}

void Add_To_Routing_Table()
{
    //generate a message to send to proxy server
    string code = "0" ;
    string message = Server_Letter + " " + to_string(S->Get_Client_Port()) + " " + S->Get_Server_IP() + " " + S->Get_Server_Port();
    string rtr_message = code + message;

    //Adding to Own RoutingTable
    R.Add_To_Routing_Table(message, true);

    C->Send(rtr_message);
}

void Delete_From_Routing_Table(int index)
{
    string cport = to_string(S->Get_Client_Port(index));

    //delete from self routingtable
    R.Delete_From_Routing_Table(cport);

    //generate a message to send to proxy server
    string code = "5";
    string rtr_message = code + to_string(S->Get_Client_Port(index));

    C->Send(rtr_message);
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

//Thread To Receive Messages from Proxy Server
void *Rec_From_Proxy_Server(void *args)
{
    while (1)
    {
        //Wait for Receive From Proxy Server
        string message = C->Receive();

        //Proxy Server is in Contact, Aqquire Lock
        Mutex = true;

        //Checking Message Type and Applying Appropiate Operation

        //Broadcast Add to RoutingTable Msg
        if (message[0] == '0')
        {
            R.Add_To_Routing_Table(message.erase(0, 1), false);
            cout << "Received Broadcase Message from ProxyServer\nAdded an Entry to Routing Table : " << message << "\n";
        }

        //Broadcast Receive From Routing Table
        else if (message[0] == '5')
        {
            R.Delete_From_Routing_Table(message.erase(0, 1));
            cout << "Received Broadcase Message from ProxyServer\nDeleted an Entry to Routing Table : " << message << "\n";
        }

        //Received Message From ProxyServer about DNS Query
        else if(message[0] == '3')
        {
            //Checking Whether This is Response from A Client to Send to ProxyServer

            //To Do this i will check whether destination port is in directly connected client or not
            string temp = message;
            temp.erase(0,1) ;
            string dport ;
            stringstream ss(temp) ;
            ss >> dport ;
            int d_fd;
            
            //scenario 1 (it is for directly connected client)
            if(R.Is_Directly_Connected(dport))
            {
                cout << "Received Message from ProxyServer of Dns Request to Forward to Client " << dport << "\n";
                
                //Forward Message To Appropiate Connected Client Port
                d_fd = S->Get_Fd_By_Port(atoi(dport.c_str())) ;
            }

            //scenario 2 (i am s4, so forward to dns server)
            else
            {
                cout << "Received Message of Client " << dport << " through ProxyServer to Send To Dns Server\n" ;

                //Forward Message To Appropiate Connected Client Port
                d_fd = S->Get_Fd_By_Port(DNS_Server_Port) ;
            }

            assert(d_fd != -1) ;
            S->Send(message, d_fd) ;

            //Release Lock
            Mutex = false;
    
            continue;                
        }

        //Received Message From ProxyServer about a Client Communication
        else if (message[0] == '2')
        {
            //CONNECTION CLOSED MESSAGE | FREE CONECTION LINE
            if(Does_Exist(message, "closed"))
            {
                Connection_Occupied_By = 0;
                Connection_Occupied = false;
            }

            string temp = message;
            temp.erase(0, 1);
            stringstream ss(temp);

            string cport, dport;

            //Extracting Source Port From Message
            ss >> cport;
            //Extracting Destination Port From Message
            ss >> dport;

            //Connection is Occupied (e.g C1 and C2 of S1 are Talking To Each Other)
            if(Internally_Connected)
            {
                string msg = "2" + dport + " " + cport + " Sorry, Link is Occupied by Client " + to_string(Connection_Occupied_By) + ", Cannot Send Message To Another Client\n" ;
                C->Send(msg) ;
                cout << "Received Connection Requesest from " << cport << ", But Denied It Since Connection is Already Occupied by Client " << Connection_Occupied_By << "\n" ;
        
                //Release Lock
                Mutex = false;
        
                continue;
            }

            //Link is Occupied
            if(Does_Exist(message, "Link is Occupied"))
            {
                Connection_Occupied_By = 0 ;
                Connection_Occupied = false ;
                cout << "Received Message from ProxyServer For Client " << dport << "\n";
            }

            else
                cout << "Received Message from Client " << cport << " Through ProxyServer For Client " << dport << "\n";
        
            //Forward Message To Appropiate Connected Client Port
            int d_fd = S->Get_Fd_By_Port(atoi(dport.c_str())) ;
            assert(d_fd != -1) ;
            S->Send(message, d_fd) ;
        }

        //Release Lock
        Mutex = false;
    }
}

//It Checks Response From Client
void Check_Response_Type(string response, int Temp_sd, int index)
{
    //request for routing table
    if (response[0] == '1')
    {
        cout << "Client IP(" << S->Get_Client_IP(index) << ") Port(" << S->Get_Client_Port(index) << ") Has Requested for RoutingTable\n";

        string message = R.Get_RoutingTable();

        S->Send(message, Temp_sd);
    }

    //closing connection
    else if (response == "exit")
    {
        cout << "Client IP(" << S->Get_Client_IP(index) << ") Port(" << S->Get_Client_Port(index) << ") Has Closed the Connection with Server\n";
        Delete_From_Routing_Table(index);
    }

    //client to client chat
    else if (response[0] == '2')
    {
        //First Message
        if(!Connection_Occupied)
        {
            Connection_Occupied = true;
            Connection_Occupied_By = S->Get_Client_Port(index) ;
        }

        else if(Connection_Occupied_By && Internally_Connected)
            Connection_Occupied_By = S->Get_Client_Port(index) ;

        //If Path is Occupied, Cannot Send Message
        if(Connection_Occupied && Connection_Occupied_By != S->Get_Client_Port(index) && !Internally_Connected)
        {
            string msg = "2Sorry, Link is Occupied by Client " + to_string(Connection_Occupied_By) + ", Cannot Send Message To Another Client\n" ;
            S->Send(msg);
            return ;
        }

        //CONNECTION CLOSED MESSAGE | FREE CONECTION LINE
        if(Does_Exist(response, "closed"))
        {
            Connection_Occupied_By = 0 ;
            Connection_Occupied = false ;
            Internally_Connected = false ;
        }

        //Extracting Destination Port From Message
        string temp = response;
        temp.erase(0,1) ;

        stringstream ss(temp);
        string dport ;
        ss >> dport ;

        response = "2" + to_string(Connection_Occupied_By) + " " + temp ;

        cout << "Client IP(127.0.0.1) Port(" << Connection_Occupied_By << ") Has Sent Message To Client " << dport<< "\n";

        if (!R.Does_Client_Exist(dport))
        {
            temp = "2No Client Exists With That Port Number\n";
            S->Send(temp, Temp_sd);
            cout << "Connection Establishment Failed Due To Invalid Destination Port Number\n";
           
            Connection_Occupied = false;
            Connection_Occupied_By = 0 ;
            
        }

        //Cannot Send Message to Itself
        else if (Connection_Occupied_By == atoi(dport.c_str()) && !Internally_Connected)
        {
            cout << "Connection Establishment Failed Due To Sender Sending Messages to Itself\n";
        
            Connection_Occupied = false;
            Connection_Occupied_By = 0 ;
            

            temp = "2You Cannot Send Messages to Yourself\n" ;
            S->Send(temp, Temp_sd) ;
        }

        //Sending Message to Other Directly Connected Client
        else if (R.Is_Directly_Connected(dport))
        {
            if(Connection_Occupied)
                Internally_Connected = true ;

            for(int i=0 ; i<S->Get_Max_Clients() ; i++)
            {
                Temp_sd = S->Get_Client_FD(i) ;

                if (Temp_sd == S->Get_Fd_By_Port(atoi(dport.c_str())))
                {                 
                    S->Send(response, Temp_sd);
                    return ;
                }
            }
        }

        //Forward Message to ProxyServer
        else
        {
            C->Send(response);
        }
    }

    //DNS Server Active Msg
    else if(Does_Exist(response,"DNS SERVER"))
    {
        DNS_Server_Port = S->Get_Client_Port(index) ;
        Delete_From_Routing_Table(index) ;
    }

    //Request From DNS Server
    else if ( response[0] == '3' && response[1] != 'R' )
    {
        cout << "Client IP(" << S->Get_Client_IP(index) << ") Port(" << S->Get_Client_Port(index) << ") Has Requested Ip Address of a Website From DNS Server\n";

        //Preparing The Message Format
        response.erase(0,1) ;
        response = "3" + to_string(S->Get_Client_Port(index)) + " " + response ;

        C->Send(response) ;
    }

    //Scenario 3 DNS (Message Received From DNS Server)
    else if(response[0] == '3' && response[1] == 'R')
    {
        cout << "A Query Response From DNS Server Received To Forward to ProxyServer" ;

        C->Send(response) ;
    }

    //Scenario 4 (I am Server 4 : Received Message From Client to Send to DNS Server)
    //{

    //}
}

void Wait_For_Connection()
{
    int Temp_sd;
    string response, message;
    int max_clients = S->Get_Max_Clients();

    int ret_val;

    while (1)
    {
        //Lock is Not Aqquired
        while (Mutex)
        {
        }

        ret_val = S->Select(2);

        //new client has been connected so add it to routing table
        if (ret_val == 0)
        {
            Add_To_Routing_Table();
        }

        //Timeout Occured on Select
        else if (ret_val == 2)
        {
            //cout << "TIMEOUT OCCURED\n";
            continue;
        }

        //checking if existing client sent message
        for (int i = 0; i < max_clients; i++)
        {
            Temp_sd = S->Get_Client_FD(i);

            if (S->Check_FD_Set(Temp_sd))
            {
                response = S->Receive(Temp_sd, i);

                //CHECK RESPONSE TYPE
                Check_Response_Type(response, Temp_sd, i);
            }
        }
    }
}