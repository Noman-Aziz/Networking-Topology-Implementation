#include<iostream>
#include <string.h>


using namespace std;

int main(){
    string tkn = "aaaloo obssfsf csfwqfw afsfd";

    string token ;
    for(int i = 0 ; i < tkn.length() ; i++){
        if(tkn[i]==' '){
            cout<<token<<endl;
            token = "";
            continue;    
        }
        token = token + tkn[i];
    }
    cout<<token<<endl;

}