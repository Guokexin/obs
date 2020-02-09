#include "HandleAskPBX.h"
#include "AskProxyDlg.h"
#include "UntiTool.h"
#include "askproxydef.h"
#include "AgentOp.h"
#include "QueueStatic.h"
#include "./ResponseMsg.h"

CHandleAskPBX::CHandleAskPBX()
    :m_AskMsgRun(0, NULL, NULL, PBXTHREADNUM), m_DispatchRun(0, NULL, NULL, 10)
{
//    pthread_mutex_init(&m_AskMsgCritical, NULL);
//    sem_init(&m_AskMsgRun, 0, 0);
//    sem_init(&m_DispatchRun, 0, 0);
    m_strEvtPBX = "";

    m_AskMsgStop = false;
    m_HandAskPBXGrp = -1;

    m_DispatchAskPBXID = 0;
    m_DispatchStop = false;
}

CHandleAskPBX::~CHandleAskPBX()
{

}

BOOL CHandleAskPBX::StartHandleAskPBX(CAskProxyDlg* pDlg)
{
    m_pDlg = pDlg;
    /*m_hStartAskPBX = CreateEvent(NULL,FALSE,FALSE,"StartHandleAskPBX"+CUntiTool::GenUCID());
       if(m_hStartAskPBX)
       {
       m_pDlg->m_Log.Log(FLOW_LOG,"create start ask pbx succ");
       }
       else
       {
       m_pDlg->m_Log.Log(FLOW_LOG,"create start ask pbx fail");
       return FALSE;
       }

       m_hStopAskPBX = CreateEvent(NULL,FALSE,FALSE,"StopHandleAskPBX"+CUntiTool::GenUCID());
       if(m_hStopAskPBX)
       {
       m_pDlg->m_Log.Log(FLOW_LOG,"create stop ask pbx succ");
       }
       else
       {
       m_pDlg->m_Log.Log(FLOW_LOG,"create stop ask pbx fail");
       return FALSE;
       }

       m_hStartDispatchPBX = CreateEvent(NULL,FALSE,FALSE,"StartAstDispatchPBX"+CUntiTool::GenUCID());
       if(m_hStartDispatchPBX)
       {
       m_pDlg->m_Log.Log(FLOW_LOG,"create start pbxmsg dispatch succ");
       }
       else
       {
       m_pDlg->m_Log.Log(FLOW_LOG,"create start pbxmsg dispatch fail");
       return FALSE;
       }

       m_hStopDispatchPBX = CreateEvent(NULL,FALSE,FALSE,"StopAstDispatchPBX"+CUntiTool::GenUCID());
       if(m_hStopDispatchPBX)
       {
       m_pDlg->m_Log.Log(FLOW_LOG,"create stop pbxmsg dispatch succ");
       }
       else
       {
       m_pDlg->m_Log.Log(FLOW_LOG,"create stop pbxmsg dispatch fail");
       return FALSE;
       }


       m_lpHandleAskPBX = AfxBeginThread(CHandleAskPBX::HandleAskPBX,\
       this,\
       THREAD_PRIORITY_NORMAL,\
       0,\
       CREATE_SUSPENDED,\
       NULL);
       if(m_lpHandleAskPBX)
       {
       m_pDlg->m_Log.Log(FLOW_LOG,"create handle ask pbx thread succ");
       }
       else
       {
       m_pDlg->m_Log.Log(FLOW_LOG,"create handle ask pbx thread fail");
       return FALSE;
       }

       m_lpDispatchAskPBX = AfxBeginThread(CHandleAskPBX::DispatchAskPBX,\
       this,\
       THREAD_PRIORITY_NORMAL,\
       0,\
       CREATE_SUSPENDED,\
       NULL);
       if(m_lpDispatchAskPBX)
       {
       m_pDlg->m_Log.Log(FLOW_LOG,"create handle ask pbx thread succ");
       }
       else
       {
       m_pDlg->m_Log.Log(FLOW_LOG,"create handle ask pbx thread fail");
       return FALSE;
       }


       m_lpDispatchAskPBX->m_bAutoDelete = FALSE;
       m_lpHandleAskPBX->m_bAutoDelete = FALSE;
       m_lpHandleAskPBX->ResumeThread();
       m_lpDispatchAskPBX->ResumeThread();*/

    //for(int i=0; i<PBXTHREADNUM; ++i)
    m_HandAskPBXGrp = ACE_Thread_Manager::instance()->spawn_n(
                          PBXTHREADNUM,
                          CHandleAskPBX::HandleAskPBX,
                          this,
                          (THR_NEW_LWP|THR_JOINABLE|THR_INHERIT_SCHED|THR_SUSPENDED),
                          ACE_DEFAULT_THREAD_PRIORITY,
                          -1,
                          0,
                          m_HandAskPBXID
                      );
    if(m_HandAskPBXGrp == -1)
    {
        m_pDlg->m_Log.Log("create ask pbx thread fail");
        m_AskMsgStop = true;
        return FALSE;
    }
    else
    {
        m_pDlg->m_Log.Log("create ask pbx thread succ");
    }
    ACE_Thread_Manager::instance()->spawn(CHandleAskPBX::DispatchAskPBX,
                                          this,
                                          (THR_NEW_LWP|THR_JOINABLE|THR_INHERIT_SCHED|THR_SUSPENDED),
                                          &m_DispatchAskPBXID
                                         );
    if(m_DispatchAskPBXID==0)
    {
        m_pDlg->m_Log.Log("create handle ask pbx thread fail");
        return FALSE;
    }
    else
    {
        m_pDlg->m_Log.Log("create handle ask pbx thread succ");
    }

    ACE_Thread_Manager::instance()->resume_grp(m_HandAskPBXGrp);
    ACE_Thread_Manager::instance()->resume(m_DispatchAskPBXID);

    return TRUE;
}

BOOL CHandleAskPBX::StopHandleAskPBX()
{
    m_AskMsgStop = true;
//    sem_post(&m_AskMsgRun);
    m_AskMsgRun.release();
    ACE_Thread_Manager::instance()->wait_grp(m_HandAskPBXGrp);
    m_AskMsgStop = false;
    m_HandAskPBXGrp = -1;

    m_DispatchStop = true;
//    sem_post(&m_DispatchRun);
    m_DispatchRun.release();
    ACE_Thread_Manager::instance()->join(m_DispatchAskPBXID);
    m_DispatchStop = false;
    m_DispatchAskPBXID = 0;

    m_AskMsgList.clear();

    return TRUE;
}

void* CHandleAskPBX::HandleAskPBX(LPVOID lpvoid)
{
    CHandleAskPBX *pHandleAskPBX = reinterpret_cast<CHandleAskPBX*>(lpvoid);

    while(true)
    {
//        sem_wait(&(pHandleAskPBX->m_AskMsgRun));
        pHandleAskPBX->m_AskMsgRun.acquire();
        if(pHandleAskPBX->m_AskMsgStop)
            break;
        else
            pHandleAskPBX->RunHandleAskPBX();
    }
    return NULL;
}

void* CHandleAskPBX::DispatchAskPBX(LPVOID lpvoid)
{
    CHandleAskPBX *pHandleAskPBX = reinterpret_cast<CHandleAskPBX*>(lpvoid);

    while(true)
    {
//        sem_wait(&(pHandleAskPBX->m_DispatchRun));
        pHandleAskPBX->m_DispatchRun.acquire();
        if(pHandleAskPBX->m_DispatchStop)
            break;
        else
            pHandleAskPBX->RunDispatchAskPBX();
    }
    return NULL;
}

void  CHandleAskPBX::RunHandleAskPBX()
{
    while(!m_AskMsgList.empty())
    {
//        pthread_mutex_lock(&m_AskMsgCritical);
        m_AskMsgCritical.acquire();
        std::string strEvt = m_AskMsgList.front();
        m_AskMsgList.pop_front();
//        pthread_mutex_unlock(&m_AskMsgCritical);
        m_AskMsgCritical.release();
        usleep(10*1000);

        m_pDlg->m_Log.Log("接收到PBX的数据：%s",strEvt.c_str());
        CGeneralUtils  myGeneralUtils;

        char szResponse[64];
        memset(szResponse,0,64);
        myGeneralUtils.GetStringValue(strEvt,"Response",szResponse);

        char szEvent[64];
        memset(szEvent,0,64);
        myGeneralUtils.GetStringValue(strEvt,"Event",szEvent);
        //以上分别为Response 和 Event

        if(strcmp(szEvent,"") != 0)
        {
            char szEvent[128];
            memset(szEvent,0,128);
            myGeneralUtils.GetStringValue(strEvt,"Event",szEvent);

            if(strcmp(szEvent,"QueueMemberAdded") == 0)
            {
                //登录成功事件
                m_pDlg->m_Log.Log("Add QueueMember");
                //HandleLoginEvt(strEvt);
            }
            else if(strcmp(szEvent,"QueueMemberRemoved") == 0)
            {
                //登出成功事件
                m_pDlg->m_Log.Log("Remove QueueMember");
                //HandleLogoutEvt(strEvt);
            }
            else if(strcmp(szEvent,"QueueMemberPaused") == 0)
            {
                //空闲成功事件+示忙后处理成功事件
                HandleQueueMemberPausedEvt(strEvt);
            }
            else if(strcmp(szEvent,"OriginateResponse") == 0)
            {
                //呼出失败事件
                HandleOriginateResponseEvt(strEvt);
            }
            else if(strcmp(szEvent,"Dial") == 0)
            {
                //拨号成功事件
                HandleDialEvt(strEvt);
            }
            else if(strcmp(szEvent,"Bridge") == 0)
            {
                //通话建立事件
                HandleEstablishEvt(strEvt);
            }
            else if(strcmp(szEvent,"Newstate") == 0)
            {
                //震铃事件
                HandleNewstateEvt(strEvt);
            }
            else if(strcmp(szEvent,"QueueMemberStatus") == 0)
            {
                //座席状态事件
                HandQueueMemberStatusEvt(strEvt);
            }
            else if(strcmp(szEvent,"Hangup") == 0)
            {
                //挂机事件
                HandleHangupEvt(strEvt);
            }
            else if(strcmp(szEvent,"MeetmeJoin") == 0)
            {
                //进入会议事件
                HandleConfEvt(strEvt);
            }
            else if(strcmp(szEvent,"QueueParams") == 0)
            {
                //skillsnap事件，即技能组快照事件
                HandleQueueStaticEvt(strEvt);
            }
            else if(strcmp(szEvent,"UserEvent") == 0)
            {
                //非Astrisk自带的事件
                HandleUserEvent(strEvt);
            }
            else if(strcmp(szEvent,"Join") == 0)
            {
                //进入队列事件：呼入队列产生
                HandleJoinEvt(strEvt);
            }
            else if(strcmp(szEvent,"Leave") == 0)
            {
                //离开队列：分配到座席产生 skillRecord|
                HandleLeaveEvt(strEvt);
            }
        }
        else if(strcmp(szResponse,"") != 0)
        {
            //Response: ...
            char szActionID[128];
            memset(szActionID,0,128);
            myGeneralUtils.GetStringValue(strEvt,"ActionID",szActionID);

            if(strcmp(szActionID, "") != 0)
            {
                //每个动作会产生一个response事件
                HandleResponse(strEvt);
            }
        }
    }
}

