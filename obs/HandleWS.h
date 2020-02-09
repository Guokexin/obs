#ifndef HANDLEWS_H
#define HANDLEWS_H

#include <ace/Thread_Manager.h>
#include "AskProxy.h"

class CAskProxyDlg;

class CHandleWS
{
public:
	CHandleWS();
	virtual ~CHandleWS();

	bool m_hStopQuery;

	ACE_thread_t m_lpThread;
	CAskProxyDlg* m_pDlg;

	BOOL StartQueryThread(CAskProxyDlg* pDlg);
	static void* QueryProc(void *lpvoid);
	BOOL StopQueryThread();
	BOOL QueryRun();
};

#endif
