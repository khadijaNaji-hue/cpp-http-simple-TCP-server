#include "server.hpp"

int main()
{
    server s(8080 ,"D:\\0.6\\UDP SERVER PROJECT\\SERVER_FILES\\");
    s.start();
    return 0;
}


/*#include <iostream>          // cout
#include <winsock2.h>
#include <windows.h>         // core Windows API — gives us HANDLE, CreateFile, CloseHandle, TRANSMIT_FILE_BUFFERS...
#include <mswsock.h>         // Microsoft Winsock extension — gives us TransmitFile (high performance file sending)
#include <ws2tcpip.h>        // gives us inet_pton and other modern TCP/IP functions
#include <string.h>
#include <unordered_map>
#include <unordered_set>

using namespace std ;

// tells the linker to include these .lib files automatically
// without these the Winsock and TransmitFile functions won't link
#pragma comment(lib, "Ws2_32.lib")   // links the Winsock library
#pragma comment(lib, "Mswsock.lib")  // links the Microsoft Winsock extension (needed for TransmitFile)


string getPath (string &msg);
//string getbody (string &msg);
void httpGET (string recvmsg);
void httpDELETE (string recvmsg);// مضطرين نوحد الارجيومنت ف هنبعت المسج عامله ونطلع اللي عايزينه منها جوا كل فانكشن
void httpPUT (string recvmsg); // ممكن نعمل struct للrequest يبقى فيه ال method , body ,path ,headers socket
//struct Request {string method; string path;string body;SOCKET clientsock;};
const string root = "D:\\0.6\\UDP SERVER PROJECT\\SERVER_FILES\\" ;
unordered_set <string> files
{
    "cat.jpg","cup.png","hello.html", "click.html", "RateCardMuhmdAdel.html"
};
unordered_map<string, string> typemap =
{
    {"html", "text/html"}, {"HTML", "text/html"},
    {"png",  "image/png"}, {"PNG",  "image/png"},
    {"jpeg", "image/jpeg"},{"JPEG", "image/jpeg"},
    {"jpg",  "image/jpeg"},{"JPG",  "image/jpeg"}
};

SOCKET clientsock ;
int main ()
{
    WSADATA temp ;
    WSAStartup (MAKEWORD(2,2), &temp );

    unordered_map<string, void(*)(string)> doMethod =
    {
        {"GET",    httpGET},
        {"DELETE", httpDELETE},
        {"PUT",    httpPUT}
    };


    SOCKET listensocket = INVALID_SOCKET;
    listensocket = socket (AF_INET, SOCK_STREAM,IPPROTO_TCP );
    sockaddr_in  serveraddr = {0};
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(8080);
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    bind (listensocket, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    listen(listensocket, SOMAXCONN);
    cout<< "listening" << endl ;


    while (1)
    {
        clientsock = accept(listensocket,0,0);
        int buflen = 1000000;
        char recvmsgbuf[buflen];
        int recvlen =recv(clientsock, recvmsgbuf, buflen,0);
        string recvmsg (recvmsgbuf,recvlen);

        int index = recvmsg.find(' ') ;
        string method = recvmsg.substr(0, index );

        if (doMethod.count(method))
        {
            doMethod[method](recvmsg);
            cout<< method << endl ;
        }
        else// هنا محتاجه نرجع ايرور معين يدل على ان الميثود دي مش سابورتيد
        {
            const char* buf = "HTTP/1.1 405 Method Not Allowed\r\n\r\n";
            send(clientsock, buf, strlen(buf), 0);
            cout << "this request method not exist" << endl ;
        }


        closesocket(clientsock);

    }
    closesocket(listensocket);
    WSACleanup();
    return 0;
}
string getPath (string &msg)//make the path returned all in small letters
{
    cout << msg << endl;
    int start = msg.find(' ') + 2;
    int end = msg.find(' ', start );
    return msg.substr(start, end -start);

}
/*string getbody (string &msg)
{
    int start = msg.find('\r\n\r\n') + 4 ;
    return msg.substr(start);
}*/
/*
void httpGET (string recvmsg)//open file + TransmitFile to client handle-> createfile
{
    string path = getPath (recvmsg);
    cout << path << endl;
    if (files.count(path))// اعملي else يا هانم
    {
        string fullpath = root  +  path ;// ROOT + FILE NAME
        HANDLE filehandle = CreateFile (fullpath.c_str(),
                                        GENERIC_READ,
                                        FILE_SHARE_READ,
                                        NULL,
                                        OPEN_EXISTING,
                                        FILE_FLAG_SEQUENTIAL_SCAN,
                                        NULL
                                       );
        int index = path.find('.')+1 ;
        string type = path.substr( index,path.size() - index  );
        string headbuf =  "HTTP/1.1 200 OK\r\nContent-Type: " + (typemap[type]) + "\r\n\r\n";
        cout << headbuf << endl;
        TRANSMIT_FILE_BUFFERS buf;
        buf.Head      = (PVOID)( headbuf.c_str());
        buf.HeadLength=  headbuf.size();
        buf.Tail      = NULL;
        buf.TailLength=0;

        TransmitFile(
            clientsock,
            filehandle,
            0,
            0,
            NULL,
            &buf,
            0
        );

        CloseHandle(filehandle);
    }
    else
    {
        const char* buf = "HTTP/1.1 404 Not Found\r\n\r\n";
        send(clientsock, buf, strlen(buf), 0);
    }

}
void httpDELETE (string recvmsg)// DeleteFileA
{
    string path = getPath (recvmsg);//should clean the path because client could delet out of the root
    const char* buf;
    if (files.count(path))
    {
        string fullpath = root  +  path ;// ROOT + FILE NAME = D:\\CAT.PNG
        if (DeleteFile(fullpath.c_str()))  // تحقق من النجاح
        {
            files.erase(path);
            buf = "HTTP/1.1 200 OK\r\n\r\n";
            send(clientsock, buf, strlen(buf), 0);
        }
        else
        {
            buf = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
            send(clientsock, buf, strlen(buf), 0);
        }
    }
    else
    {
        buf = "HTTP/1.1 404 Not Found\r\n\r\n";
        send (clientsock,buf, strlen(buf),0 );
        cout << "file not here" << endl;
    }
}
void httpPUT (string recvmsg)//CreateFile + WriteFile
{
    string path = getPath (recvmsg);
    if (recvmsg.find("\r\n\r\n") == string::npos)
    {
        const char* buf = "HTTP/1.1 400 Bad Request\r\n\r\n";
        send(clientsock, buf, strlen(buf), 0);
        return;
    }
    int startbody  = recvmsg.find("\r\n\r\n") + 4 ;//should check if the body is exist (not bad req)
    string body = recvmsg.substr(startbody);
    DWORD  written ;
    if (!(files.count(path)))
    {
        string fullpath = root  +  path ;// ROOT + FILE NAME
        HANDLE filehandle = CreateFile (fullpath.c_str(),
                                        GENERIC_WRITE,
                                        0,
                                        NULL,
                                        CREATE_NEW, //send 409 Conflict back to client // THES IS DOUBLE CHECK WITH IF STATMENT
                                        FILE_ATTRIBUTE_NORMAL,
                                        NULL
                                       );
        if (filehandle == INVALID_HANDLE_VALUE)
        {
            const char* buf = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
            send(clientsock, buf, strlen(buf), 0);
            return;
        }
        WriteFile(
            filehandle,
            body.c_str(),
            body.size(),   // we WANT to write this many bytes
            &written,      // WriteFile puts the ACTUAL written count here to check that all written
            NULL
        ); // if written != body.size() <- fails

        CloseHandle(filehandle);
        files.insert(path);
        if (written == body.size())
        {
            const char* buf = "HTTP/1.1 200 OK\r\n\r\n";
            send(clientsock, buf, strlen(buf), 0);
        }
        else
        {
            const char* buf = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
            send(clientsock, buf, strlen(buf), 0);
        }
    }
    else   // اصلا موجود
    {
        const char* buf = "HTTP/1.1 409 Conflict\r\n\r\n";
        send(clientsock, buf, strlen(buf), 0);
    }
}*/
