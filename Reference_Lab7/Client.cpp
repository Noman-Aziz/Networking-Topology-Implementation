#include "util_headers/Client.h"

using namespace std;

int main()
{
    Client C;

    C.Set_Tcp();
    C.Set_Port(6004);
    C.Create_Socket();
    C.Connect();

    string message = "", response ;

    //Initial Server Connection Response
    cout << C.Receive() << endl ;

    cout << "\nStarting Conversation with Server : Enter exit to End Connection\n\n";

    while(1)
    {
        cout << "Client : " ;
        getline(cin,message);
        C.Send(message);

        if (message == "exit")
            break;

        response = C.Receive();

        cout << "Server : " << response << endl ;
    }

    return 0;
}