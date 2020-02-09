#ifndef CHECKAGENT_H
#define CHECKAGENT_H

#include <ace/Thread_Manager.h>
#include <ace/Thread_Semaphore.h>
#include "logclass/log.h"
#include "owntype.h"

class CAskProxyDlg;

class CCheckAgent
{
public:
	CCheckAgent();
	virtual ~CCheckAgent();

	CAskProxyDlg* m_pDlg;

	ACE_Thread_Semaphore  m_hStartCheckAgentEvent;
	volatile bool  m_hStopCheckAgentEvent;
	ACE_thread_t m_lpThread;
	CLog m_CheckAgentLog;

	void SetWnd(CAskProxyDlg* pDlg);

	//启动检测坐席的线程
	BOOL StartCheckAgentThread();
	//停止检测坐席的线程
	void StopCheckAgentThread();
	//检查坐席线程主函数
	static LPVOID CheckAgentFun(LPVOID lpvoid);
	//检查函数
	void run();
};

#endif
