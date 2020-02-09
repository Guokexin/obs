#include "HandleAskPBX.h"
#include "UntiTool.h"
//#include "ResponseMsg.h"
#include <map>
#include "logManager.h"
#include "HandleUserMsgThread.h"
#include "StdString.h"
#include "obsCfg.h"
#include "Userconn.h"
#include "MarkUp.h"

CHandleAskPBX CHandleAskPBX::m_singleton;
//
CHandleAskPBX* CHandleAskPBX::Instance()
{
	return &m_singleton;
}


CHandleAskPBX::CHandleAskPBX()
    :m_AskMsgRun(0, NULL, NULL, PBXTHREADNUM), m_hSerialEvent(1, NULL, NULL, 1), m_DispatchRun(0, NULL, NULL, 10)
{

    m_strEvtPBX = "";

    m_AskMsgStop = false;
    m_HandAskPBXGrp = -1;

    m_DispatchAskPBXID = 0;
    m_DispatchStop = false;
}

CHandleAskPBX::~CHandleAskPBX()
{

}

BOOL CHandleAskPBX::StartHandleAskPBX()
{
	CLogManager* pLog = CLogManager::Instance();
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
        pLog->m_rootLog.Log("create ask pbx thread fail");
        m_AskMsgStop = true;
        return FALSE;
    }
    else
    {
        pLog->m_rootLog.Log("create ask pbx thread succ");
    }
    ACE_Thread_Manager::instance()->spawn(CHandleAskPBX::DispatchAskPBX,
                                          this,
                                          (THR_NEW_LWP|THR_JOINABLE|THR_INHERIT_SCHED|THR_SUSPENDED),
                                          &m_DispatchAskPBXID
                                         );
    if(m_DispatchAskPBXID==0)
    {
        pLog->m_rootLog.Log("create handle ask pbx thread fail");
        return FALSE;
    }
    else
    {
       	pLog->m_rootLog.Log("create handle ask pbx thread succ");
    }

    ACE_Thread_Manager::instance()->resume_grp(m_HandAskPBXGrp);
    ACE_Thread_Manager::instance()->resume(m_DispatchAskPBXID);

    return TRUE;
}

BOOL CHandleAskPBX::StopHandleAskPBX()
{
    m_AskMsgStop = true;
    m_AskMsgRun.release();
    ACE_Thread_Manager::instance()->wait_grp(m_HandAskPBXGrp);
    m_AskMsgStop = false;
    m_HandAskPBXGrp = -1;

    m_DispatchStop = true;
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
        pHandleAskPBX->m_AskMsgRun.acquire();
        if(pHandleAskPBX->m_AskMsgStop)
        {
            pHandleAskPBX->m_hSerialEvent.release();
            break;
        }
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
	CLogManager* pLog = CLogManager::Instance();
    std::string strEvt;
    while(true)
    {
        m_AskMsgCritical.acquire();
        if(m_AskMsgList.empty())
        {
            m_AskMsgCritical.release();
            break;
        }
        strEvt = m_AskMsgList.front();
        m_AskMsgList.pop_front();
        int ncount = m_AskMsgList.size();
        m_AskMsgCritical.release();

        pLog->m_rootLog.Log("%s", strEvt.c_str());
        pLog->m_rootLog.Log("wait messages %d", ncount);

        m_hSerialEvent.acquire();
        ProcessEvt(strEvt);
        m_hSerialEvent.release();
    }
}

void CHandleAskPBX::ProcessEvt(const std::string &strEvt)
{
	CLogManager* pLog = CLogManager::Instance();
	pLog->m_astLog.Log("##################PBX EVT############################## \r\n%s",strEvt.c_str());
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

        if(strcmp(szEvent,"OriginateResponse") == 0)
        {
            //呼出失败事件
            pLog->m_rootLog.Log("OriginateResponse");
            HandleOriginateResponseEvt(strEvt);
        }
        else if(strcmp(szEvent,"Dial") == 0)
        {
            //拨号成功事件
            pLog->m_rootLog.Log("Dial");
            HandleDialEvt(strEvt);
        }
        else if(strcmp(szEvent,"Bridge") == 0)
        {
            //通话建立事件
            pLog->m_rootLog.Log("Bridge");
            HandleEstablishEvt(strEvt);
        }
        else if(strcmp(szEvent,"Newstate") == 0)
        {
            //震铃事件
            pLog->m_rootLog.Log("Newstate");
            //HandleNewstateEvt(strEvt);
        }
        else if(strcmp(szEvent,"Hangup") == 0)
        {
            //挂机事件
            pLog->m_rootLog.Log("Hangup");
            HandleHangupEvt(strEvt);
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
            pLog->m_rootLog.Log("response");
            HandleResponse(strEvt);
	    	std::cout<<strEvt.c_str()<<std::endl;
        }
    }
}

