#include "util_headers/Server.h"

using namespace std;

int main()
{
    Server S(10);

    S.Set_Tcp();
    S.Set_Port(6004);
    S.Create_Socket();
    S.Bind();
    S.Listen(3);

    int Temp_sd;
    string response, message;
    int max_clients = S.Get_Max_Clients() ;

    while(1)
    {
        S.Select();

        //checking if existing client sent message
        for(int i=0 ; i< max_clients ; i++)
        {
            Temp_sd = S.Get_Client_FD(i) ;

            if (S.Check_FD_Set(Temp_sd))
            {                 
                response = S.Receive(Temp_sd, i) ;

                if(response == "exit")
                    continue;

                cout << "Client IP(" << S.Get_Client_IP() << ") Port(" << S.Get_Client_Port() << ") : " << response << "\n" ;

                cout << "Server : " ;
                getline(cin,message) ;

                S.Send(message, Temp_sd);
            }
        }
    }

    return 0;
}