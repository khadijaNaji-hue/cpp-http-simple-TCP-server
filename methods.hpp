#ifndef METHODS_HPP
#define METHODS_HPP

#include "httpRequest.hpp"
#include <winsock2.h>

void httpGET (httpReq msg , SOCKET  sock);
void httpDELETE (httpReq msg , SOCKET  sock);
void httpPUT (httpReq msg , SOCKET  sock);
void sendRespnse (int code , SOCKET sock);
#endif
