#include "HandleWebResponse.h"
#include "AskProxyDlg.h"
#include "./AskMsg/XmlMarkup.h"
#include "./logclass/log.h"
#include "UntiTool.h"

CHandleWebResponse::CHandleWebResponse()
{
    m_hStopHandleWebResponse = false;
    m_pHandleWebResponseTread = 0;
    m_EnableWebServiceRequest = FALSE;
}

CHandleWebResponse::~CHandleWebResponse()
{

}

BOOL   CHandleWebResponse::StartHandleWebResponseThread(CAskProxyDlg* pDlg)//启动线程
{
    m_pDlg = pDlg;

    m_strReportIP = pDlg->m_strResportServer;
    m_nPort = atoi(pDlg->m_strResportPort.c_str());
    /*
       m_hStartHandleWebResponse = CreateEvent(NULL,FALSE,FALSE,"StartHandleWebResponse"+CUntiTool::GenUCID());

       if(!m_hStartHandleWebResponse)
       {
    	   m_pDlg->m_Log.Log(FLOW_LOG,"create start web response event fail");
    	   return FALSE;
       }
       else
       {
    	   m_pDlg->m_Log.Log(FLOW_LOG,"create start web response event succ");
       }

       m_hStopHandleWebResponse = CreateEvent(NULL,FALSE,FALSE,"StopHandleWebResponse"+CUntiTool::GenUCID());

       if(!m_hStopHandleWebResponse)
       {
    	   m_pDlg->m_Log.Log(FLOW_LOG,"create stop web response event fail");
    	   return FALSE;
       }
       else
       {
    	   m_pDlg->m_Log.Log(FLOW_LOG,"create stop web response event succ");
       }
    */

    ACE_Thread_Manager::instance()->spawn(CHandleWebResponse::HandleWebReponse,\
                                          this,\
                                          (THR_NEW_LWP|THR_JOINABLE|THR_INHERIT_SCHED|THR_SUSPENDED),
                                          &m_pHandleWebResponseTread);

    if(m_pHandleWebResponseTread == 0)
    {
        m_pDlg->m_Log.Log("FLOW_LOG-> start thead fail");
        return FALSE;
    }
    else
    {
        m_pDlg->m_Log.Log("FLOW_LOG-> start thread succ");
    }

    ACE_Thread_Manager::instance()->resume(m_pHandleWebResponseTread);

    return TRUE;
}

void* CHandleWebResponse::HandleWebReponse(void *lpvoid)
{
    CHandleWebResponse* pHandleWebResponse = reinterpret_cast<CHandleWebResponse*>(lpvoid);

    while(true)
    {
        pHandleWebResponse->m_hStartHandleWebResponse.acquire();
        if(pHandleWebResponse->m_hStopHandleWebResponse)
        {
            pHandleWebResponse->m_pDlg->m_Log.Log("FLOW_LOG-> CHandleWebResponse thread exit");
            return NULL;
        }
        else
        {
            pHandleWebResponse->Run();
        }
    }
    return NULL;
}

BOOL   CHandleWebResponse::StopHandleWebResponseThread()//停止线程
{
    m_hStopHandleWebResponse = true;
    m_hStartHandleWebResponse.release();
    ACE_Thread_Manager::instance()->join(m_pHandleWebResponseTread);

    return TRUE;
}

void CHandleWebResponse::Run()
{
    while(!m_pDlg->m_WSQueue.IsEmpty())
    {
        std::string strMsg = m_pDlg->m_WSQueue.Pop();

        if(strMsg != "")
        {
            m_pDlg->m_ReportLog.Log(strMsg);

/*            BYTE byBuffer[1024] = { 0 };

            int nLen = strMsg.GetLength() + 1;
            USES_CONVERSION;
            strcpy((LPSTR)byBuffer, T2CA(strMsg));
*/
            ssize_t nNum = m_pDlg->m_pSendSocket.WriteComm(strMsg, strMsg.length());
            if(nNum == -1)
            {
                m_pDlg->m_Log.Log("ERROR_LOG-> CHandleWebResponse: Run: SendTo: %s", strerror(errno));
            }
        }
    }
}
