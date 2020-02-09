#ifndef COMPLETEPORT_H
#define COMPLETEPORT_H

class CAskProxyDlg;

#include "logclass/log.h"
#include "UserConn/Userconn.h"
//#include "UserConn/MulAcceptor.h"
//#include "UserConn/MyTask.h"
#include <ace/Thread_Manager.h>

class CCompletePort
{
public:
	CCompletePort();
	~CCompletePort();

public:
    bool Init(CAskProxyDlg *dlg);
    bool SendMsgToUser(int s, const std::string &msg);
//    bool startThread();

private:
    CAskProxyDlg* m_pDlg;
//    ACE_thread_t m_eventThreadID;
//    volatile bool m_stop;

private:
//    static void* event_thread(void *p);

public:
//private:
//    MulAcceptor m_acceptor;
//	MyTask m_myTask;
//	ACE_INET_Addr m_addrAccept;
//	ACE_Proactor *m_Proactor;
    CUserConn m_conn;
};

#endif