void CHandleAskPBX::RunDispatchAskPBX()
{
    while(!m_pDlg->m_PBXMsgList.empty())
    {

//        std::string strEvt;
        std::string sTmp;
//        pthread_mutex_lock(&m_pDlg->m_PBXMsgCritical);
        m_pDlg->m_PBXMsgCritical.acquire();
        sTmp = m_pDlg->m_PBXMsgList.front();
        m_pDlg->m_PBXMsgList.pop_front();
        m_pDlg->m_PBXMsgCritical.release();
//        pthread_mutex_unlock(&m_pDlg->m_PBXMsgCritical);

        m_strEvtPBX += sTmp;

        size_t n = m_strEvtPBX.find("\r\n\r\n");
        size_t index;
        while(n != std::string::npos)
        {
            std::string strMsg = m_strEvtPBX.substr(0, n);

            index = strMsg.find(": ");
            while (index != std::string::npos)
            {
                strMsg.replace(index, strlen(": "), "=");
                index = strMsg.find(": ");
            }

            index = strMsg.find("\r\n");
            while (index != std::string::npos)
            {
                strMsg.replace(index, strlen("\r\n"), ";");
                index = strMsg.find("\r\n");
            }

//            pthread_mutex_lock(&m_AskMsgCritical);
            m_AskMsgCritical.acquire();
            m_AskMsgList.push_back(strMsg);
//            pthread_mutex_unlock(&m_AskMsgCritical);
            m_AskMsgCritical.release();
//            sem_post(&m_AskMsgRun);
            m_AskMsgRun.release();
            int nLen = n+4;
            m_strEvtPBX.erase(0, nLen);
            n=m_strEvtPBX.find("\r\n\r\n");
        }
    }
}

BOOL CHandleAskPBX::SetAgentStatusReportData(CAgent* pAgent)
{
    std::string sAgentStatus = m_pDlg->m_strAgentStatusWeb;
    if(pAgent)
    {
        std::string sCmd="agentStatus|";
        CUntiTool tool;
        std::string sCurrTime = tool.GetCurrTime();

        std::string sStatusCode;
        char format[128];
        memset(format, 0, 128);
        snprintf(format, 127, "%d", pAgent->AgentState);
        sStatusCode = format;
//		sStatusCode.Format("%d",pAgent->AgentState);
        std::string sStatusName = GetStatusNameFromCode(pAgent->AgentState);

        if(pAgent->AgentState == AS_Login)
        {
            REPLACE(sAgentStatus, "[agentId]", pAgent->sWorkNo);
            REPLACE(sAgentStatus, "[extension]", pAgent->sStation);
            REPLACE(sAgentStatus, "[skill]", pAgent->sQueueName);
            //sAgentStatus.Replace("[ringTime]","null");
            REPLACE(sAgentStatus, "[startTime]", sCurrTime);
            REPLACE(sAgentStatus, "[statusCode]", sStatusCode);
            REPLACE(sAgentStatus, "[statusName]", sStatusName);
            REPLACE(sAgentStatus, "[statusReason]", "nu");
            REPLACE(sAgentStatus, "[tenantId]", pAgent->sTeantid);
            std::string sData = "";
            sData += sCmd;
            sData += sAgentStatus;
            pAgent->m_strAgentStatusReport = sData;
        }
        else
        {

            CUntiTool tool;
            std::string sCurrTime = tool.GetCurrTime();
            REPLACE(pAgent->m_strAgentStatusReport, "[endTime]", sCurrTime);

            m_pDlg->m_WSQueue.Push(pAgent->m_strAgentStatusReport);
//			SetEvent(m_pDlg->m_HandleWS.m_hStartHandleWebResponse);
            m_pDlg->m_HandleWS.m_hStartHandleWebResponse.release();

            ///========================================================================

            pAgent->m_strAgentStatusReport = "";
            REPLACE(sAgentStatus, "[agentId]", pAgent->sWorkNo);
            REPLACE(sAgentStatus, "[extension]", pAgent->sStation);
            REPLACE(sAgentStatus, "[skill]", pAgent->sQueueName);
            REPLACE(sAgentStatus, "[startTime]", sCurrTime);
            REPLACE(sAgentStatus, "[statusCode]", sStatusCode);
            REPLACE(sAgentStatus, "[statusName]", sStatusName);
            //只有小休、挂机有原因，其他的没有原因置为nu
            m_pDlg->m_Log.Log("FLOW_LOG->小休的原因码%s",pAgent->sReasonCode.c_str());
            if(pAgent->AgentState == AS_Notready && pAgent->sReasonCode != "")
            {
                REPLACE(sAgentStatus, "[statusReason]", pAgent->sReasonCode);
            }
            else
            {
                REPLACE(sAgentStatus, "[statusReason]", "nu");
            }
            m_pDlg->m_Log.Log("FLOW_LOG->小休报表字段1:%s",sAgentStatus.c_str());
            //////////////////////////////////////////////////////////////////////////

            REPLACE(sAgentStatus, "[tenantId]", pAgent->sTeantid);

            //根据电信的情况要求追加的
            if(pAgent->AgentState == AS_Active)
            {
                std::string tmp = "&ani=" + pAgent->sAni + "&dnis=" + pAgent->sDnis + "&calldirect=" + pAgent->sCallDirect;
//				tmp.Format("&ani=%s&dnis=%s&calldirect=%s",,,);
                sAgentStatus += tmp;
            }

            std::string sMsg = "";
            sMsg += sCmd;
            sMsg += sAgentStatus;

            pAgent->m_strAgentStatusReport = sMsg;
        }

        std::string strAgentActualTimeStatus = m_pDlg->m_strAgentActualTimeStatus;
        REPLACE(strAgentActualTimeStatus, "[agentId]", pAgent->sWorkNo);
        REPLACE(strAgentActualTimeStatus, "[extension]", pAgent->sStation);
        REPLACE(strAgentActualTimeStatus, "[skill]", pAgent->sQueueName);
        REPLACE(strAgentActualTimeStatus, "[ringTime]", "nu");
        REPLACE(strAgentActualTimeStatus, "[startTime]", sCurrTime);
        REPLACE(strAgentActualTimeStatus, "[statusCode]", sStatusCode);
        REPLACE(strAgentActualTimeStatus, "[statusName]", sStatusName);
        //只有小休、挂机有原因，其他的没有原因置为nu
        m_pDlg->m_Log.Log("FLOW_LOG->小休的原因码%s",pAgent->sReasonCode.c_str());
        if(pAgent->AgentState == AS_Notready && pAgent->sReasonCode != "")
        {
            REPLACE(strAgentActualTimeStatus, "[statusReason]", pAgent->sReasonCode);
        }
        else
        {
            REPLACE(strAgentActualTimeStatus, "[statusReason]", "nu");
        }
        m_pDlg->m_Log.Log("FLOW_LOG->小休报表字段1:%s",sAgentStatus.c_str());

        if(pAgent->AgentState == AS_Active)
        {
            //通话的时候才有主被叫
            std::string tmp = "&ani=" + pAgent->sAni + "&dnis=" + pAgent->sDnis + "&calldirect=" + pAgent->sCallDirect;
//			tmp.Format("&ani=%s&dnis=%s&calldirect=%s",pAgent->sAni,pAgent->sDnis,pAgent->sCallDirect);
            strAgentActualTimeStatus += tmp;
        }

        REPLACE(strAgentActualTimeStatus, "[tenantId]", pAgent->sTeantid);

        std::string cmd = "agentActualTimeStatus|";
        strAgentActualTimeStatus = cmd + strAgentActualTimeStatus;
        m_pDlg->m_WSQueue.Push(strAgentActualTimeStatus);
    }
    return TRUE;
}

