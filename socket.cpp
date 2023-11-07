#include <sys/socket.h>
#include <netinet/in.h>
#include<stdio.h>
#include <stdlib.h>
#include<iostream>
#include<modbus/modbus.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
using namespace std;
char buffer[2000];
void error(const char *msg)
{
    perror(msg);
    exit(0);
}
void fill_all(int &sock_c){
    int id;
    float aip,aop,dip,dop;
    bool bip,bop;
    printf("\nenter index analog input ,analog output,digital input,digital output,binary input ,binary output respectively\n");
    cin>>id>>aip>>aop>>dip>>dop>>bip>>bop;
    char s[200];
    sprintf(s,"GET /fill_all?ind=%d&analog_input=%f&analog_output=%f&digital_input=%f&digital_output=%f&binary_input=%d&binary_output=%d HTTP/1.1\r\n",id,aip,aop,dip,dop,bip,bop);
    send(sock_c,s,sizeof(s)-2,0);
}
void read(int &sock_c){
    cout<<"enter b/n 1-6"<<endl;
    int a,id;
    cin>>a;
    char *s;
    char t[200];
    switch(a){
        case 1:
            s="analog_input";
            break;
        case 2:
            s="analog_output";
            break;
        case 3:
            s="digital_input";
            break;
        case 4:
            s="digital_output";
            break;
        case 5:
            s="binary_input";
            break;
        case 6:
            s="binary_output";
            break;
        default:
            cout<<"invalid";
            return;
    }
    cout<<"\nenter id : ";
    cin>>id;
    sprintf(t,"GET /read/%s?id=%d HTTP/1.1\r\nHost: 127.0.0.1\r\nConnection: keep-alive\r\n\r\n",s,id);
    int sen_t=send(sock_c,t,sizeof(t)-2,0); 
    if(sen_t<0)error("not sent");
    string responce="";
    while(int rec_v=recv(sock_c,buffer,2000,0)>0){
        for(auto i:buffer) responce+=i;
        bzero(&buffer,2000);
    }
    //cout<<"request sent\n";
    cout<<responce;
    return;
}
void edit(int &sock_c){
    cout<<"enter b/n 1-6"<<endl;
    int a,id;
    cin>>a;
    char *s;
    char t[200];
    switch(a){
        case 1:
            s="analog_input";
            break;
        case 2:
            s="analog_output";
            break;
        case 3:
            s="digital_input";
            break;
        case 4:
            s="digital_output";
            break;
        case 5:
            s="binary_input";
            break;
        case 6:
            s="binary_output";
            break;
        default:
            cout<<"invalid";
            return;
    }
    cout<<"\nenter id and val : ";
    cin>>id;
    if(a==5||a==6){
        bool val;
        cin>>val;
        sprintf(t,"GET /edit/%s?id=%d&val=%d HTTP/1.1\n",s,id,val);
    }
    else{
        float val;
        cin>>val;
        sprintf(t,"GET /edit/%s?id=%d&val=%f HTTP/1.1\n",s,id,val);
    }
    send(sock_c,t,sizeof(t)-2,0);
    return;
}
int main(int argc, char *argv[]){
    int sock_c=socket(AF_INET,SOCK_STREAM,0);
    if(sock_c<0) {
        error("socket not established");
        return -1;
    }
    struct sockaddr_in serv_adr;
    serv_adr.sin_family=AF_INET;
    serv_adr.sin_port=htons(std::stoi(argv[2]));
    int client_fd;
    std::cout<<argv[2]<<" "<<argv[1];
    if (inet_pton(AF_INET,argv[1] , &serv_adr.sin_addr)  <= 0) {  
               printf(  
                       "\nInvalid address/ Address not supported \n");  
               return -1;  
       }
    if (client_fd  
               = connect(sock_c, (struct sockaddr*)&serv_adr,  sizeof(serv_adr))  < 0) {  
               printf("\nConnection Failed \n");  
               return -1;  
       } 
    int a;
    //read(sock_c);
    cout<<"enter 1 to fill 2 to edit and 3 to read :\n";
    cin>>a;
    if(a==1)fill_all(sock_c);
    else if(a==2){
        edit(sock_c);
    }
    else {
        read(sock_c);
    }
    close(sock_c);
}