void CHandleAskPBX::RunDispatchAskPBX()
{
    while(!m_PBXMsgList.empty())
    {
        std::string sTmp;
        m_PBXMsgCritical.acquire();
        sTmp = m_PBXMsgList.front();
        m_PBXMsgList.pop_front();
        m_PBXMsgCritical.release();

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

            m_AskMsgCritical.acquire();
            m_AskMsgList.push_back(strMsg);
            m_AskMsgCritical.release();
            m_AskMsgRun.release();
            int nLen = n+4;
            m_strEvtPBX.erase(0, nLen);
            n = m_strEvtPBX.find("\r\n\r\n");
        }
    }
}



BOOL CHandleAskPBX::HandleHangupEvt(const std::string &strEvt)
{
	CLogManager* pLog = CLogManager::Instance();	
	
    std::cout<<strEvt.c_str()<<std::endl;
    CGeneralUtils myGeneralUtil;
    char szChannel[64];
    memset(szChannel,0,64);
    myGeneralUtil.GetStringValue(strEvt,"Channel",szChannel);
//////////////////////////////////////////////////////////////////////////
    char szCallerIDNum[64];
    memset(szCallerIDNum,0,64);
    myGeneralUtil.GetStringValue(strEvt,"CallerIDNum",szCallerIDNum);
    pLog->m_rootLog.Log("Hangup Event , ani %s",szCallerIDNum);
//////////////////////////////////////////////////////////////////////////
    //m_pDlg->SetLock();

    char szPhoneNo[64];
    memset(szPhoneNo,0,64);
    myGeneralUtil.GetStringValue(strEvt,"phoneno",szPhoneNo);
    std::cout<<"ext "<<szPhoneNo<<std::endl;
    pLog->m_rootLog.Log(" Hangup Event ext %s", szPhoneNo);
    if(strcmp(szPhoneNo,"")!=0)
    {
		
        char szCause[63];
        memset(szCause,0,63);
        myGeneralUtil.GetStringValue(strEvt,"Cause",szCause);

        char ucid[256];
        memset(ucid,0,256);
        myGeneralUtil.GetStringValue(strEvt,"ucid",ucid);

        char szSock[64];
        memset(szSock,0,64);
        myGeneralUtil.GetStringValue(strEvt,"sock",szSock);

        char szCmd[64];
        memset(szCmd,0,64);
        myGeneralUtil.GetStringValue(strEvt,"cmd",szCmd);

        

		std::string no(szPhoneNo);



		CHandleUserMsgThread* pHandleUserMsgThread = CHandleUserMsgThread::Instance();
		pHandleUserMsgThread->m_phoneMapCritical.acquire();
		std::cout<<"size == "<<pHandleUserMsgThread->m_phoneMap.size()<<std::endl;
		std::map<CStdString,CPhone*>::iterator it = pHandleUserMsgThread->m_phoneMap.find(no);

		if(it != pHandleUserMsgThread->m_phoneMap.end())
		{
      pLog->m_rootLog.Log(" Hangup phoneNo %s ", no.c_str());
			CPhone* pPhone = (*it).second;
			//Succ

			std::string msg = "";
			if(pPhone->isSucc)
			{
				
         pLog->m_rootLog.Log(" MakeCall succ " );
				if(strcmp(szCmd,"VoiceCampaign") == 0)
				{

          pLog->m_rootLog.Log(" Hangup Return Msg");
					std::cout<<"Enter Hangup Return Msg"<<std::endl;
				/*	
					msg = "{\"Event\":\"VoiceCampaignResult\",";
					msg += std::string("\"CampaignID\":")+std::string("\"")+std::string(ucid)+std::string("\",");
					msg += std::string("\"PhoneNo\":")+std::string("\"")+std::string(szPhoneNo)+std::string("\",");
					msg += std::string("\"Result\":")+std::string("\"1\"}");	
        */  
          std::string result = "";
          if (strcmp(szCause,"16") == 0) {
            result = "0";
          }
          CMarkup xml;
          xml.AddElem("response");
          xml.AddChildElem("id", ucid);
          xml.AddChildElem("result", result);
          xml.AddChildElem("reason", szCause); 
          msg = xml.GetDoc();
					//std::cout<<msg<<std::endl;
          pLog->m_rootLog.Log(" Hangup send msg %s ", msg.c_str()); 
				}
				else if(strcmp(szCmd,"HumanRecord") == 0)
				{
					
					std::string httpPath;
					httpPath += "http://";
					httpPath += COBSCfg::Instance()->m_sLocalIP.c_str();
					httpPath += "/";
					httpPath += std::string(ucid);
					httpPath += std::string(".wav");
					
					msg = "{\"Event\":\"VoiceRecordResult\",";
					msg += std::string("\"CampaignID\":")+std::string("\"")+std::string(ucid)+std::string("\",");
					msg += std::string("\"VoiceFile\":")+std::string("\"")+httpPath+std::string("\",");
					msg += std::string("\"Result\":")+std::string("\"1\"}");	
				}
                                if (szSock != "0") {
				  CUserConn::Instance()->send_to_softphone(atoi(szSock),msg);
				  sleep(1);
                                }
				pHandleUserMsgThread->m_phoneMap.erase(it);
				delete pPhone;
			}
			else
			{
				
				/*
				if(strcmp(szCmd,"VoiceCampaign") == 0)
				{
					msg = "{\"Event\":\"VoiceCampaignResult\",";
					msg += std::string("\"CampaignID\":")+std::string("\"")+std::string(ucid)+std::string("\",");
					msg += std::string("\"PhoneNo\":")+std::string("\"")+std::string(szPhoneNo)+std::string("\",");
					msg += std::string("\"Result\":")+std::string("\"0\"}");	
				}
				else if(strcmp(szCmd,"HumanRecord") == 0)
				{
					msg = "{\"Event\":\"VoiceRecordResult\",";
					msg += std::string("\"CampaignID\":")+std::string("\"")+std::string(ucid)+std::string("\",");
					msg += std::string("\"VoiceFile\":")+std::string("\"")+std::string("\",");
					msg += std::string("\"Result\":")+std::string("\"0\"}");	
				}
				*/
			}
			
		}
		pHandleUserMsgThread->m_phoneMapCritical.release();
	}
    return TRUE;
}

