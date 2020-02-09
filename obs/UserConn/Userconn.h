#ifndef USERCONN_H
#define USERCONN_H

#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <string>
#include <sys/signal.h>
#include <iostream>

class CUserConn
{
public:
    CUserConn();

public:
    bool init(int port);

public:
    void send_to_softphone(int fd, const std::string &msg);
    void process_message(int fd, const char *buffer);
    void closeConn(int fd);

private:
    void new_conn();
    void read_message(int fd);
    bool write_message(int fd, const char *buffer, ssize_t len);

private:
    bool setnonblocking(int sock);

private:
    static void* StartThread(void*);
    void  WorkThread();
    pthread_t m_pid;

private:
    int                 m_listenfd;
    int                 m_epfd;
    socklen_t           m_socklen;
    struct sockaddr_in  m_serveraddr;
    struct sockaddr_in  m_clientaddr;
    struct epoll_event  m_ev;
    struct epoll_event  m_events[1024];
};

#endif
