#include "../Header_Files/Server.h"
#include "../Header_Files/RoutingTable.h"

using namespace std;

//Function Prototypes
Server *Setup_Server_Connection();
void Send_Broadcast_Message(Server *, string, int);
void Send_Client_Message(Server *, string, int, RoutingTable *, int &, bool);
void Send_Con_Occupied_Msg(Server *, string, int) ;
bool Does_Exist(string, string);

int main()
{
    Server * S = Setup_Server_Connection();

    RoutingTable R ;

    int Temp_sd;
    string response, message;
    int max_clients = S->Get_Max_Clients() ;

    bool Connection_Occupied = false ;
    int Connection_Occupied_By = 0 ;
    int Connection_Occupied_By_Server = 0 ;
    int Connection_Occupied_By_Server_2 = 0 ;

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
                    cout << "Received a Client Message From Server " << client_port << " To Forward to Server \n" ;//<< response << "\n" ;

                    if( Connection_Occupied && client_port != Connection_Occupied_By_Server && client_port != Connection_Occupied_By_Server_2 )
                    {
                        Send_Con_Occupied_Msg(S, response, Connection_Occupied_By) ;
                        cout << "Link was Occupied by Client " << Connection_Occupied_By << ", Therefore, Denied That Request\n" ;
                    }

                    else
                    {
                        if(!Connection_Occupied)
                        {
                            Connection_Occupied = true ;
                            Connection_Occupied_By_Server = client_port ;

                            string temp = response ;
                            temp.erase(0,1) ;
                            stringstream ss(temp);
                            string sport ;
                            ss >> sport ;
                            Connection_Occupied_By = atoi(sport.c_str()) ;

                            Send_Client_Message(S, response, i, &R, Connection_Occupied_By_Server_2, true) ;

                            continue ;
                        }
                        
                        else if(Does_Exist(response, "closed") || Does_Exist(response, "Link is Occupied"))
                        {
                            Connection_Occupied = false;
                            Connection_Occupied_By = 0 ;
                            Connection_Occupied_By_Server = 0 ;
                            Connection_Occupied_By_Server_2 = 0 ;
                        }

                        Send_Client_Message(S, response, i, &R, Connection_Occupied_By_Server_2, false) ;
                    }
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

void Send_Con_Occupied_Msg(Server * S, string response, int OccupiedBy)
{
    //extracting sender port from message
    string temp = response;
    string sport, dport ;
    temp.erase(0,1);

    stringstream ss(temp) ;
    ss >> sport ;
    ss >> dport ;

    string msg = "2ProxyServer " + sport + " Cannot Establish Connection with Client " + dport + ", Link is Occupied by Client " + to_string(OccupiedBy) + "\n" ;

    S->Send(msg) ;
}

void Send_Client_Message(Server * S, string message, int Sender_Index, RoutingTable * R, int & Server2, bool Update)
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
    
    //getting server2 info
    if(Update)
        Server2 = Sdport ;

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