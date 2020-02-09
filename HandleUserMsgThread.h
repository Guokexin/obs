#ifndef _HANDLE_USER_MSG_THREAD_H_
#define _HANDLE_USER_MSG_THREAD_H_

#include "StdString.h"
#include <list>
#include <pthread.h>
#include <sys/signal.h>
#include "Phone.h"
#include <map>
#include "ace/Thread_Mutex.h"
class CHandleUserMsgThread
{
public:
	static CHandleUserMsgThread* Instance();
private:
	CHandleUserMsgThread();

	static CHandleUserMsgThread m_singleton;
public:
	bool Initialize();
	~CHandleUserMsgThread();
public:
    static void* StartThread(void*);
    void  WorkThread();
    pthread_t m_pid;
    int m_nCount;
	
    pthread_mutex_t mutex ;
    std::list<CPhone*>  m_phoneList;	
    ACE_Thread_Mutex m_phoneMapCritical;
    std::map<CStdString,CPhone*>  m_phoneMap;
    static void* StartOBSThread(void*);
    void OBSMainThread();
    pthread_t m_obsPid;
	
   static void* StartReadDThread(void*);
   void  ReadDB();
   pthread_t m_readdb_pid;
  
};


class CHandleDBDataThread {

  public:
    static CHandleDBDataThread* Instance();
  private:
    CHandleDBDataThread();
    static CHandleUserMsgThread m_singleton;
  public:
    bool Initialize();
    ~CHandleDBDataThread();
  public:
    static void* StartThread(void*);
    void ReadDBThread();
    pthread_t m_read_db_pid;
    int m_nCount;
    pthread_mutex_t mutex;
    std::list<CPhone*> m_phoneList;

};

#endif