BOOL CHandleAskPBX::HandleOriginateResponseEvt(const std::string &strEvt)
{

	CHandleUserMsgThread* pHandleUserMsgThread = CHandleUserMsgThread::Instance();
	CUserConn* pUserconn = CUserConn::Instance();
    std::cout<<strEvt.c_str()<<std::endl;
    CGeneralUtils myGeneralUtil;

    char szAction[512];
    memset(szAction,0,512);
    myGeneralUtil.GetStringValue(strEvt,"ActionID",szAction);

    char szResponse[64];
    memset(szResponse,0,64);
    myGeneralUtil.GetStringValue(strEvt,"Response",szResponse);

    char szReason[64];
    memset(szReason, 0, 64);
    myGeneralUtil.GetStringValue(strEvt, "Reason", szReason);
	
    std::string action(szAction);
    size_t pos1 = action.find("|");
    size_t pos2 = action.find("|",pos1+1);
    size_t pos3 = action.find("|",pos2+1);
    size_t pos4 = action.find("|",pos3+1);
    size_t pos5 = action.find("|",pos4+1);

	std::string cmd = action.substr(0,pos1-1);
    std::string no = action.substr(pos1+1,pos2-pos1-1);
    std::string sock = action.substr(pos3+1,pos4-pos3-1);
    std::string ucid = action.substr(pos4+1,pos5-pos4-1);
    std::cout<<no<<" "<<sock<<" "<<ucid<<std::endl;
	
	std::map<CStdString,CPhone*>::iterator it = pHandleUserMsgThread->m_phoneMap.find(no);
	if(it != pHandleUserMsgThread->m_phoneMap.end())
	{
		CPhone* pPhone = (*it).second;
		if(pPhone!=NULL)
		{
			if(strcmp(szResponse,"Failure") == 0)
			{
				pPhone->isSucc = FALSE;
				std::string msg;
        CMarkup xml;
        xml.AddElem("response");
        xml.AddChildElem("id", ucid);
        xml.AddChildElem("result","1");
        xml.AddChildElem("reason", szReason);
        msg = xml.GetDoc();
				int s = atoi(sock.c_str());
				pUserconn->send_to_softphone(s,msg);
				sleep(1);
				CPhone* pPhone = (*it).second;
				delete pPhone;
				pHandleUserMsgThread->m_phoneMap.erase(it);
			}
			else if(strcmp(szResponse,"Success") == 0)
			{
				pPhone->isSucc = TRUE;
			}	
		}
	}
	return TRUE;
}
/*
    if(strcmp(szResponse,"Failure") == 0)
    {
		
    	std::string action(szAction);
    	size_t pos1 = action.find("|");
    	size_t pos2 = action.find("|",pos1+1);
    	size_t pos3 = action.find("|",pos2+1);
    	size_t pos4 = action.find("|",pos3+1);
    	size_t pos5 = action.find("|",pos4+1);

		std::string cmd = action.substr(0,pos1-1);
    	std::string no = action.substr(pos1+1,pos2-pos1-1);
    	std::string sock = action.substr(pos3+1,pos4-pos3-1);
    	std::string ucid = action.substr(pos4+1,pos5-pos4-1);
    	std::cout<<no<<" "<<sock<<" "<<ucid<<std::endl;

	
		std::map<CStdString,CPhone*>::iterator it = pHandleUserMsgThread->m_phoneMap.find(no);
		if(it != pHandleUserMsgThread->m_phoneMap.end())
		{
			std::string msg ;
			if(cmd == "HumanRecord")
			{
				msg = "{\"Event\":\"VoiceRecordResult\",";
			}
			else
			{
				msg = "{\"Event\":\"VoiceCampaignResult\",";
			}
			msg += std::string("\"CampaignID\":")+std::string("\"")+std::string(ucid)+std::string("\",");
			if(cmd == "HumanRecord")
			{
				msg += std::string("\"VoiceFile\":","\"\"");
			}
			else
			{
				msg += std::string("\"PhoneNo\":")+std::string("\"")+no+std::string("\",");
			}
			msg += std::string("\"Result\":")+std::string("\"0\"}");	
			int s = atoi(sock.c_str());
			pUserconn->send_to_softphone(s,msg);
			CPhone* pPhone = (*it).second;
			delete pPhone;
			pHandleUserMsgThread->m_phoneMap.erase(it);
			
		}
    	else if(strcmp(szResponse,"Success") == 0)
		{
			
			
		}
    }
	

}
*/
BOOL CHandleAskPBX::HandleDialEvt(const std::string &strEvt)
{
	CLogManager* pLog = CLogManager::Instance();
    BOOL bRet = FALSE;
    pLog->m_rootLog.Log("TEST_LOG->HandDialEvt:%s",strEvt.c_str());
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

    pLog->m_rootLog.Log("TEST_LOG->caller=%s",szCaller);
    CUntiTool tool;
    //m_pDlg->SetLock();
/*    
    CAgent* pAgent = m_pDlg->GetAgentFromStation(szCaller);
    if(pAgent)
    {

        char szCalled[32];
        memset(szCalled,0,32);
        myGeneralUtil.GetStringValue(strEvt,"Called",szCalled);

        char szUCID[128];
        memset(szUCID,0,128);
        myGeneralUtil.GetStringValue(strEvt,"Ucid",szUCID);

	std::cout<<"ucid="<<szUCID<<std::endl;

        char szDirection[32];
        memset(szDirection,0,32);
        myGeneralUtil.GetStringValue(strEvt,"Direction",szDirection);

        char szTenantId[128];
        memset(szTenantId,0,128);
        myGeneralUtil.GetStringValue(strEvt,"TenantId",szTenantId);

        char szCalledGatewayCaller[32];
		//新增协议：只有当被叫号码为PSTN号码时，CalledGatewayCaller字段才被设置
        memset(szCalledGatewayCaller,0,32);
        myGeneralUtil.GetStringValue(strEvt,"CalledGatewayCaller",szCalledGatewayCaller);

        pAgent->lock();
        pAgent->sDnis = szCalled;

        std::string sCalledGatewayCaller = szCalledGatewayCaller;

        pAgent->sAni = szCaller;


        pAgent->sCallDirect = szDirection;
        pAgent->sUCID = szUCID;
        pAgent->sTeantid = szTenantId;
        pAgent->sSelfChanID = szChan;
        pAgent->AgentState = AS_Dial;

        pAgent->m_callID = szChan; //Channel就是callID
        pAgent->m_accessNumber = szAccessNumber;
        pAgent->m_ivrTrack = szAccessNumber;//2012-11-30暂定ivr号和接入号一样
        pAgent->unlock();

        m_pDlg->m_Log.Log("TEST_LOG->HandDialEvt:Ext=%s Chan=%s",pAgent->sStation.c_str(), szChan);


        m_pDlg->SetAgentToChan(szChan,pAgent);



        CMakeCallEvt evt;
        evt.m_strRet = "Succ";
        evt.m_strAni = szCaller;
        evt.m_strDnis = szCalled;
        evt.m_strUCID = szUCID;
        evt.m_strDirector = szDirection;
        CUntiTool tool;
        evt.m_strTime = tool.GetCurrTime();
        evt.m_strStation = pAgent->sStation;
        evt.m_strAgentId = pAgent->sWorkNo;

        CAgentOp op;
        op.SendResult(pAgent,evt.EnSoftPhoneMsg());

        pAgent->lock();
        SetWSCDRData(pAgent);
        SetAgentStatusReportData(pAgent);
        pAgent->unlock();
        pAgent->Release();

        bRet = TRUE;


    }
    else
    {
        bRet = FALSE;
        m_pDlg->m_Log.Log("FLOW_LOG->HandDialEvt:Ext=%s,Chan=%s,没有发现通道对应的分机",szCaller,szChan);
    }
*/
    return bRet;
}