std::string CHandleAskPBX::GetStatusNameFromCode(int callStatus)
{
    if(callStatus == AS_Unknown)
    {
        return "AS_Unknown";
    }
    else if(callStatus == AS_Notlogin)
    {
        return "AS_Notlogin";
    }
    else if(callStatus == AS_Login)
    {
        return "AS_Login";
    }
    else if(callStatus == AS_Idle)
    {
        return "AS_Idle";
    }
    else if(callStatus == AS_Notready)
    {
        return  "AS_Notready";
    }
    else if(callStatus == AS_Alerting)
    {
        return "AS_Alerting";
    }
    else if(callStatus == AS_Dial)
    {
        return  "AS_Dial";
    }
    else if(callStatus == AS_DialAgent)
    {
        return "AS_DialAgent";
    }
    else if(callStatus == AS_Active)
    {
        return "AS_Active";

    }
    else if(callStatus == AS_Preview)
    {
        return "AS_Preview";
    }
    else if(callStatus == AS_Hold)
    {
        return "AS_Hold";
    }
    else if(callStatus == AS_Consulting)
    {
        return "AS_Consulting";
    }
    else if(callStatus == AS_Conferencing)
    {
        return "AS_Conferencing";
    }
    else if(callStatus == AS_Manage)
    {
        return "AS_Manage";
    }
    else if(callStatus == AS_Monitor)
    {
        return "AS_Monitor";
    }
    else if(callStatus == AS_HoldUp)
    {
        return "AS_HoldUp";
    }
    else if(callStatus == AS_Observe)
    {
        return "AS_Observe";
    }
    else if(callStatus == AS_Wrap)
    {
        return "AS_Wrap";
    }
    else if(callStatus == AS_Hangup)
    {
        return "AS_Hangup";
    }
    else
    {
        return "null";
    }
}

BOOL CHandleAskPBX::SetWSSkillSnap(CQueueEvt* pQueue, const std::string &sTenantID)
{
    std::string sCmd("skillSnap|");
    CUntiTool tool;
    std::string sCurrTime = tool.GetCurrTime();

    std::string sData = m_pDlg->m_strskillSnapWeb;

    REPLACE(sData, "[creatTime]", sCurrTime);
//	sData.Replace("[createTime]",sCurrTime);

    REPLACE(sData, "[tenantId]",sTenantID);
//	sData.Replace("[tenantId]",sTenantID);

    REPLACE(sData, "[skill]",pQueue->sQueue);
//	sData.Replace("[skill]",pQueue->sQueue);

    REPLACE(sData, "[waitingCalls]",pQueue->sWaitCalls);
//	sData.Replace("[waitingCalls]",pQueue->sWaitCalls);

    REPLACE(sData, "[agentActiveCount]",pQueue->sActiveCalls);
//	sData.Replace("[agentActiveCount]",pQueue->sActiveCalls);//add by ly

    REPLACE(sData, "[discardPhoneCount]",pQueue->sDiscardCalls);
//	sData.Replace("[discardPhoneCount]",pQueue->sDiscardCalls);

    REPLACE(sData, "[agentCallCount]",pQueue->sCompleteCalls);
//	sData.Replace("[agentCallCount]",pQueue->sCompleteCalls);

    REPLACE(sData, "[agentAvailableCount]",pQueue->sagentAvailableCount);
//	sData.Replace("[agentAvailableCount]",pQueue->sagentAvailableCount);

    REPLACE(sData, "[pausedMembers]",pQueue->spausedMembers);
//	sData.Replace("[pausedMembers]",pQueue->spausedMembers);

    std::string sMsg(sCmd+sData);
//	sMsg += sCmd;
//	sMsg += sData;

//	m_pDlg->m_skillSnapLog.Log(FLOW_LOG,"skillSnap push");//更改，因为有记录
    m_pDlg->m_WSQueue.Push(sMsg);
//	SetEvent(m_pDlg->m_HandleWS.m_hStartHandleWebResponse);
    m_pDlg->m_HandleWS.m_hStartHandleWebResponse.release();

    return 1;
}

BOOL CHandleAskPBX::HandleQueueMemberPausedEvt(const std::string &strEvt)
{
    CGeneralUtils myGeneralUtils;

    char szMemberName[64];
    memset(szMemberName,0,64);
    myGeneralUtils.GetStringValue(strEvt,"MemberName",szMemberName);

    CUntiTool tool;
    std::string strStation = tool.GetStationFromMemberName(szMemberName);

    BOOL bRet = FALSE;

    m_pDlg->SetLock();
    CAgent* pAgent = m_pDlg->GetAgentFromStation(strStation);


    if(!pAgent)
    {
        m_pDlg->m_Log.Log("FLOW_LOG->no find agent %s",strStation.c_str());
        bRet = FALSE;
    }
    else
    {
        std::string strAskMsg;
        if(pAgent->sAction == "SetIdle")
        {
            CIdleEvt msg;
            msg.m_strRet = "Succ";
            msg.m_strUserData = "station set idle succ";
            strAskMsg = msg.EnSoftPhoneMsg();
            pAgent->AgentState = AS_Idle;
            SetAgentStatusReportData(pAgent);

            CAgentOp op;
            op.SendResult(pAgent,strAskMsg);
            m_pDlg->m_Log.Log("FLOW_LOG->station=%s idle succ",pAgent->sStation.c_str());
        }
        else if(pAgent->sAction == "SetBusy")
        {
            CBusyEvt evt;
            evt.m_strRet = "Succ";
            evt.m_strUserData = "station set busy succ";
            strAskMsg = evt.EnSoftPhoneMsg();
            pAgent->AgentState = AS_Notready;
            SetAgentStatusReportData(pAgent);
            //		Sleep(0);
            CAgentOp op;
            op.SendResult(pAgent,strAskMsg);
            m_pDlg->m_Log.Log("FLOW_LOG->station=%s busy succ",pAgent->sStation.c_str());

        }
        else if(pAgent->sAction == "SetWrapup")
        {
            CWrapupEvt evt;
            evt.m_strRet = "Succ";
            evt.m_strUserData = "station set wrapup succ";
            strAskMsg = evt.EnSoftPhoneMsg();
            pAgent->AgentState = AS_Wrap;
            SetAgentStatusReportData(pAgent);
            usleep(10*1000);
            CAgentOp op;
            op.SendResult(pAgent,strAskMsg);
            m_pDlg->m_Log.Log("FLOW_LOG->station=%s wrapup succ",pAgent->sStation.c_str());
            usleep(10*1000);

        }
        bRet = TRUE;
    }

    m_pDlg->SetUnLock();

    return bRet;
}

BOOL CHandleAskPBX::SetWSCDRData(CAgent* pAgent)
{
    std::string sCmd("cdr|");

    CUntiTool tool;

    std::string sCurrTime = tool.GetCurrTime();

//	CString sData = m_pDlg->m_strCdrWeb;

    if(pAgent->AgentState == AS_Alerting)
    {
        //第一次接听的时间:振铃时间
//		int nPos = pAgent->m_strAgentCDRReport.Replace("[ringTime]",sCurrTime);
        size_t nPos = pAgent->m_strAgentCDRReport.find("[ringTime]");
        if(nPos != std::string::npos)
        {
            pAgent->m_strAgentCDRReport.replace(nPos, strlen("[ringTime]"), sCurrTime);
            pAgent->orginAni = pAgent->sAni;
            pAgent->orginDnis = pAgent->sDnis;
            pAgent->orginCallDirect = pAgent->sCallDirect;
            pAgent->originUCID = pAgent->sUCID;
            pAgent->m_callRingTime = sCurrTime;
        }
        // pAgent->m_strAgentCDRReport.Replace(_T("[startTime]"),"");//振铃时
    }
    else if(pAgent->AgentState == AS_Dial)
    {
        //第一次拨号成功的的时间:是指对方振铃的时间
//		int nPos = pAgent->m_strAgentCDRReport.Replace(_T("[ringTime]"),sCurrTime);
        size_t nPos = pAgent->m_strAgentCDRReport.find("[ringTime]");

        if(nPos != std::string::npos)
        {
            pAgent->m_strAgentCDRReport.replace(nPos, strlen("[ringTime]"), sCurrTime);
            pAgent->orginAni = pAgent->sAni;
            pAgent->orginDnis = pAgent->sDnis;
            pAgent->orginCallDirect = pAgent->sCallDirect;
            pAgent->originUCID = pAgent->sUCID;
            //////////////////////////////////////////////////////////////////////////
            //2013-01-21 add by chenlin
            pAgent->m_callRingTime = sCurrTime;
            //////////////////////////////////////////////////////////////////////////
        }
    }
    else if(pAgent->AgentState == AS_Monitor || pAgent->AgentState == AS_Observe)
    {
//		int nPos = pAgent->m_strAgentCDRReport.Replace(_T("[ringTime]"),sCurrTime);
//	    nPos = pAgent->m_strAgentCDRReport.Replace(_T("[startTime]"),sCurrTime);
        size_t nPos = pAgent->m_strAgentCDRReport.find("[ringTime]");
        if(nPos != std::string::npos)
        {
            pAgent->m_strAgentCDRReport.replace(nPos, strlen("[ringTime]"), sCurrTime);
        }

        nPos = pAgent->m_strAgentCDRReport.find("[startTime]");
        if(nPos != std::string::npos)
        {
            pAgent->m_strAgentCDRReport.replace(nPos, strlen("[startTime]"), sCurrTime);
        }

        pAgent->m_callRingTime = sCurrTime;
        pAgent->m_callStartTime = sCurrTime;

    }
    else if(pAgent->AgentState == AS_Active)
    {
        //第一次通话建立的时间:双方建立通话，注意下面如果没有建立通话，则将这个值置为""
//		int nPos = pAgent->m_strAgentCDRReport.Replace(_T("[startTime]"),sCurrTime);
        size_t nPos = pAgent->m_strAgentCDRReport.find("[startTime]");
        if(nPos != std::string::npos)
        {
            pAgent->m_strAgentCDRReport.replace(nPos, strlen("[startTime]"), sCurrTime);
            pAgent->m_callStartTime = sCurrTime;
            pAgent->bIntoEstablish = TRUE;
        }
    }
    else if(pAgent->AgentState == AS_Hangup)
    {
        //判断是否进入了通话,开始时间==结束时间
        if(!pAgent->bIntoEstablish)
        {
//			pAgent->m_strAgentCDRReport.Replace(_T("[startTime]"),sCurrTime);
            REPLACE(pAgent->m_strAgentCDRReport, "[startTime]", sCurrTime);
            pAgent->m_callStartTime = sCurrTime;
            //////////////////////////////////////////////////////////////////////////
        }

        //挂机的时间
        REPLACE(pAgent->m_strAgentCDRReport, "[ucid]", pAgent->originUCID);
        REPLACE(pAgent->m_strAgentCDRReport, "[agentId]", pAgent->sWorkNo);
        REPLACE(pAgent->m_strAgentCDRReport, "[extension]", pAgent->sStation);
        REPLACE(pAgent->m_strAgentCDRReport, "[skill]", pAgent->sQueueName);
        REPLACE(pAgent->m_strAgentCDRReport, "[callType]", pAgent->orginCallDirect);
        REPLACE(pAgent->m_strAgentCDRReport, "[caller]", pAgent->orginAni);
        REPLACE(pAgent->m_strAgentCDRReport, "[called]", pAgent->orginDnis);
        REPLACE(pAgent->m_strAgentCDRReport, "[endTime]", sCurrTime);
        REPLACE(pAgent->m_strAgentCDRReport, "[tenantId]", pAgent->sTeantid);
        REPLACE(pAgent->m_strAgentCDRReport, "[callReason]", "un");//
        REPLACE(pAgent->m_strAgentCDRReport, "[callEndReason]", "un");//,pAgent->sReasonCode);
        pAgent->m_callClearTime = sCurrTime;
        pAgent->savePopData();

        std::string sData = sCmd;
        sData += pAgent->m_strAgentCDRReport;

        m_pDlg->m_Log.Log("FLOW_LOG->cdr push");
        m_pDlg->m_WSQueue.Push(sData);

//	    SetEvent(m_pDlg->m_HandleWS.m_hStartHandleWebResponse);
        m_pDlg->m_HandleWS.m_hStartHandleWebResponse.release();

        pAgent->orginAni = "";
        pAgent->orginDnis = "";
        pAgent->bIntoEstablish = FALSE;

    }
    return TRUE;
}

