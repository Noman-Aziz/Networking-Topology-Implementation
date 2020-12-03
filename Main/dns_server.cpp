#include<iostream>
#include "../Header_Files/DNS_Resolver.h"
#include "../Header_Files/Client.h"

using namespace std;

int main(){

    DNS_TABLE DT("DNS.csv");
    //DT.Display();
    string ipadd = "127.0.10.4";
    int dnsport = 5004 ;

    Client * DNS_CLIENT = new Client;
    DNS_CLIENT->Set_Ip(ipadd);
    DNS_CLIENT->Set_Port(dnsport);
    DNS_CLIENT->Set_Tcp();
    DNS_CLIENT->Create_Socket();
    DNS_CLIENT->Connect();

    //Sending Active Message
    string first = "DNS SERVER" ;
    DNS_CLIENT->Send(first) ;
    sleep(1) ;
    cout << "DNS SERVER IS ACTIVE\n\n" ;

    bool first_msg = false ;

    for(;;){

        string msg = DNS_CLIENT->Receive();

        if(!first_msg)
        {
            first_msg = true ;
            cout << "Server 4 Connection Response : " << msg << "\n" ;
            continue ;
        }

        //seperate query from message
        string query , cport ;
        stringstream ss(msg.erase(0,1)) ;
        ss >> cport ;
        ss >> query ;

        cout << "DNS QUERY RECEIVED FOR SITE : " << query << "\n" ;

        string res = "3R" + cport + " " + query + " : " + DT.get_by_name(query) + " Replied by DNS Server" ;
        DNS_CLIENT->Send(res);    
    }

    delete DNS_CLIENT ;
}

    // string tkn = "aaaloo obssfsf csfwqfw afsfd";

    // string token ;
    // for(int i = 0 ; i < tkn.length() ; i++){
    //     if(tkn[i]==' '){
    //         cout<<token<<endl;
    //         token = "";
    //         continue;    
    //     }
    //     token = token + tkn[i];
    // }
    // cout<<token<<endl;