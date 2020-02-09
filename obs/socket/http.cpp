#include "http.h"
#include <stdio.h>
#include "../AskProxy.h"
#include "../AskProxyDlg.h"

bool CHttp::request(const std::string &URLRequest, const std::string &time, std::string &result)
{
    //URL中不能有空格，使用转义字符
    std::string msg = URLRequest;
    size_t index = msg.find(" ");
    while(index != std::string::npos)
    {
        msg.replace(index, 1, "%20");
        index = msg.find(" ");
    }

    int overtime = atoi(time.c_str());
    int second   = overtime / 1000;
    int msecond  = overtime % 1000;

//    std::cout << "second: " << second << " . msecond: " << msecond << std::endl;

    size_t ip_begin = msg.find("http://") + strlen("http://");
    if(ip_begin == std::string::npos)
        return false;

    size_t ip_end = msg.find(":", ip_begin);
    if(ip_end == std::string::npos)
        return false;

    std::string ip = msg.substr(ip_begin, ip_end-ip_begin);

    size_t port_begin = ip_end + 1;
    size_t port_end = msg.find("/", port_begin);
    if(port_end == std::string::npos)
        return false;

    int port = atoi(msg.substr(port_begin, port_end-port_begin).c_str());

    ACE_INET_Addr addr(port, ip.c_str());
    ACE_SOCK_Connector connector;
    ACE_SOCK_Stream stream;
    ACE_Time_Value value(second, msecond);

    std::string request("GET ");
//    request += msg.substr(port_end) + "\r\nHTTP/1.1\r\n\r\n";
    request += msg.substr(port_end) + "\r\nHTTP/1.1\r\nHost:" + ip  + "\r\n\r\n";

    if(connector.connect(stream, addr, &value)!=0)
    {
        GetApp()->m_Log.Log("can't connect error %s", strerror(errno));
        stream.close();
        return false;
    }

    if(stream.send_n(request.c_str(), request.length(), &value)<=0)
    {
        GetApp()->m_Log.Log("can't send error %s", strerror(errno));
        stream.close();
        return false;
    }

    char buffer[1024*10];
    memset(buffer, 0, 1024*10);
//    std::string re;
    int ret;
    result.clear();

    while((ret=stream.recv(buffer, 1024*10-1, &value))>0)
    {
        //re += buffer;
        result += buffer;
        memset(buffer, 0, 1024*10);
    }

    if(ret==-1 || errno==EWOULDBLOCK || errno==EAGAIN)
    {
        stream.close();
        return false;
    }

    stream.close();

    return !result.empty();
}
