#include"basic_instruct_module/nettransfer.h"
#include <stdio.h>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include<iostream>
using namespace  std;
int main()
{
    return 0;
}
CNetTransfer::CNetTransfer()
{
}

CNetTransfer::~CNetTransfer()
{
}

SNetRecieve CNetTransfer::netTransfer(const char* ip,int port,char arrMsg[],int n)
{
    SNetRecieve x;
    int client = socket(AF_INET, SOCK_STREAM, 0);
    
    if (client == -1) 
    {
        std::cout << "Error: socket" << std::endl;
        x.length=-1;
        return x;
    }
    
    // connect
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip);
    if (connect(client, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) 
    {
        std::cout << "Error: connect" << std::endl;
        x.length=-1;
        return x;
     }
    // std::cout << "...connect" << std::endl;
    char* data=arrMsg;
    char buf[128];
   
    int l=sizeof(data);
    int ll=strlen(data);
        
    int re=send(client, data, n, 0);

    if(re<0)
    {
        x.length=-1;
        return x;
        
    }
    if (strcmp(data, "exit") == 0) 
    {
        std::cout << "...disconnect" << std::endl;
        x.length=-1;
        return x;
    }
    memset(buf, 0, sizeof(buf));
    int len = recv(client, buf, sizeof(buf), 0);
    buf[len] = '\0';
    
    x.length=len;
    x.content=buf;
    unsigned char num=x.content[len-1];
    x.realNum=num;
    close(client);
    return x;
}