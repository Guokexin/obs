#ifndef MSGQUEUE_H
#define MSGQUEUE_H

#include <string>
#include <list>
#include <ace/Thread_Mutex.h>
#include <ace/Date_Time.h>
#include "AskProxy.h"


struct WS_Q
{
	int  cmd;
	std::string sQueueMember;
	ACE_Date_Time dt;
};

typedef std::list<WS_Q*> CWSQueue;

class CMsgQueue
{
public:
	CMsgQueue();
	virtual ~CMsgQueue();
public:
//	pthread_mutex_t m_CriticalSection;
    ACE_Thread_Mutex m_CriticalSection;
	std::list<std::string> m_wsQueue;
	void       Push(const std::string &sMsg);
	std::string    Pop();
	BOOL       Clear();
	BOOL       IsEmpty();
};

#endif