BOOL CHandleAskPBX::HandleHangupEvt(const std::string &strEvt)
{
    CGeneralUtils myGeneralUtil;
    char szChannel[64];
    memset(szChannel,0,64);
    myGeneralUtil.GetStringValue(strEvt,"Channel",szChannel);

    m_pDlg->SetLock();
    CAgent* pAgent = m_pDlg->GetAgentFromChan(szChannel);
    if(pAgent)
    {
        m_pDlg->m_Log.Log("TEST_LOG->HandleHangupEvt:Ext=%s Chan=%s", pAgent->sStation.c_str(), szChannel);

        char szCause[63];
        memset(szCause,0,63);
        myGeneralUtil.GetStringValue(strEvt,"Cause",szCause);

        char szUserData[63];
        memset(szUserData,0,63);
        myGeneralUtil.GetStringValue(strEvt,"Cause-txt",szUserData);
        CHangupEvt evt;
        evt.m_strReason = szCause;
        evt.m_strUserData = szUserData;

        pAgent->AgentState = AS_Hangup;
        SetWSCDRData(pAgent);

        std::string strInfo = evt.EnSoftPhoneMsg();
        CAgentOp op;
        op.SendResult(pAgent,strInfo);

        pAgent->connChan[0].ResetCONN();
        pAgent->connChan[1].ResetCONN();

        pAgent->SetWrapup();
        pAgent->reset_phone();
        pAgent->AgentState = AS_Wrap;
        SetAgentStatusReportData(pAgent);
    }
    else
    {
        m_pDlg->m_Log.Log("FLOW_LOG->HandleHangupEvt  Chan=%s,没有发现通道对应的分机",szChannel);
    }
    //std::map<std::string, CAgent*>::iterator it = m_pDlg->m_MapChanToAgent.find(szChannel);
    auto it = m_pDlg->m_MapChanToAgent.find(szChannel);
    if(it != m_pDlg->m_MapChanToAgent.end())
    {
        CAgent *p = NULL;
        p = it->second;
        m_pDlg->m_MapChanToAgent.erase(it);
        if(p != NULL)
        {
            delete p;
        }
    }
//	m_pDlg->m_MapChanToAgent.RemoveKey(szChannel);  //移除map表key值
    m_pDlg->SetUnLock();

    return TRUE;
}

BOOL CHandleAskPBX::HandleOriginateResponseEvt(const std::string &strEvt)
{
    CGeneralUtils myGeneralUtil;

    char szAction[128];
    memset(szAction,0,128);
    myGeneralUtil.GetStringValue(strEvt,"ActionID",szAction);

    char szResponse[64];
    memset(szResponse,0,64);
    myGeneralUtil.GetStringValue(strEvt,"Response",szResponse);

    std::string sAction(szAction);

    //发现Station
    size_t n1 = sAction.find("|",0);
    size_t n2 = sAction.find("|",n1+1);
    if(n1 != std::string::npos || n2 != std::string::npos)
        return FALSE;
    std::string strStation = sAction.substr(n1+1,n2-n1-1);

    if(strStation != "")
    {
        CAgent* pAgent = NULL;
        m_pDlg->SetLock();
        pAgent = m_pDlg->GetAgentFromStation(strStation);
        if(pAgent != NULL)
        {
            if(pAgent->sAction == "MakeCall")
            {
                std::string strAskMsg;
                if(strcmp(szResponse,"Failure") == 0)
                {

//					char szReason[64];
// 										memset(szReason,0,64);
// 						                myGeneralUtil.GetStringValue(strEvt,"Reason",szReason);
// 					/*					CMakeCallEvt msg;
// 										msg.m_strCause = szReason;
// 										msg.m_strRet = "Fail";
// 										msg.m_strUserData = "make call fail";
// 										strAskMsg = msg.EnSoftPhoneMsg();
//
// 										pAgent->AgentState = AS_Dial;
// 										SetAgentStatusReportData(pAgent);
// 										SetWSCDRData(pAgent);
//
// 										CAgentOp op;
// 										op.SendResult(pAgent,strAskMsg);*/
                }
                else
                {
                    CMakeCallEvt msg;
                }
            }
        }
        else
        {
            m_pDlg->m_Log.Log("FLOW_LOG->HandleOriginateResponseEvt Not Find station",strStation.c_str());
        }
        m_pDlg->SetUnLock();
    }
    else
    {
        m_pDlg->m_Log.Log("FLOW_LOG->HandleOriginateResponseEvt station is null");
    }

    return TRUE;
}

BOOL CHandleAskPBX::HandleNewstateEvt(const std::string &strEvt)
{
    CGeneralUtils myGeneralUtil;

    char szAccessNumber[32];
    memset(szAccessNumber,0,32);
    myGeneralUtil.GetStringValue(strEvt,"AccessNumber",szAccessNumber);

    char szState[32];
    memset(szState,0,32);
    myGeneralUtil.GetStringValue(strEvt,"ChannelStateDesc",szState);
    //如果被叫是座席
    if(strcmp(szState,"Ringing") == 0)
    {
        char szCalled[32];
        memset(szCalled,0,32);
        myGeneralUtil.GetStringValue(strEvt,"Called",szCalled);
        m_pDlg->SetLock();
        CAgent* pAgent = m_pDlg->GetAgentFromStation(szCalled);
        if(pAgent)
        {

            char szChan[32];
            memset(szChan,0,32);
            myGeneralUtil.GetStringValue(strEvt,"Channel",szChan);

            char szCaller[32];
            memset(szCaller,0,32);
            myGeneralUtil.GetStringValue(strEvt,"Caller",szCaller);

            char szUCID[128];
            memset(szUCID,0,128);
            myGeneralUtil.GetStringValue(strEvt,"Ucid",szUCID);

            char szDirection[32];
            memset(szDirection,0,32);
            myGeneralUtil.GetStringValue(strEvt,"Direction",szDirection);

            pAgent->sAni = szCaller;
            pAgent->sDnis = szCalled;
            pAgent->AgentState = AS_Alerting;
            pAgent->sSelfChanID = szChan;
            pAgent->sCallDirect = szDirection;
            pAgent->sUCID = szUCID;

            //////////////////////////////////////////////////////////////////////////
            //2013-01-07 add by chenlin ↓
            //////////////////////////////////////////////////////////////////////////
            pAgent->m_callID = szChan; //Channel就是callID
            pAgent->m_accessNumber = szAccessNumber;
            pAgent->m_ivrTrack = szAccessNumber;//2012-11-30暂定ivr号和接入号一样
            //@时间已移至SetWSCDRData函数——2013-01-21
// 			//在popdata数据中增加振铃时间
// 			COleDateTime ti = COleDateTime::GetCurrentTime();
// 			pAgent->m_callRingTime = ti.Format("%Y-%m-%d %H:%M:%S");

            //////////////////////////////////////////////////////////////////////////
            //2013-01-07 add by chenlin ↑
            //////////////////////////////////////////////////////////////////////////


            m_pDlg->m_Log.Log("FLOW_LOG->振铃 存入map：chan %s--分机%s",szChan,pAgent->sStation.c_str());
            //m_pDlg->m_MapChanToAgent.SetAt(szChan,(CObject*&)pAgent);
            m_pDlg->m_MapChanToAgent[szChan] = pAgent;

            SetAgentStatusReportData(pAgent);
            SetWSCDRData(pAgent);
            CRingEvt evt;
            evt.m_strAni = szCaller;
            evt.m_strDnis = szCalled;
            evt.m_strDirect = szDirection;
            evt.m_strUCID = szUCID;
            evt.m_strIvrTrack = szAccessNumber;//弹屏数据增加IVR轨迹
            std::string sMsg = evt.EnSoftPhoneMsg();
            CAgentOp op;
            op.SendResult(pAgent,sMsg);


        }
        m_pDlg->SetUnLock();
    }

    return TRUE;
}

