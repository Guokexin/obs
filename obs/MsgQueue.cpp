#include "MsgQueue.h"

CMsgQueue::CMsgQueue()
{
//    pthread_mutex_init(&m_CriticalSection, NULL);
}

CMsgQueue::~CMsgQueue()
{

}

void  CMsgQueue::Push(const std::string &sMsg)
{
//	pthread_mutex_lock(&m_CriticalSection);
    m_CriticalSection.acquire();
	m_wsQueue.push_back(sMsg);
//	pthread_mutex_unlock(&m_CriticalSection);
    m_CriticalSection.release();
}
std::string   CMsgQueue::Pop()
{
	std::string sMsg;

//	pthread_mutex_lock(&m_CriticalSection);
    m_CriticalSection.acquire();
	sMsg = *(m_wsQueue.begin());
	m_wsQueue.pop_front();
//	pthread_mutex_unlock(&m_CriticalSection);
    m_CriticalSection.release();
	return sMsg;
}

BOOL CMsgQueue::Clear()
{
//	pthread_mutex_lock(&m_CriticalSection);
    m_CriticalSection.acquire();
	m_wsQueue.clear();
//	pthread_mutex_unlock(&m_CriticalSection);
    m_CriticalSection.release();
	return TRUE;
}

BOOL CMsgQueue::IsEmpty()
{
	return m_wsQueue.empty();
}
