#include "../Header_Files/Server.h"
#include "../Header_Files/RoutingTable.h"

using namespace std;

//Function Prototypes
Server *Setup_Server_Connection();
void Send_Broadcast_Message(Server *, string, int);
void Send_Client_Message(Server *, string, int, RoutingTable *);

int main()
{
    Server * S = Setup_Server_Connection();

    RoutingTable R ;

    int Temp_sd;
    string response, message;
    int max_clients = S->Get_Max_Clients() ;


    
    while(1)
    {
        S->Select(0);
        
        //checking if existing client sent message
        for(int i=0 ; i< max_clients ; i++)
        {
            Temp_sd = S->Get_Client_FD(i) ;

            if (S->Check_FD_Set(Temp_sd))
            {                 
                response = S->Receive(Temp_sd, i) ;
                int client_port = S->Get_Client_Port(i) ;

                //only one message and response with one client
                //if(response == "exit")
                //    continue;

                //cout << response << endl;

                //Checking Message Type and Acting Accordingly

                //Broadcast Add to RoutingTable Message
                if (response[0] == '0')
                {
                    //Send Message to Other Servers
                    Send_Broadcast_Message(S, response, i) ;
                
                    //Add to Own Routing Table
                    R.Add_To_Routing_Table(response.erase(0,1), false, client_port);

                    cout << "Received Broadcast Message From Server " << client_port << " To Add To RoutingTable : (Client_Port Server_IP Server_Port) " << response << "\n" ;
                }

                //Broadcast Delete From RoutingTable Message
                else if (response[0] == '5')
                {
                    //Send Message to Other Servers
                    Send_Broadcast_Message(S, response, i) ;
                
                    //Delete from Own Routing Table
                    R.Delete_From_Routing_Table(response.erase(0,1));
                
                    cout << "Received Broadcast Message From Server " << client_port << " To Delete From RoutingTable : " << response << "\n" ;
                }

                //Forward A Client Message To Any Client Via Server
                else if (response[0] == '2')
                {
                    Send_Client_Message(S, response, i, &R) ;

                    cout << "Received a Client Message From Server " << client_port << " To Forward to Server \n" ;//<< response << "\n" ;
                }
                //S->Send(message, Temp_sd);

                continue;
            }
        }
    }

    //Deallocating Memory
    delete S ;

    return 0 ;
}

Server *Setup_Server_Connection()
{
    Server *S = new Server(5);

    S->Set_Tcp();
    S->Set_Port(5003);

    S->Set_Ip("127.0.10.3");
    S->Create_Socket();

    S->Bind();
    S->Listen(3);

    return S;
}

void Send_Broadcast_Message(Server * S, string message, int Sender_Index)
{
    int max_clients = S->Get_Max_Clients() ;
    int Temp_sd;

    for(int i=0 ; i<max_clients ; i++)
    {
        if (i == Sender_Index)
            continue;

        Temp_sd = S->Get_Client_FD(i) ;

        if (Temp_sd != 0)
        {                 
            S->Send(message, Temp_sd);
        }
    }
}

void Send_Client_Message(Server * S, string message, int Sender_Index, RoutingTable * R)
{
    int max_clients = S->Get_Max_Clients() ;
    int Temp_sd;

    //extracting receiver port from message
    string temp = message;
    string dport ;
    temp.erase(0,1);

    stringstream ss(temp) ;
    ss >> dport ;
    ss >> dport ;

    //getting server's port wrt client's destination port
    int Sdport = R->Get_ProxyServer_Client_Port(dport) ;
    assert(Sdport != -1) ;
    
    for(int i=0 ; i<max_clients ; i++)
    {
        if (i == Sender_Index)
            continue;

        Temp_sd = S->Get_Client_FD(i) ;

        if (Temp_sd == S->Get_Fd_By_Port(Sdport))
        {                 
            S->Send(message, Temp_sd);
            return ;
        }
    }
}