BOOL CHandleAskPBX::HandQueueMemberStatusEvt(const std::string &strEvt)
{
    CGeneralUtils myGeneralUtil;

    char szQueueName[64];
    memset(szQueueName,0,64);
    myGeneralUtil.GetStringValue(strEvt,"Queue",szQueueName);

    if(strcmp(szQueueName,"") != 0)
    {
        char szMemberName[64];
        memset(szMemberName,0,64);
        myGeneralUtil.GetStringValue(strEvt,"MemberName",szMemberName);
        CUntiTool tool;
        std::string strStation = tool.GetStationFromMemberName(szMemberName);
        if(strStation != "")
        {
            CAgent* pAgent = NULL;
            m_pDlg->SetLock();
            pAgent = m_pDlg->GetAgentFromStation(strStation);
            if(pAgent != NULL/* && pQueue!=NULL*/)
            {
                char szStatus[64];
                memset(szStatus,0,64);
                myGeneralUtil.GetStringValue(strEvt,"Status",szStatus);
                if(strcmp(szStatus,"3") == 0)
                {
                    pAgent->sAgentStatus = "3";
                }
                else
                {
                    if(pAgent->sAgentStatus == "3")
                    {
                        pAgent->sAgentStatus = "-1";
                    }
                }
            }
            m_pDlg->SetUnLock();
        }
    }
    return TRUE;
}

BOOL CHandleAskPBX::HandleDialEvt(const std::string &strEvt)
{
    BOOL bRet = FALSE;
    m_pDlg->m_Log.Log("TEST_LOG->HandDialEvt");
    CGeneralUtils myGeneralUtil;

    char szAccessNumber[32];
    memset(szAccessNumber,0,32);
    myGeneralUtil.GetStringValue(strEvt,"AccessNumber",szAccessNumber);

    char szChan[32];
    memset(szChan,0,32);
    myGeneralUtil.GetStringValue(strEvt,"Channel",szChan);

    char szCaller[32];
    memset(szCaller,0,32);
    myGeneralUtil.GetStringValue(strEvt,"Caller",szCaller);

    CUntiTool tool;
    m_pDlg->SetLock();
    CAgent* pAgent = m_pDlg->GetAgentFromStation(szCaller);
    if(pAgent)
    {
        char szCalled[32];
        memset(szCalled,0,32);
        myGeneralUtil.GetStringValue(strEvt,"Called",szCalled);

        char szUCID[128];
        memset(szUCID,0,128);
        myGeneralUtil.GetStringValue(strEvt,"Ucid",szUCID);

        char szDirection[32];
        memset(szDirection,0,32);
        myGeneralUtil.GetStringValue(strEvt,"Direction",szDirection);

        char szTenantId[128];
        memset(szTenantId,0,128);
        myGeneralUtil.GetStringValue(strEvt,"TenantId",szTenantId);

        char szCalledGatewayCaller[32];//新增协议：只有当被叫号码为PSTN号码时，CalledGatewayCaller字段才被设置
        memset(szCalledGatewayCaller,0,32);
        myGeneralUtil.GetStringValue(strEvt,"CalledGatewayCaller",szCalledGatewayCaller);

        pAgent->sDnis = szCalled;

        std::string sCalledGatewayCaller="";
        sCalledGatewayCaller=szCalledGatewayCaller;

        pAgent->sAni = szCaller;

        pAgent->sCallDirect = szDirection;
        pAgent->sUCID = szUCID;
        pAgent->sTeantid = szTenantId;
        pAgent->sSelfChanID = szChan;
        pAgent->AgentState = AS_Dial;

        pAgent->m_callID = szChan; //Channel就是callID
        pAgent->m_accessNumber = szAccessNumber;
        pAgent->m_ivrTrack = szAccessNumber;//2012-11-30暂定ivr号和接入号一样

        m_pDlg->m_Log.Log("TEST_LOG->HandDialEvt:Ext=%s Chan=%s",pAgent->sStation.c_str(),szChan);

        m_pDlg->m_MapChanToAgent[szChan] = pAgent;

        CMakeCallEvt evt;
        evt.m_strRet = "Succ";
        evt.m_strAni = szCaller;
        evt.m_strDnis = szCalled;
        evt.m_strUCID = szUCID;
        evt.m_strDirector = szDirection;

        CAgentOp op;
        op.SendResult(pAgent,evt.EnSoftPhoneMsg());

        SetWSCDRData(pAgent);
        SetAgentStatusReportData(pAgent);

        bRet = TRUE;
    }
    else
    {
        bRet = FALSE;
        m_pDlg->m_Log.Log("FLOW_LOG->HandDialEvt:Ext=%s,Chan=%s,没有发现通道对应的分机",szCaller,szChan);
    }
    m_pDlg->SetUnLock();

    return bRet;
}

BOOL CHandleAskPBX::HandleEstablishEvt(const std::string &strEvt)
{
    CGeneralUtils myGeneralUtil;


    char szCaller[32];
    memset(szCaller,0,32);
    myGeneralUtil.GetStringValue(strEvt,"Caller",szCaller);


    char szCalled[32];
    memset(szCalled,0,32);
    myGeneralUtil.GetStringValue(strEvt,"Called",szCalled);


    char szUCID[128];
    memset(szUCID,0,128);
    myGeneralUtil.GetStringValue(strEvt,"Ucid",szUCID);

    char szChan1[64];
    memset(szChan1,0,64);
    myGeneralUtil.GetStringValue(strEvt,"Channel1",szChan1);

    char szChan2[64];
    memset(szChan2,0,64);
    myGeneralUtil.GetStringValue(strEvt,"Channel2",szChan2);

    char szDirect[32];
    memset(szDirect,0,32);
    myGeneralUtil.GetStringValue(strEvt,"Direction",szDirect);

    m_pDlg->SetLock();
    CAgent* pCallerAgent = m_pDlg->GetAgentFromChan(szChan1);
    if(pCallerAgent)
    {
        CEstablishEvt evt;
        evt.m_strAni = szCaller;
        evt.m_strDnis = szCalled;
        evt.m_strUCID = szUCID;

        pCallerAgent->sOtherID = szChan2;
        pCallerAgent->AgentState=AS_Active;

        std::string sInfo = evt.EnAskMsg();

        CAgentOp op;
        op.SendResult(pCallerAgent,sInfo);

        SetAgentStatusReportData(pCallerAgent);
        SetWSCDRData(pCallerAgent);

    }
    else
    {
        m_pDlg->m_Log.Log("FLOW_LOG->%s 没有匹配的主叫",szChan1);
    }

    CAgent* pCalledAgent = m_pDlg->GetAgentFromChan(szChan2);
    if(pCalledAgent)
    {
        CEstablishEvt evt;
        evt.m_strAni = szCaller;
        evt.m_strDnis = szCalled;
        evt.m_strUCID = szUCID;
        pCalledAgent->sOtherID = szChan1;
        pCalledAgent->AgentState=AS_Active;//19

        std::string sInfo = evt.EnAskMsg();

        CAgentOp op;
        op.SendResult(pCalledAgent,sInfo);

        SetAgentStatusReportData(pCalledAgent);
        SetWSCDRData(pCalledAgent);
    }
    else
    {
        m_pDlg->m_Log.Log("FLOW_LOG->%s 没有匹配的被叫",szChan2);
    }

    m_pDlg->SetUnLock();

    return TRUE;
}

BOOL CHandleAskPBX::HandleConfEvt(const std::string &strEvt)
{
    BOOL bRet = FALSE;
    CConfEvt evt;
    evt.ParaResponseMsg(strEvt);

    evt.m_strRet = "Succ";
    evt.m_strUserData = "Conference";

    m_pDlg->SetLock();
    CAgent* pAgent = m_pDlg->GetAgentFromChan(evt.m_strChan);

    if(pAgent)
    {
        std::string strInfo = evt.EnSoftPhoneMsg();
        CAgentOp op;
        op.SendResult(pAgent,strInfo);
        pAgent->AgentState = AS_Conferencing;
        SetAgentStatusReportData(pAgent);
        pAgent->sHoldID = "";
        bRet = TRUE;
    }
    m_pDlg->SetUnLock();
    return bRet;
}

BOOL CHandleAskPBX::HandleLoginEvt(const std::string &strEvt)
{
    CLoginEvt evt;
    evt.ParaRetPBXEvt(strEvt);

    evt.m_strRet = "Succ";
    evt.m_strUserData = "login succ";

    m_pDlg->SetLock();
    CAgent* pAgent = m_pDlg->GetAgentFromStation(evt.m_strStation);
    if(pAgent)
    {
        std::string strInfo = evt.EnSoftPhoneMsg();
        CAgentOp op;
        BOOL bRet = op.SendResult(pAgent,strInfo);//忽略返回
        if(bRet)
        {
            pAgent->AgentState = AS_Login;
            SetAgentStatusReportData(pAgent);
            pAgent->bLogin = TRUE;
            pAgent->AgentState = AS_Notready;
            pAgent->sReasonCode = "0";
            SetAgentStatusReportData(pAgent);

            CBusyEvt evt;
            evt.m_strRet = "Succ";
            evt.m_strUserData  = "After Login set agent busy ";
            m_pDlg->SetWorkNoToMap(pAgent);

        }
    }
    m_pDlg->SetUnLock();
    return TRUE;
}