BOOL CHandleAskPBX::HandleEstablishEvt(const std::string &strEvt)
{
	CLogManager* pLog = CLogManager::Instance();

	pLog->m_astLog.Log("HandleEstablishEvt \r\n %s",strEvt.c_str());
    CGeneralUtils myGeneralUtil;

    char szBridgeState[32];
    memset(szBridgeState,0,32);
    myGeneralUtil.GetStringValue(strEvt,"Bridgestate",szBridgeState);

    if(strcmp(szBridgeState,"Unlink")==0)
	return 0;
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


    std::string tChan1 = CUntiTool::getValidChanFromOrigChan(szChan1);
    std::string tChan2 = CUntiTool::getValidChanFromOrigChan(szChan2);

    //tChan1.find("AsyncGoto/");
    
    pLog->m_rootLog.Log("#################chan1 = %s ,  chan2 = %s ",tChan1.c_str(),tChan2.c_str());
    //m_pDlg->SetLock();

	
	CHandleUserMsgThread* pHandleUserMsg = CHandleUserMsgThread::Instance();
	
	std::map<CStdString,CPhone*>::iterator it = pHandleUserMsg->m_phoneMap.find(CStdString(szCalled));
	if( it != pHandleUserMsg->m_phoneMap.end() )
	{
		CPhone* pPhone = (*it).second;
		if(pPhone != NULL)
		{
			pPhone->isSucc = TRUE;
			pLog->m_astLog.Log("%s make call succ",pPhone->no.c_str());
		}
	}
	
    

    return TRUE;
}






BOOL CHandleAskPBX::HandleResponse(const std::string &strEvt)
{
    //CResponseMsg msg;

    //std::string strBody = msg.ParaAskPBXResponse(strEvt);


    return TRUE;
}




