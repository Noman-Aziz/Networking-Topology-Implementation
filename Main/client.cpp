#include "Header_Files/Client.h"
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>


using namespace std;

#define PORT 8080 

int main(){

    Client C;

    C.Set_Ip("");
    C.Set_Port(PORT);
    C.Set_Tcp();
    C.Create_Socket();
    C.Connect();


	bool ex = false;

	
	//for(;;){
		

		pid_t pid1 = fork();
		pid_t pid2;

		if(pid1 == 0){
			for(;;){
				string msg;
				getline(cin,msg);
				C.Send(msg);
				if(msg == "EXIT"){
					cout<<"ENDING"<<endl;
					 exit(0);
				}

			}
		
		}



		if(pid1 > 0){
			pid2 = fork();
			
			if(pid2 == 0){
				for(;;)
					cout<<C.Receive()<<endl;	
					if(ex){
						exit(0);
					}
			}
		}


		int p = (int)pid1;
		wait(&p);

		kill(pid2,SIGKILL);
		
		//wait(NULL);
		exit(0);
		





       
}