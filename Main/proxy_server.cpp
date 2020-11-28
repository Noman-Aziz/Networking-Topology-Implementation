#include "../Header_Files/Server.h"
#include "../Header_Files/RoutingTable.h"

using namespace std;

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

int main()
{
    Server * S = Setup_Server_Connection();

    int Temp_sd;
    string response, message;
    int max_clients = S->Get_Max_Clients() ;

    while(1)
    {
        S->Select();

        //checking if existing client sent message
        for(int i=0 ; i< max_clients ; i++)
        {
            Temp_sd = S->Get_Client_FD(i) ;

            if (S->Check_FD_Set(Temp_sd))
            {                 
                response = S->Receive(Temp_sd, i) ;

                //only one message and response with one client
                //if(response == "exit")
                //    continue;

                //cout << "Server : " ;
                //message = "OK";

                //S->Send(message, Temp_sd);

                continue;
            }
        }
    }

    //Deallocating Memory
    delete S ;

    return 0 ;
}