BOOL CHandleAskPBX::HandleLogoutEvt(const std::string &strEvt)
{
    BOOL bRet=FALSE;
    CLogoutEvt evt;
    evt.ParaRetPBXEvt(strEvt);

    evt.m_strRet = "Succ";
    evt.m_strUserData = "logout succ";

    m_pDlg->SetLock();
    CAgent* pAgent = m_pDlg->GetAgentFromStation(evt.m_strStation);
    if(pAgent)
    {
        std::string strInfo = evt.EnSoftPhoneMsg();
        if(strInfo != "")
        {
            CAgentOp op;
            bRet = FALSE;
            if(pAgent->sAction == "Logout")
            {
                m_pDlg->m_Log.Log("FLOW_LOG->坐席 %s Logout, Action=%s",pAgent->sStation.c_str(),pAgent->sAction.c_str());
                bRet = op.SendResult(pAgent,strInfo);
            }
            else
            {
                m_pDlg->m_Log.Log("FLOW_LOG->坐席 %s  Action=%s",pAgent->sStation.c_str(),pAgent->sAction.c_str());
            }

            pAgent->bLogin = FALSE;
            pAgent->AgentState = AS_Notlogin;
            SetAgentStatusReportData(pAgent);
            SetAgentStatusReportData(pAgent);

            m_pDlg->RemoveAgentFromMap(pAgent);

            bRet = TRUE;
        }
    }
    m_pDlg->SetUnLock();
    return bRet;
}

BOOL CHandleAskPBX::HandleQueueStaticEvt(const std::string &sEvt)
{
    CQueueEvt evt;
    evt.ParaQueueParams(sEvt);

    std::string sTenantID("0");//根据技能组获取该技能组所在的租户ID
    //根据evt从PBX获取的技能组号码，通过链表获取该技能组所对应的租户
    std::list<PBXInfo*>::iterator pos;
    /*	while (pos != m_pDlg->m_PBXInfoList.end())
    	{
    		m_pDlg->m_PBXInfoCritical.acquire();
    		PBXInfo* pPBXInfo1 = m_pDlg->m_PBXInfoList.GetNext(pos);
    	    m_pDlg->m_PBXInfoCritical.release();
    		if (pPBXInfo1->ssipSkillName == evt.sQueue)
    		{
    			sTenantID=pPBXInfo1->stenantId;
    			break;
    		}
    	}
    */
    PBXInfo *p = NULL;
    for(pos=m_pDlg->m_PBXInfoList.begin(); pos!=m_pDlg->m_PBXInfoList.end(); ++pos)
    {
        m_pDlg->m_PBXInfoCritical.acquire();
        p = *pos;
        m_pDlg->m_PBXInfoCritical.release();
        if(p->ssipSkillName == evt.sQueue)
        {
            sTenantID = p->stenantId;
            break;
        }
    }

    SetWSSkillSnap(&evt,sTenantID);
    return TRUE;
}


BOOL CHandleAskPBX::HandleJoinEvt(const std::string &sEvt)
{
    CGeneralUtils myGeneralUtil;

    char szChan[64];
    memset(szChan,0,64);
    myGeneralUtil.GetStringValue(sEvt,"Channel",szChan);

    char szCaller[64];
    memset(szCaller,0,64);
    myGeneralUtil.GetStringValue(sEvt,"Caller",szCaller);

    m_pDlg->SetLock();
    CAgent* pAgent = m_pDlg->GetAgentFromChan(szChan);
    if(pAgent)
    {
        if(pAgent->sAction == "Consult")
        {
            CEstablishEvt evt;
            evt.m_strAni = szCaller;
            evt.m_strDnis = "";
            evt.m_strUCID = "";

            pAgent->AgentState=AS_Active;

            std::string sInfo = evt.EnAskMsg();

            CAgentOp op;
            op.SendResult(pAgent,sInfo);
        }
    }
    m_pDlg->SetUnLock();

    CUntiTool tool;

    CQueueCallInfo* pQueueCallInfo = new CQueueCallInfo;
    pQueueCallInfo->sAni = szCaller;
    pQueueCallInfo->sJoinTime = tool.GetCurrTime();

    m_pDlg->m_MapQueueCallLock.acquire();
    m_pDlg->m_MapQueueCall[szChan] = pQueueCallInfo;
    m_pDlg->m_MapQueueCallLock.release();

    return TRUE;
}

BOOL CHandleAskPBX::HandleLeaveEvt(const std::string &sEvt)
{
    CGeneralUtils myGeneralUtil;

    char szChan[64];
    memset(szChan,0,64);
    myGeneralUtil.GetStringValue(sEvt,"Channel",szChan);

    //出队列事件Leave增加字段Reason表示排队结果（answer/noanswer）
    char szReason[64];
    memset(szReason,0,64);
    myGeneralUtil.GetStringValue(sEvt,"Reason",szReason);

    m_pDlg->m_MapQueueCallLock.acquire();
    CQueueCallInfo* pQueueCall = NULL;
//	BOOL bret=m_pDlg->m_MapQueueCall.Lookup(szChan,(CObject*&)pQueueCall);

    //std::map<std::string, CQueueCallInfo*>::iterator it = m_pDlg->m_MapQueueCall.find(szChan);
    auto it = m_pDlg->m_MapQueueCall.find(szChan);

    if(it!=m_pDlg->m_MapQueueCall.end())
    {
        pQueueCall = it->second;
        if(pQueueCall)
        {
            CUntiTool tool;
            std::string endTime = tool.GetCurrTime();
            pQueueCall->sLeaveTime = endTime;
            //设置技能组的详细信息
            std::string sCmd = "skillRecord|";
            std::string sData = m_pDlg->m_strskillRecordWeb;

            REPLACE(sData, "[tenantId]",pQueueCall->sTeantId);
//            sData.Replace("[tenantId]",pQueueCall->sTeantId);

            REPLACE(sData, "[skill]",pQueueCall->sQueueName);
//            sData.Replace("[skill]",pQueueCall->sQueueName);

            REPLACE(sData, "[callType]",pQueueCall->sCallType);
//            sData.Replace("[callType]",pQueueCall->sCallType);

            REPLACE(sData, "[caller]",pQueueCall->sAni);
//            sData.Replace("[caller]",pQueueCall->sAni);

            REPLACE(sData, "[inQueueTime]",pQueueCall->sJoinTime);
//            sData.Replace("[inQueueTime]",pQueueCall->sJoinTime);

            REPLACE(sData, "[outQueueTime]",tool.GetCurrTime());
//            sData.Replace("[outQueueTime]",tool.GetCurrTime());

            REPLACE(sData, "[queueResult]",szReason);
//            sData.Replace("[queueResult]",szReason);

            REPLACE(sData, "[ucid]",pQueueCall->sUcid);
//            sData.Replace("[ucid]",pQueueCall->sUcid);
            if(pQueueCall->saccessNumber != "")
            {
                REPLACE(sData, "[accessNumber]",pQueueCall->saccessNumber);
//                sData.Replace("[accessNumber]",pQueueCall->saccessNumber);
            }
            else
            {
                REPLACE(sData, "[accessNumber]","^");
//                sData.Replace("[accessNumber]","^");
            }
            m_pDlg->m_MapQueueCall.erase(it);
            delete pQueueCall;
            pQueueCall = NULL;

            std::string sMsg = "";
            sMsg += sCmd;
            sMsg += sData;


            m_pDlg->m_WSQueue.Push(sMsg);
//            SetEvent(m_pDlg->m_HandleWS.m_hStartHandleWebResponse);
            m_pDlg->m_HandleWS.m_hStartHandleWebResponse.release();
        }
    }
    m_pDlg->m_MapQueueCallLock.release();

    return TRUE;
}

