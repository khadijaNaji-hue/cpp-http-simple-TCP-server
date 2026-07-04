#include "server.hpp"
#include "hashmaps.hpp"
#include"httpRequest.hpp"
#include <iostream>

server::server()
{
    serverFileRoot = "D:\\" ;
    serveraddr.sin_family = AF_INET ;
    serveraddr.sin_port = htons (8080);
    serveraddr.sin_addr.s_addr = INADDR_ANY;
}

server::server(int p, std::string Root )
{
    serverFileRoot = Root ;
    serveraddr.sin_family = AF_INET ;
    serveraddr.sin_port = htons (p);
    serveraddr.sin_addr.s_addr = INADDR_ANY;//can change it into data set by user like port
}


void server::start()
{
    WSADATA temp ;
    WSAStartup (MAKEWORD(2,2), &temp ); // WSAStartup and listensocket closed by deconstractor
    //error check WSAStartup

    listensocket = socket ( AF_INET, SOCK_STREAM, IPPROTO_TCP);
    //error check socket
    bind (listensocket,(struct sockaddr*)&serveraddr, sizeof(serveraddr));
    //error check bind
    listen (listensocket,SOMAXCONN);
    //error check listen
    std::cout << "server listen" << std::endl ;

    SOCKET clientsock ;
    while (1)
    {
        clientsock = accept (listensocket, 0,0);
        //error check accept
        int buflen = 4096;
        char* recvmsgbuf = new char[buflen];
        int recvlen = recv(clientsock, recvmsgbuf, buflen,0); //هل مفروض امسح دا كمان ؟
        recvmsgbuf[recvlen]= '\0' ;
        //error check recv


        char* temp= new char[8] ;
        char* lastindex =strchr(recvmsgbuf, ' ');
        strncpy(temp, recvmsgbuf,lastindex-recvmsgbuf );
        temp[lastindex-recvmsgbuf]= '\0';
        std::string method(temp);
        delete [] temp ;

        std::cout << "method: [" << method << "]" << std::endl;
        if (doMethod.count(method))
        {
            httpReq http (recvmsgbuf, recvlen, serverFileRoot) ;
            delete [] recvmsgbuf ;

            std::cout << "http msg done" << std::endl;
            doMethod[method](http, clientsock);
            std::cout << "do msg finished" << std::endl;

        }
        else
        {
            delete [] recvmsgbuf ;
            std::cout << "method skiped " << std::endl;
            sendRespnse(501, clientsock);
            std::cout << "this request method not exist" << std::endl ;
        }
        closesocket(clientsock);
        std::cout << "connection stopped" << std::endl;
    }
    closesocket(listensocket);
    WSACleanup();
    return ;
}
