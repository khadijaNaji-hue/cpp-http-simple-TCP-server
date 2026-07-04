#include "methods.hpp"
#include "httpRequest.hpp"
#include "hashmaps.hpp"

#include <cstring>
#include <string>
#include <string.h>
#include <winsock2.h>
#include <mswsock.h>
#include <iostream>

#pragma comment(lib, "Mswsock.lib")
//محتاجين نهندل ايرور انو يطلب url مش موجود
// او يطلب حاجه برا الفايل سيرفر المخصص انو يتوصل ليه
void httpGET (httpReq msg, SOCKET  sock)
{
    std::cout << "get fun reached " << std::endl;
    if (msg.URL== "")
    {
        sendRespnse(403, sock);
        return;
    }

    std::cout << "url geted" << msg.URL << std::endl;

    HANDLE filehandle = CreateFile ((msg.URL).c_str(),
                                    GENERIC_READ,
                                    FILE_SHARE_READ,
                                    NULL,
                                    OPEN_EXISTING,
                                    FILE_FLAG_SEQUENTIAL_SCAN,
                                    NULL
                                   );
    //error handle open ?
    if (filehandle == INVALID_HANDLE_VALUE)
    {

        sendRespnse(GetLastError(),sock);
        return;
    }

    std::string headbuf = "HTTP/1.1 200 OK\r\nContent-Type: " + msg.content_type + "\r\n\r\n";



    TRANSMIT_FILE_BUFFERS buf ;
    buf.Head       = (PVOID) (headbuf.c_str());
    buf.HeadLength = headbuf.size();
    buf.Tail       = NULL;
    buf.TailLength = 0;
    bool err = TransmitFile (sock,
                             filehandle,
                             0,
                             0,
                             NULL,
                             &buf,
                             0
                            );
    if (err==0 )
    {
        sendRespnse(WSAGetLastError(), sock);
        return;
    }
    CloseHandle(filehandle);
}
void httpDELETE (httpReq msg, SOCKET  sock)
{
    std::cout << "DELETE fun reached " << std::endl;
    if (msg.URL== "")
    {
        sendRespnse(403, sock);
        return;
    }
    std::cout << "URL: "<<  msg.URL << std::endl;
    if (DeleteFile(msg.URL.c_str()))
    {
        sendRespnse(200,sock);
        std::cout << "done" << std::endl;
        return;
    }
    else
    {
        std::cout << "error" << std::endl;
        sendRespnse(GetLastError(),sock);
        return;
    }
}
void httpPUT (httpReq msg, SOCKET  sock)
{
    std::cout << "DELETE fun reached " << std::endl;
    if (msg.URL== "")
    {
        sendRespnse(403, sock);
        return;
    }
    std::cout << "URL: "<<  msg.URL << std::endl;
    HANDLE filehandle = CreateFile(msg.URL.c_str(),
                                   GENERIC_WRITE,
                                   0,
                                   NULL,
                                   CREATE_NEW, //send 409 Conflict back to client // THES IS DOUBLE CHECK WITH IF STATMENT
                                   FILE_ATTRIBUTE_NORMAL,
                                   NULL
                                  );
    if (filehandle == INVALID_HANDLE_VALUE)
    {
        sendRespnse(GetLastError(),sock);
        return;
    }

    DWORD totalwritten = 0;
    DWORD written;
    int totalrecved ;
    char* buf = new char[4096];
    memcpy(buf, msg.body.c_str(), msg.body.size());
    int buflen = msg.body.size();
    totalrecved = buflen;
    DWORD chunktotalwritten = 0;


    while (totalwritten < msg.contentLen )
    {
        bool err = WriteFile(
                       filehandle,
                       buf ,
                       buflen,
                       &written,
                       NULL
                   );
        if (!err)
        {
            delete[] buf;
            sendRespnse(500, sock);
            CloseHandle(filehandle);
            return;
        }
        totalwritten += written ;
        // هل كتبنا كل ال buflen?
        // كتبنا منه 30
        // كدا البافر الجديد عايزينه المره الجايه يبدا يكتب من اول 30
        // وقبل كدا يروح يستقبل 30 دا لو باقي 30 لسه مستقبلهمش
        memmove(buf, buf + written, buflen - written);

        buflen -= written ;

        if (totalrecved!=msg.contentLen)
        {
            int res= recv(sock, (buf+buflen)/*اخر حاجه فاضيه فيه*/,  4096-buflen,0 );
            buflen += res ;
            buf[buflen]='\0';
            totalrecved +=res ;
            if (res < 0 || ( res == 0 && totalrecved!=msg.contentLen) )
            {
                delete[] buf;
                sendRespnse(500, sock);
                CloseHandle(filehandle);
                return;
            }
        }
    }
    delete[] buf;
    sendRespnse(200, sock);
    CloseHandle(filehandle);
    return;

    /* while (totalwritten < msg.contentLen )
     {
         std::cout << "I AM IN 1ST WHILE"  << std::endl;
         while (chunktotalwritten < (DWORD)buflen )
         {
             std::cout << "I AM IN SECUND WHILE"  << std::endl;
             bool err = WriteFile(
                            filehandle,
                            buf + chunktotalwritten,
                            buflen- chunktotalwritten,
                            &written,
                            NULL
                        );
             if (!err)
             {
                 delete[] buf;
                 sendRespnse(500, sock);
                 CloseHandle(filehandle);
                 return;
             }
             chunktotalwritten+=written;
             totalwritten+= written ;
         }
         chunktotalwritten = 0;
         buflen = recv(sock,buf, 4095, 0);
         buf[buflen] = '\0';

         if (buflen < 0 || ( buflen == 0 && totalwritten!=msg.contentLen) )
         {
             delete[] buf;
             sendRespnse(500, sock);
             CloseHandle(filehandle);
             return;
         }

     }*/


    /*WriteFile(
        filehandle,
        msg.body.c_str(),
        msg.body.size(),   // we WANT to write this many bytes
        &written,      // WriteFile puts the ACTUAL written count here to check that all written
        NULL
    );

    CloseHandle(filehandle);
    if (written == msg.body.size())
        sendRespnse(200,sock);
    else
        sendRespnse(500,sock);
    return;
    */
}
void sendRespnse (int code, SOCKET sock)
{
    std::cout << "http error response: "  << std::endl;
    if (responsemap.count(code))
    {
        send(sock, responsemap  [code], strlen(responsemap [code]), 0);
        std::cout << responsemap  [code]  << std::endl;
        return ;
    }

    else
        send(sock, responsemap [500], strlen(responsemap [500]), 0);
    std::cout << responsemap  [500]  << std::endl;

    return ;
}
