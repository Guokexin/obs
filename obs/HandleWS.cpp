#include "HandleWS.h"
#include "AskProxyDlg.h"
#include "AskMsg/QueueEvt.h"
#include "UntiTool.h"

CHandleWS::CHandleWS()
{
    m_hStopQuery = false;

	m_lpThread = 0;
}

CHandleWS::~CHandleWS()
{

}

BOOL CHandleWS::StartQueryThread(CAskProxyDlg* pDlg)
{
	m_pDlg = pDlg;

    ACE_Thread_Manager::instance()->spawn(CHandleWS::QueryProc,
                                          this,
                                          (THR_NEW_LWP|THR_JOINABLE|THR_INHERIT_SCHED|THR_SUSPENDED),
                                          &m_lpThread);

	ACE_Thread_Manager::instance()->resume(m_lpThread);

	return TRUE;
}

void* CHandleWS::QueryProc(LPVOID lpvoid)
{
    CHandleWS* pThis = reinterpret_cast<CHandleWS*>(lpvoid);

    int times = GetApp()->m_iGetQueueTime;

	while(true)
	{
		//pThis->m_hStopQuery
		sleep(times);
		//std::cout << "handle ws" << std::endl;
		if(pThis->m_hStopQuery)
		{
			pThis->m_pDlg->m_Log.Log("FLOW_LOG-> CHandleWS Exit thread ");
			return NULL;
		}
		else
		{
            pThis->QueryRun();
		}
	}
	return NULL;
}

BOOL CHandleWS::QueryRun()
{
   CQueueEvt  queue;
   std::string sendCmd = queue.CmdQueue();

   if(sendCmd.length()<0)
	   return FALSE;

   ssize_t ret = 0;

   ret = m_pDlg->m_SockManager.WriteComm(sendCmd, sendCmd.length());
   if(ret == 0)
	   return FALSE;
   return TRUE;
}
