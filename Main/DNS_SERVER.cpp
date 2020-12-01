#include<iostream>
#include "DNS_Resolver.h"
#include "../Header_Files/Client.h"

using namespace std;



int main(){

    DNS_TABLE DT("DNS.csv");
    //DT.Display();
    string ipadd = "";
    int dnsport = 8080;

    Client * DNS_CLIENT = new Client;
    DNS_CLIENT->Set_Ip(ipadd);
    DNS_CLIENT->Set_Port(dnsport);
    DNS_CLIENT->Set_Tcp();
    DNS_CLIENT->Create_Socket();
    DNS_CLIENT->Connect();

    
    for(;;){
        string query = DNS_CLIENT->Receive();
        string res = DT.get_by_name(query);
        DNS_CLIENT->Send(res);

    }
        


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