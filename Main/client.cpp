#include "../Header_Files/Client.h"
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>

using namespace std;

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

int main()
{

    Client *C = Setup_Client_Connection();

    bool ex = false;

    pid_t pid1 = fork();
    pid_t pid2;

    //child process sending messages
    if (pid1 == 0)
    {
        for (;;)
        {
            string msg;
            getline(cin, msg);
            C->Send(msg);
            if (msg == "exit")
            {
                exit(0);
            }
        }
    }

    //parent process receiving messages
    if (pid1 > 0)
    {
        pid2 = fork();

        if (pid2 == 0)
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

                if (ex)
                {
                    exit(0);
                }
            }
        }
    }

    //waiting for sending messages to end
    int p = (int)pid1;
    wait(&p);

    //killing all child processes
    kill(pid2, SIGKILL);

    //wait(NULL);
    exit(0);
}