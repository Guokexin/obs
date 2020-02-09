#ifndef HANDLEWEBRESPONSE_H
#define HANDLEWEBRESPONSE_H

#include "logclass/log.h"
#include <string>
#include <ace/Thread_Semaphore.h>
#include <ace/Thread_Manager.h>
#include "owntype.h"

class CAskProxyDlg;

class CHandleWebResponse
{
public:
    CHandleWebResponse();
    virtual ~CHandleWebResponse();
public:
    //
    BOOL            StartHandleWebResponseThread(CAskProxyDlg* pDlg);//启动线程
    BOOL            StopHandleWebResponseThread();//停止线程
    void            Run();//运行


    static  void*     HandleWebReponse(void *lpvoid);

//   HANDLE           m_hStopHandleWebResponse;
//   HANDLE           m_hStartHandleWebResponse;
//   CWinThread*      m_pHandleWebResponseTread;
    volatile bool           m_hStopHandleWebResponse;
    ACE_Thread_Semaphore    m_hStartHandleWebResponse;
    ACE_thread_t            m_pHandleWebResponseTread;
    BOOL                    m_EnableWebServiceRequest;
    std::string             m_strWebServiceIPAddr;
    std::string             m_strWebServicePort;
    std::string             m_strWebServiceMethodName;
    CAskProxyDlg*           m_pDlg;

    std::string             m_strReportIP;
    int                     m_nPort;
};

#endif
