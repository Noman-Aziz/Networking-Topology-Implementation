#include<iostream>
#include<vector>
#include<fstream>
#include<sstream>
using namespace std;



struct DNS_Resolver
{
    string ip;
    string name;

    DNS_Resolver(string iname,string ip_addr){
        this->name = iname;
        this->ip = ip_addr;
    }

    void setDS(string iname,string ip_addr){
        this->name = iname;
        this->ip = ip_addr;
    }

};


struct DNS_TABLE{
    vector<DNS_Resolver> Table;
    
    DNS_TABLE(string filename){
        fstream fin; 
        fin.open(filename, ios::in);

        string line, word, temp; 
         
        while (fin >> temp) {
            string name,ip;
            
            int i ;
            for(i = 0 ; i < temp.length();i++){
                if(temp[i] == ','){
                    i+=1;
                    break;
                }
                name = name + temp[i];                        
            }
            for(i; i < temp.length();i++){
                ip = ip + temp[i];                        
            }

            DNS_Resolver DR(name,ip);
            Table.push_back(DR);

        } 
    }; 

    string get_by_name(string query){
        for(vector<DNS_Resolver>::iterator it = Table.begin() ; it != Table.end(); ++it){
            if(it->name == query){
                return it->ip; 
            }
        }

        return "NOT FOUND";
    };

    void Display(){
        for (vector<DNS_Resolver>::iterator it = Table.begin() ; it != Table.end(); ++it){
            cout<<(it->name)<<" "<<(it->ip)<<endl;
        }
    }

};




