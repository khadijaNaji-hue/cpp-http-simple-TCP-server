//httpRequest.cpp
#include "httpRequest.hpp"
#include "hashmaps.hpp"
#include <string>
#include <algorithm>
#include <cctype>


httpReq::httpReq () {}
httpReq::httpReq (char* rawmsg, int rawmsglen, std::string &Root )  //call parsehttp
{
    parshttp (rawmsg,rawmsglen, Root );
}
void httpReq::parshttp(char* rawmsg, int rawmsglen, std::string &Root)
{
    std::string msg(rawmsg,rawmsglen);
    //setmethod(msg);
    setURL(msg, Root);
    setheaders(msg);
    setContentLen();
    setcontent_type();
    setbody(msg);
    return;
}
/*void httpReq::setmethod (std::string &rawmsg)
{
    int index = rawmsg.find(' ') ;
    method = rawmsg.substr(0, index );
}*/
void httpReq::setURL (std::string &rawmsg, std::string &Root )
{
    int start = rawmsg.find(' ') + 2;
    int end = rawmsg.find(' ', start );
    URL = Root + rawmsg.substr(start, end -start);
    char resolvedURL[ MAX_PATH];
    GetFullPathNameA(URL.c_str(), MAX_PATH, resolvedURL, NULL);
    std::string resolvedStr = std::string(resolvedURL);
    if (resolvedStr.find(Root))
    {
        URL= "";
        return;
    }
    else
        URL = resolvedStr ;
}
void httpReq::setbody ( std::string &rawmsg)//لو جالك مسج بدايتها الميثود ساوي البودي ب الممحتوى اللي بعد العلامه غير كدا هتلزقي المسج اللي جايه ف نهايه المحتوى الموجود
{
    if (rawmsg.find("\r\n\r\n") != std::string::npos) // thats mean we are in first chunk
    {
        int startbody = rawmsg.find("\r\n\r\n") + 4;
        body = rawmsg.substr(startbody);
    }
    else
    {
        body = body + rawmsg ;
    }
    return ;
}
void httpReq::setcontent_type()
{
    if (headers.count("Content-Type"))
        content_type = headers["Content-Type"];  // من الـ headers
    else if (URL.find('.') != std::string::npos)   // من ال url دا لو موجود
        {
            std::string str =URL.substr(URL.find('.') + 1);
            std::transform(str.begin(), str.end(), str.begin(), ::tolower);
            content_type = typemap[str];
        }

    else
        content_type = "application/octet-stream";  // default لو مش عارف النوع معرفش ايه دا غشيتها قال انو لو اتبعت كدا البراوزر مش هيحاول يقرأه وهيعمله داونلود عل ى طول
}
void httpReq::setContentLen()
{
    if (headers.count("Content-Length"))
        contentLen = stoi(headers["Content-Length"]);  // string -> int
    else
        contentLen = body.size();
}
void httpReq::setpersistant ()
{
    if (headers.count("Connection"))
        persistant = (headers["Connection"] == "keep-alive");
    else
        persistant = false;
    return;

}
void httpReq::setheaders(std::string &rawmsg)
{
    int startline = rawmsg.find("\r\n") + 2;  // ابدأ بعد أول سطر
    int endheaders = rawmsg.find("\r\n\r\n");

    while (startline < endheaders)
    {
        int endtype  = rawmsg.find(':', startline);
        int endline  = rawmsg.find('\r', startline);

        std::string type  = rawmsg.substr(startline, endtype - startline);
        std::string value = rawmsg.substr(endtype + 2, endline - (endtype + 2));

        headers[type] = value;
        startline = endline + 2;
    }
}
