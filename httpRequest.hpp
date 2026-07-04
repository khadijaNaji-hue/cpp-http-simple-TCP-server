#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <unordered_map>
#include <string>
class httpReq
{
private :

    void setURL (std::string &rawmsg , std::string &Root);// بترجع ال URL بس في حاليه انو جوا ال ROOT بتاع السيرفر غير كدا هترجع ""
    //void setmethod (std::string &rawmsg );
    void setbody ( std::string &rawmsg);
    void setheaders (std::string &rawmsg );//WE NEED THIS ? WHY ?
    void setcontent_type ();//from URL
    void setContentLen ();
    void setpersistant ();

public :
    std::string URL ; // include the root "D:\\0.6\\UDP SERVER PROJECT\\SERVER_FILES\\" (RESOLVED PATH)
    //std::string method ;
    std::string content_type ;
    std::string body ;
    std::unordered_map<std::string, std::string> headers ;
    int contentLen ;
    bool persistant ;

    httpReq ();
    httpReq (char* rawmsg , int rawmsglen, std::string &Root);//call parsehttp
    void parshttp(char* rawmsg , int rawmsglen, std::string &Root);

};

#endif
