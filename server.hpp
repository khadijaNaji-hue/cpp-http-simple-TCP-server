#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <winsock2.h>
//ip set to (INADDR_ANY) which include any ip at the device : get it from ipconfig
//u should call start fun to activate your server
class server
{
private:
    SOCKET listensocket;
    sockaddr_in  serveraddr ;
    std::string serverFileRoot ;

public:
    server(); //set port to defult -> 8080 root to "D:\\"
    server(int port , std::string Root );
    void start();
};

#endif