BOOL CHandleAskPBX::HandleUserEvent(const std::string &sEvt)
{
    CGeneralUtils myGeneralUtil;

    char szUserEvent[32];
    memset(szUserEvent,0,32);
    myGeneralUtil.GetStringValue(sEvt,"UserEvent",szUserEvent);

    char szChan[128];
    memset(szChan,0,128);
    myGeneralUtil.GetStringValue(sEvt,"Channel",szChan);

    m_pDlg->m_Log.Log("FLOW_LOG->userEvent=%s",szUserEvent);

    if(strcmp(szUserEvent,"customQueueInfo") == 0)
    {
        //呼入队列时产生
        char szUCID[128];
        memset(szUCID,0,128);
        myGeneralUtil.GetStringValue(sEvt,"Ucid",szUCID);

        char szName[512];
        memset(szName,0,512);
        myGeneralUtil.GetStringValue(sEvt,"Name",szName);

        char szTeantId[128];
        memset(szTeantId,0,128);
        myGeneralUtil.GetStringValue(sEvt,"TenantId",szTeantId);

        char szaccessNumber[256];
        memset(szaccessNumber,0,sizeof(szaccessNumber));
        myGeneralUtil.GetStringValue(sEvt,"AccessNumber",szaccessNumber);

        CQueueCallInfo* pQueueCallInfo = NULL;
        m_pDlg->m_MapQueueCallLock.acquire();
//		BOOL bret=m_pDlg->m_MapQueueCall.Lookup(szChan,(CObject*&)pQueueCallInfo);
        //std::map<std::string, CQueueCallInfo*>::iterator it = m_pDlg->m_MapQueueCall.find(szChan);
        auto it = m_pDlg->m_MapQueueCall.find(szChan);
        if(it!=m_pDlg->m_MapQueueCall.end())
        {
            pQueueCallInfo = it->second;
            if(pQueueCallInfo)
            {
                char szCallInType[32];
                memset(szCallInType,0,32);
                myGeneralUtil.GetStringValue(sEvt,"Direction",szCallInType);

                CUntiTool tool;
                pQueueCallInfo->sUcid = szUCID;
                pQueueCallInfo->sQueueName = szName;
                pQueueCallInfo->sTeantId = szTeantId;
                pQueueCallInfo->sCallType = szCallInType;
                pQueueCallInfo->saccessNumber = szaccessNumber;


                m_pDlg->m_Log.Log("FLOW_LOG->customQueueInfo,Chan=%s,Ucid=%s,QueueName=%s,TeantID=%s,CallType=%s,accessNumber=%s",szChan,szUCID,szName,szTeantId,szCallInType,szaccessNumber);
            }
            else
            {
                m_pDlg->m_Log.Log("FLOW_LOG->customQueueInfo no find chan=%s QueueCallInfo.",szChan);
            }
        }
        m_pDlg->m_MapQueueCallLock.release();
    }
    //
    else if(strcmp(szUserEvent,"customRecordInfo") == 0)
    {
        //录音事件：拨号时产生(不需要通话)；座席通话后产生
        //Event=UserEvent;Privilege=user,all;UserEvent=customRecordInfo;Direction=out;Caller=21008;Reason=caller;TenantId=tenant2e629aafbfe042fb8296b1f677f96143;ActionID=linux-00000001;Ucid=192-168-20-91-2011-04-14-17-22-43-1302816162-484;Action=UserEvent;FileName=/var/spool/asterisk/monitor/tenant2e629aafbfe042fb8296b1f677f96143/2011/04/14/tenant2e629aafbfe042fb8296b1f677f96143-20110414172252-21008-53228.wav;Called=6111119
        //Event=UserEvent;Privilege=user,all;UserEvent=customRecordInfo;Direction=in;Caller=6111119;Reason=caller;TenantId=tenant2e629aafbfe042fb8296b1f677f96143;ActionID=linux-00000001;Ucid=192-168-20-91-2011-04-14-17-25-26-1302816326-486;Action=UserEvent;FileName=/var/spool/asterisk/monitor/tenant2e629aafbfe042fb8296b1f677f96143/2011/04/14/tenant2e629aafbfe042fb8296b1f677f96143-20110414172527-6111119-63577.wav;Called=21008

        char szFileName[512];
        memset(szFileName,0,512);
        myGeneralUtil.GetStringValue(sEvt,"FileName",szFileName);

        char szCaller[64];
        memset(szCaller,0,64);
        myGeneralUtil.GetStringValue(sEvt,"Caller",szCaller);

        char szCalled[64];
        memset(szCalled,0,64);
        myGeneralUtil.GetStringValue(sEvt,"Called",szCalled);

        char szCallDirection[64];
        memset(szCallDirection,0,64);
        myGeneralUtil.GetStringValue(sEvt,"Direction",szCallDirection);


        char szcallerAgentId[64];
        memset(szcallerAgentId,0,64);
        myGeneralUtil.GetStringValue(sEvt,"callerAgentId",szcallerAgentId);

        char szcalledAgentId[64];
        memset(szcalledAgentId,0,64);
        myGeneralUtil.GetStringValue(sEvt,"calledAgentId",szcalledAgentId);

        std::string strDirection;
        strDirection = szCallDirection;
        CAgent* pAgent = NULL;

        if (strDirection == "in")
        {
            m_pDlg->SetLock();
            pAgent = m_pDlg->GetAgentFromStation(szCalled);
            if(pAgent)
            {
                CRecordEvt evt;
                evt.sUCID = szFileName;//将录音文件名传给核心，原来是传ucid作为录音标识
                //存入录音recordID以备GetPopData获取
                pAgent->m_recordID = szFileName;
                std::string sMsg = evt.EnSoftPhoneMsg();
                m_pDlg->m_IOOpt.SendMsgToUser(pAgent->sock,sMsg);
            }
            m_pDlg->SetUnLock();

        }
        else if (strDirection == "out")
        {
            m_pDlg->SetLock();
            pAgent = m_pDlg->GetAgentFromStation(szCaller);
            if(pAgent)
            {
                CRecordEvt evt;
                evt.sUCID = szFileName;//将录音文件名传给核心，原来是传ucid作为录音标识。
                //存入录音recordID以备GetPopData获取
                pAgent->m_recordID = szFileName;
                std::string sMsg = evt.EnSoftPhoneMsg();
                m_pDlg->m_IOOpt.SendMsgToUser(pAgent->sock,sMsg);
            }
            m_pDlg->SetUnLock();
        }
        else if (strDirection == "internal")
        {
            m_pDlg->SetLock();
            CAgent *pAgent1 = m_pDlg->GetAgentFromStation(szCaller);
            CAgent *pAgent2 = m_pDlg->GetAgentFromStation(szCalled);
            if (pAgent1)
            {
                CRecordEvt evt;
                evt.sUCID = szFileName;//将录音文件名传给核心，原来是传ucid作为录音标识
                //存入录音recordID以备GetPopData获取
                pAgent1->m_recordID = szFileName;
                std::string sMsg = evt.EnSoftPhoneMsg();
                m_pDlg->m_IOOpt.SendMsgToUser(pAgent1->sock,sMsg);
            }
            if (pAgent2)
            {
                CRecordEvt evt;
                evt.sUCID = szFileName;//将录音文件名传给核心，原来是传ucid作为录音标识
                //存入录音recordID以备GetPopData获取
                pAgent2->m_recordID = szFileName;
                std::string sMsg = evt.EnSoftPhoneMsg();
                m_pDlg->m_IOOpt.SendMsgToUser(pAgent2->sock,sMsg);
            }
            m_pDlg->SetUnLock();
        }
    }
    else if(strcmp(szUserEvent,"customSpyInfo") == 0)
    {
        //通知外部程序监听：监听成功事件
        /*
        Event=UserEvent;
        Privilege=user,all;
        UserEvent=customSpyInfo;
        Channel=SIP/21009-0000007e;
        Caller=21009;
        Called=21008
        */
        //Event=UserEvent;
        //Privilege=user,all;
        //UserEvent=customSpyInfo;
        //Caller=56769000;
        //Called=s;
        //Channel=SIP/20.0.1.5-000001aa

        char szCaller[32];
        memset(szCaller,0,32);
        myGeneralUtil.GetStringValue(sEvt,"Caller",szCaller);


        char szCalled[32];
        memset(szCalled,0,32);
        myGeneralUtil.GetStringValue(sEvt,"Called",szCalled);


        char szChan[64];
        memset(szChan,0,64);
        myGeneralUtil.GetStringValue(sEvt,"Channel",szChan);

        char szUcid[128];
        memset(szUcid,0,128);
        myGeneralUtil.GetStringValue(sEvt,"Ucid",szUcid);

        char szTenantId[128];
        memset(szTenantId,0,128);
        myGeneralUtil.GetStringValue(sEvt,"TenantId",szTenantId);

        char szDirection[128];
        memset(szDirection,0,128);
        myGeneralUtil.GetStringValue(sEvt,"Direction",szDirection);


        //在SPY-info中绑定
        m_pDlg->SetLock();
        CAgent* pAgent = m_pDlg->GetAgentFromStation(szCaller);
        if(pAgent)
        {
            pAgent->sSelfChanID = szChan;

            if(pAgent->sAction == "Monitor")
            {
                CMonitorEvt evt;
                evt.m_strRet = "Succ";
                evt.m_strStation = szCaller;
                evt.m_strMonitorNo = szCalled;
                std::string sMsg = evt.EnSoftPhoneMsg();
                CAgentOp op;

                //

                pAgent->sSelfChanID = szChan;
                pAgent->orginAni = szCaller;
                pAgent->orginDnis = szCalled;
                pAgent->originUCID = szUcid;
                pAgent->orginCallDirect = szDirection;
                pAgent->AgentState = AS_Monitor;

                SetWSCDRData(pAgent);
                SetAgentStatusReportData(pAgent);

                m_pDlg->m_MapChanToAgent[szChan] = pAgent;

                op.SendResult(pAgent,sMsg);

            }
            else if(pAgent->sAction == "Observer")
            {
                CObserverEvt evt;
                evt.m_strRet = "Succ";
                evt.m_strStation = szCaller;
                evt.m_strObserverNo = szCalled;

                //cdr    信息
                pAgent->sSelfChanID = szChan;
                pAgent->orginAni = szCaller;
                pAgent->orginDnis = szCalled;
                pAgent->originUCID = szUcid;
                pAgent->orginCallDirect = szDirection;

                pAgent->AgentState  = AS_Observe;
                SetAgentStatusReportData(pAgent);


                m_pDlg->m_MapChanToAgent[szChan] = pAgent;


                std::string sMsg = evt.EnSoftPhoneMsg();
                CAgentOp op;
                op.SendResult(pAgent,sMsg);
            }
        }
        m_pDlg->SetUnLock();
    }
    else if(strcmp(szUserEvent,"customMusicOnHold") == 0)
    {
        //通过AMI命令显示进入保持状态时的事件：保持成功事件
        char szChan[64];
        memset(szChan,0,64);
        myGeneralUtil.GetStringValue(sEvt,"Channel",szChan);

        char szDisplay[128];
        memset(szDisplay,0,128);
        myGeneralUtil.GetStringValue(sEvt,"Display",szDisplay);

        CAgent* pAgent = m_pDlg->GetAgentFromChan(szChan);
        if(pAgent && strcmp(szDisplay,"1")==0)
        {
            CHoldEvt evt;
            evt.m_strRet = "Succ";
            evt.m_strUserData = "Hold Succ";
            std::string strInfo = evt.EnSoftPhoneMsg();
            CAgentOp op;
            op.SendResult(pAgent,strInfo);
            pAgent->AgentState = AS_Hold;
            pAgent->sHoldID = pAgent->sOtherID;
            SetAgentStatusReportData(pAgent);

        }
        else if(pAgent && strlen(szDisplay) > 3)
        {
            CUntiTool tool;
            std::string sAction;
            std::string sStation;
            std::string sVal;

            tool.GetAgentInfoFromHoldVal(szDisplay,sStation,sAction,sVal);
            m_pDlg->SetLock();
            CAgent* pAgent = m_pDlg->GetAgentFromStation(sStation);
            if(sVal == "0" && pAgent)
            {
                m_pDlg->m_Log.Log("ERROR_LOG->Station = %s , Action = %s",sStation.c_str(), sAction.c_str());

                if(sAction == "ConsultCancel")
                {
                    pAgent->ConsultCancel_1();
                }
                else if(sAction == "ConsultTrans")
                {
                    pAgent->ConsultTrans_1();
                }
                else if(sAction == "Conference")
                {
                    pAgent->Confercece_1();
                }
            }
            m_pDlg->SetUnLock();

        }
    }
    //////////////////////////////////////////////////////////////////////////
    //2013-01-29 add by chenlin ↓
    //新增拦截事件
    //////////////////////////////////////////////////////////////////////////
    else if (strcmp(szUserEvent,"customInterceptInfo") == 0)
    {
        char szCalled[32];
        memset(szCalled,0,32);
        myGeneralUtil.GetStringValue(sEvt,"Called",szCalled);



        char szCaller[32];
        memset(szCaller,0,32);
        myGeneralUtil.GetStringValue(sEvt,"Caller",szCaller);



        char szChan[64];
        memset(szChan,0,64);
        myGeneralUtil.GetStringValue(sEvt,"Channel",szChan);

        char szUcid[128];
        memset(szUcid,0,128);
        myGeneralUtil.GetStringValue(sEvt,"Ucid",szUcid);

        char szTenantId[128];
        memset(szTenantId,0,128);
        myGeneralUtil.GetStringValue(sEvt,"TenantId",szTenantId);

        char szDirection[128];
        memset(szDirection,0,128);
        myGeneralUtil.GetStringValue(sEvt,"Direction",szDirection);

        m_pDlg->SetLock();
        CAgent* pAgent = m_pDlg->GetAgentFromStation(szCaller);
        if(pAgent)
        {
            m_pDlg->m_Log.Log("TEST_LOG->CHAN=%s",szChan);
            pAgent->sAni = szCaller;
            pAgent->sDnis = szCalled;
            pAgent->AgentState = AS_HoldUp;
            pAgent->sSelfChanID = szChan;
            pAgent->sCallDirect = szDirection;
            pAgent->sUCID = szUcid;

            m_pDlg->m_MapChanToAgent[szChan] = pAgent;

            SetAgentStatusReportData(pAgent);
            SetWSCDRData(pAgent);
            CMakeCallEvt evt;
            evt.m_strRet = "Succ";
            evt.m_strAni = szCaller;
            evt.m_strDnis = szCalled;
            evt.m_strUCID = szUcid;
            evt.m_strDirector = szDirection;

            std::string sMsg = evt.EnSoftPhoneMsg();
            CAgentOp op;
            op.SendResult(pAgent,sMsg);

        }
        m_pDlg->SetUnLock();
    }
    return TRUE;
}

