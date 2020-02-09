#include <iconv.h>
#include "../AskProxyDlg.h"
#include "Userconn.h"

CUserConn::CUserConn()
    :m_listenfd(-1), m_epfd(-1)
{

}

bool CUserConn::setnonblocking(int sock)
{
    int opts;
    opts = ::fcntl(sock,F_GETFL);
    if(opts < 0)
    {
        std::cout << "fcntl sock F_GETFL error: " << strerror(errno) << std::endl;
        return false;
    }

    opts = opts|O_NONBLOCK;
    if(::fcntl(sock,F_SETFL,opts) < 0)
    {
        std::cout << "fcntl sock F_GETFL error: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}

bool CUserConn::init(int port)
{
    m_epfd = ::epoll_create(1024);
    if(m_epfd == -1)
    {
        std::cout << "epoll create error: " << strerror(errno) << std::endl;
        return false;
    }

    m_listenfd = ::socket(AF_INET, SOCK_STREAM, 0);

    int flag = 1;
    if(::setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == -1)
    {
        std::cout << "setsockopt error: " << strerror(errno) << std::endl;
        return false;
    }

    if(m_listenfd == -1)
    {
        std::cout << "create listenfd error: " << strerror(errno) << std::endl;
        return false;
    }

    m_socklen = sizeof(struct sockaddr_in);
    memset(&m_serveraddr, 0, m_socklen);
    m_serveraddr.sin_family = AF_INET;
    m_serveraddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
    m_serveraddr.sin_port = ::htons(port);

    if(::bind(m_listenfd, (sockaddr*)&m_serveraddr, m_socklen) == -1)
    {
        std::cout << "bind listenfd error: " << strerror(errno) << std::endl;
        return false;
    }

    if(::listen(m_listenfd, 20) == -1)
    {
        std::cout << "listen error: " << strerror(errno) << std::endl;
        return false;
    }

    if(::pthread_create(&m_pid, NULL, CUserConn::StartThread, this) != 0)
    {
        std::cout << "thread create error: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}

void* CUserConn::StartThread(void *arg)
{
    CUserConn *t = reinterpret_cast<CUserConn*>(arg);
    t->WorkThread();

    return NULL;
}

void CUserConn::WorkThread(void)
{
    m_ev.data.fd = m_listenfd;
    m_ev.events = EPOLLIN;

    int evs;

    ::epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_listenfd, &m_ev);

    while(true)
    {
        evs = ::epoll_wait(m_epfd, m_events, 1024, -1);
        for(int i=0; i<evs; ++i)
        {
            if(m_events[i].data.fd == m_listenfd && m_events[i].events & EPOLLIN)
            {
                new_conn();
            }
            else if(m_events[i].events & EPOLLIN)
            {
                read_message(m_events[i].data.fd);
            }
            else if(m_events[i].events & EPOLLOUT)
            {
                //需要维护异步写队列
            }
        }
    }
}

void CUserConn::new_conn()
{
    int connfd = ::accept(m_listenfd, (sockaddr*)&m_clientaddr, &m_socklen);
    if(connfd < 0)
    {
        std::cout << "accept error: " << strerror(errno) << std::endl;
        return ;
    }
    else
    {
	sockaddr_in sin;
	memcpy(&sin,&m_clientaddr,sizeof(sin));
	char ip[64];memset(ip,0,64);
	sprintf(ip,inet_ntoa(sin.sin_addr));
	int port = sin.sin_port;
	int sock = connfd;
	std::cout<<"New Client ip"<<ip<<" socket = "<<connfd<<std::endl;
	
    }
    
    setnonblocking(connfd);
    m_ev.data.fd = connfd;
    m_ev.events = EPOLLIN;

//    int sndbuf_len;
//    socklen_t snd_len = sizeof(int);
//    if( getsockopt(connfd, SOL_SOCKET, SO_SNDBUF, (void *)&sndbuf_len, &snd_len) < 0)
//    {
//        cout << "set opt buf error: " << strerror(errno) << endl;
//    }
//    cout << "sock recv sndbuf_len: " << sndbuf_len << ". snd_len: " << snd_len << endl;
//
////----------------------------------------------------------------------------------------------------
//
//    int rcvbuf_len = 32;
//    int len = sizeof(rcvbuf_len);
//    if( setsockopt( connfd, SOL_SOCKET, SO_SNDBUF, (void *)&rcvbuf_len, len ) == -1 )
//    {
//        cout << "set opt buf error: " << strerror(errno) << endl;
//    }
//
////----------------------------------------------------------------------------------------------------
//
//    if( getsockopt(connfd, SOL_SOCKET, SO_SNDBUF, (void *)&sndbuf_len, &snd_len) < 0)
//    {
//        cout << "set opt buf error: " << strerror(errno) << endl;
//    }
//    cout << "sock recv sndbuf_len: " << sndbuf_len << ". snd_len: " << snd_len << endl;

    ::epoll_ctl(m_epfd, EPOLL_CTL_ADD, connfd, &m_ev);
}

void CUserConn::read_message(int fd)
{
    //std::cout << "epoll recv fd: " << fd << std::endl;
    char buf[4096];
    ::memset(buf, 0, 4096);
    int n = ::read(fd, buf, 4096);
    if(n < 0)
    {
        if(errno == ECONNRESET)
        {
            //std::cout << "fd:" << " reset" << std::endl;
            closeConn(fd);
//            close(fd);
//            m_ev.data.fd = fd;
//            epoll_ctl(m_epfd, EPOLL_CTL_DEL, fd, &m_ev);
            return ;
        }
    }
    else if(n == 0)
    {
//        close(fd);
//        m_ev.data.fd = fd;
//        epoll_ctl(m_epfd, EPOLL_CTL_DEL, fd, &m_ev);
        closeConn(fd);
        return ;
    }
    else
    {
        process_message(fd, buf);
    }

}

bool CUserConn::write_message(int fd, const char *buffer, ssize_t len)
{
    int n = ::write(fd, buffer, len);
    if(n == len)
    {
        //std::cout << "send length = " << len << std::endl;
        return true;
    }
    else if(n == -1)
    {
        if(errno == EAGAIN || errno == EWOULDBLOCK)
        {
//            node *p = new node;
//            p->count = n;
//            p->fd = fd;
//            m_ev.events = EPOLLOUT;
//            m_ev.data.ptr = p;
//            epoll_ctl(m_epfd, EPOLL_CTL_MOD, fd, &m_ev);
//            std::cout << "again block: " << n << ".  error str: " << strerror(errno) << std::endl;
            return false;
        }
        else
        {
            //std::cout << "send = -1 error = " << errno << std::endl;
            closeConn(fd);
//            close(fd);
//            m_ev.data.fd = fd;
//            epoll_ctl(m_epfd, EPOLL_CTL_DEL, fd, &m_ev);
            return false;
        }
    }
    else if(n < len)
    {
        return false;
//        node *p = new node;
//        p->count = n;
//        p->fd = fd;
//        m_ev.events = EPOLLOUT;
//        m_ev.data.ptr = p;
//        epoll_ctl(m_epfd, EPOLL_CTL_MOD, fd, &m_ev);
//        cout << "again block: " << n << ".  error str: " << strerror(errno) << endl;
//        return false;
    }

    return true;
}

void CUserConn::process_message(int fd, const char *pbuf)
{
    GetApp()->m_AgentLog.Log("FLOW_LOG->new process message");

	std::string t(pbuf);
	if(t.length()<10)
		return ;
    AgentMsg* msg = new AgentMsg;
    msg->s = fd;
    msg->sMsg = pbuf;
	
	
    CAskProxyDlg *pDlg = GetApp();

    if(msg->sMsg.find("keep alive") == 0)
    {
        size_t nPos1=msg->sMsg.find("|");
        size_t nPos2=msg->sMsg.find("|",nPos1+1);

        std::string sStation;
        if(nPos1!=std::string::npos && nPos2!=std::string::npos)
        {
            sStation = msg->sMsg.substr(nPos1+1,nPos2-nPos1-1);
            CAgent *pAgent = pDlg->GetAgentFromStation(sStation);
            if(pAgent)
            {
                pAgent->lock();
                pAgent->sock = fd;
                pAgent->preLiveTime = ACE_Date_Time();
                pAgent->preLiveTimeSecond = time(NULL);
                pAgent->unlock();
                send_to_softphone(fd, "keep alive");
                pAgent->Release();
            }
            else
            {
                pDlg->m_keepLive.Log("ERROR_LOG->无效的KeepAlive,	没有发现指定的坐席. %s",sStation.c_str());
            }

            pDlg->m_keepLive.Log("FLOW_LOG->%s",msg->sMsg.c_str());
        }
        delete msg;
    }
    else
    {
        //将消息放入链表中。供其它线程处理!!
        pDlg->m_SoftPhoneMsgCritical.acquire();
        pDlg->m_AgentMsgList.push_back(msg);
        pDlg->m_SoftPhoneMsgCritical.release();
        pDlg->m_HandleSoftPhoneThread.m_hStartHandleSoftPhoneMsg.release();
    }
}

void CUserConn::send_to_softphone(int fd, const std::string &msg)
{
    char* utf = const_cast<char*>(msg.c_str());
    size_t inlen = msg.length();
    size_t len = 4 * msg.length();
    char *gbk = new char[len];
    memset(gbk, 0, len);

    char *in = utf;
    char *out = gbk;

    iconv_t cd = iconv_open("GBK","UTF-8");
    iconv(cd, &in, &inlen, &out, &len);
    iconv_close(cd);

    if(!write_message(fd, gbk, strlen(gbk)))
    {
        GetApp()->m_CheckAgent.m_CheckAgentLog.Log("ERROR_LOG->cti和座席 %d 连接异常", fd);
    }
    //::send(handle(), msg.c_str(), msg.length(), 0);
    //theApp.m_AgentLog.Log("FLOW_LOG->%s",msg.c_str());
//    if(send(handle(), gbk, strlen(gbk), 0) <= 0)
//    {
//        GetApp()->m_CheckAgent.m_CheckAgentLog.Log("ERROR_LOG->cti和座席socket连接异常");
//    }
    //theApp.m_AgentLog.Log("FLOW_LOG->%s",msg.c_str());
    if(msg == "keep alive")
        GetApp()->m_keepLive.Log("FLOW_LOG->keep alive");
    else
        GetApp()->m_AgentLog.Log("FLOW_LOG->send to softphone: %s",msg.c_str());

    delete [] gbk;

    return ;
}

void CUserConn::closeConn(int fd)
{
    ::close(fd);
    m_ev.data.fd = fd;
    ::epoll_ctl(m_epfd, EPOLL_CTL_DEL, fd, &m_ev);
}
