#ifndef HTTP_H
#define HTTP_H

#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Connector.h>
#include <ace/INET_Addr.h>
#include <ace/Time_Value.h>
#include <iostream>
#include <string>
#include <stdio.h>

class CHttp
{
public:
    bool static request(const std::string &URLRequest, const std::string &time, std::string &result);
};

#endif