BOOL CHandleAskPBX::HandleUCIDEvt(const std::string &sEvt)
{
    CGeneralUtils myGeneralUtil;

    char szActionID[128];
    memset(szActionID,0,128);
    myGeneralUtil.GetStringValue(sEvt,"ActionID",szActionID);

    char szVal[128];
    memset(szVal,0,128);
    myGeneralUtil.GetStringValue(sEvt,"Value",szVal);

    std::string sValName;
    std::string sStation;
    CUntiTool tool;
    tool.GetStationFromAction(szActionID,sValName,sStation);

    if(sValName == "ucid")
    {
        //返回录音事件
        m_pDlg->SetLock();
        CAgent* pAgent = m_pDlg->GetAgentFromStation(sStation);
        if(pAgent)
        {
            CRecordEvt evt;
            evt.sStation = sStation;
            evt.sUCID = szVal;
            std::string sMsg = evt.EnSoftPhoneMsg();
            m_pDlg->m_IOOpt.SendMsgToUser(pAgent->sock,sMsg);
        }
        m_pDlg->SetUnLock();
    }
    return TRUE;
}

BOOL CHandleAskPBX::HandleResponse(const std::string &strEvt)
{
    CResponseMsg msg;
    std::string strBody = msg.ParaAskPBXResponse(strEvt);

//    std::cout << "HandleResponse: strBody" << strBody << std::endl;
    //
    if(strBody != "")
    {

        if(msg.m_strOperation == "Login")
        {
            //目前签入成功
            m_pDlg->SetLock();
            CAgent* pAgent =  m_pDlg->GetAgentFromStation(msg.m_strStation);
            if(pAgent)
            {
                if(msg.m_strRet == "Succ")
                {
                    //对于成功
                    m_pDlg->m_Log.Log("FLOW_LOG->Station %s %s skill %s Succ, Msg: %s",msg.m_strStation.c_str(), msg.m_strOperation.c_str(), msg.m_strQueue.c_str(), msg.m_strUserData.c_str());
                    //m_pDlg->m_IOOpt.SendMsgToUser(pAgent->sock,msg.m_strBody);
                    pAgent->setSkillState(msg.m_strQueue,"T");
                }
                else if(msg.m_strRet == "Fail")
                {
                    //

                    if(msg.m_strUserData == "Unable to add interface=Already there")
                    {
                        //认为登录是成功的，转义一下！
                        m_pDlg->m_Log.Log("FLOW_LOG->Station %s %s Success, Msg: %s",msg.m_strStation.c_str(), msg.m_strOperation.c_str(), msg.m_strUserData.c_str());
//						msg.m_strBody.Replace(">Fail<",">Succ<");
                        REPLACE(msg.m_strBody, ">Fail<",">Succ<");
                        //m_pDlg->m_IOOpt.SendMsgToUser(pAgent->sock,msg.m_strBody);
                        pAgent->setSkillState(msg.m_strQueue,"T");
                    }
                    else
                    {
                        pAgent->setSkillState(msg.m_strQueue,"F");
                        m_pDlg->m_Log.Log("FLOW_LOG->Station %s %s Fail, Msg: %s",msg.m_strStation.c_str(), msg.m_strOperation.c_str(), msg.m_strUserData.c_str());
                        m_pDlg->m_IOOpt.SendMsgToUser(pAgent->sock,msg.m_strBody);
                        //如果全部登陆失败了，但是此时需要清空坐席端
                    }
                }

                int nNum = pAgent->IsFullSills();

                if(nNum == 0)
                {
                    //还存在未签入技能组
                }
                else if(nNum == 1)
                {
                    //存在失败的
                    pAgent->LogOut();
                    //					closesocket(pAgent->sock);
                }
                else if(nNum == 2)
                {
                    //全部成功了
                    m_pDlg->m_IOOpt.SendMsgToUser(pAgent->sock,msg.m_strBody);
                    //执行HandleLoginEvt的动作

                    pAgent->AgentState = AS_Login;
                    SetAgentStatusReportData(pAgent);
                    pAgent->bLogin = TRUE;
                    pAgent->AgentState = AS_Notready;
                    pAgent->sReasonCode = "0";
                    SetAgentStatusReportData(pAgent);

                    CBusyEvt evt;
                    evt.m_strRet = "Succ";
                    evt.m_strUserData  = "After Login set agent busy ";
                    m_pDlg->SetWorkNoToMap(pAgent);
                }
                else
                {
                    //不可能发生
                }
            }
            m_pDlg->SetUnLock();
        }
        else if(msg.m_strOperation == "Logout")
        {
            //不管成功还是失败，都给坐席端回签出成功的消息，在Ast平台上，签出失败的原因只有一个，分机不在技能组中。

            m_pDlg->SetLock();
            CAgent* pAgent = NULL;
            pAgent = m_pDlg->GetAgentFromStation(msg.m_strStation);
            if(pAgent)
            {
                m_pDlg->m_Log.Log("FLOW_LOG->Station %s %s  from %s, Msg: %s",msg.m_strStation.c_str(), msg.m_strOperation.c_str(), msg.m_strQueue.c_str(), msg.m_strUserData.c_str());
                if(msg.m_strRet == "Succ")
                {
                    m_pDlg->m_IOOpt.SendMsgToUser(pAgent->sock,msg.m_strBody);
                }
                else if(msg.m_strRet == "Fail")
                {
                    if(msg.m_strUserData == "Unable to remove interface: Not there")
                    {
                        //已经不在队列了，说明签出成功了！！但是不会产生LogoutEvt事件了，此时资源无法清空！
                        //但是仍然告诉坐席签出成功了.
//						msg.m_strBody.Replace(">Fail<","Succ");
                        REPLACE(msg.m_strBody, ">Fail<","Succ");
                        m_pDlg->m_IOOpt.SendMsgToUser(pAgent->sock,msg.m_strBody);
                    }
                    else
                    {
                        //致命错误，肯定失败！！或者根据本程序不可能发生的错误，万一发生了，也要返回错误。
                        m_pDlg->m_IOOpt.SendMsgToUser(pAgent->sock,msg.m_strBody);
                    }
                }

                //
                pAgent->setSkillState(msg.m_strQueue,"C");
                int nNum = pAgent->IsOutSkills();
                //
                if(nNum == 2)
                {
                    pAgent->bLogin = FALSE;
                    pAgent->AgentState = AS_Notlogin;
                    SetAgentStatusReportData(pAgent);
                    SetAgentStatusReportData(pAgent);

                    m_pDlg->RemoveAgentFromMap(pAgent);

                    usleep(2000*1000);
                    //shutdown(pAgent->sock,SHUT_RDWR);
                    //close(pAgent->sock);
                    pAgent->sock->closeServer();

                    delete pAgent;
                    pAgent = NULL;
                }
            }
            m_pDlg->SetUnLock();
        }
        else
        {
            //其余操作的处理！！！
            m_pDlg->SetLock();
            CAgent* pAgent =  m_pDlg->GetAgentFromStation(msg.m_strStation);
            if(pAgent)
            {

                m_pDlg->m_Log.Log("FLOW_LOG->Station %s %s Succ, Msg: %s",msg.m_strStation.c_str(), msg.m_strOperation.c_str(), msg.m_strUserData.c_str());
                m_pDlg->m_IOOpt.SendMsgToUser(pAgent->sock,msg.m_strBody);
            }
            m_pDlg->SetUnLock();
        }
    }
    //最终返回给坐席的数据
    m_pDlg->m_Log.Log("TEST_LOG->response给坐席的数据：%s",msg.m_strBody.c_str());
    return TRUE;
}






