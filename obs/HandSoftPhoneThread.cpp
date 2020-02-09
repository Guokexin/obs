#include "HandSoftPhoneThread.h"
#include "./logclass/log.h"
#include "AskProxyDlg.h"
#include "askproxydef.h"
#include "Agent.h"
//#include "WS.h"
#include "./AskMsg/XmlMarkup.h"
#include "UntiTool.h"
#include "./AskMsg/EnforceLogOutEvt.h"
#include "./GenCMD.h"
#include "AgentOp.h"
#include "ResponseMsg.h"
#include "GeneralUtils.h"
#include <string>
CHandSoftPhoneThread::CHandSoftPhoneThread()
    :m_hStartHandleSoftPhoneMsg(0, NULL, NULL, 10)
{
    m_hStopHandleSoftPhoneMsg = false;
    m_pHandleSoftPhoneThread = 0;
    m_webSock = -1;
}

CHandSoftPhoneThread::~CHandSoftPhoneThread()
{

}

BOOL CHandSoftPhoneThread::StartHandleSoftPhoneMsg(CAskProxyDlg* pDlg)
{
    m_pDlg = pDlg;
    m_pDlg->m_Log.Log("TEST_LOG->Start Handle SoftPhone Msg");

//	m_hStartHandleSoftPhoneMsg = CreateEvent(NULL,FALSE,FALSE,"start handle softphone msg"+CUntiTool::GenUCID());

//	m_hStopHandleSoftPhoneMsg = CreateEvent(NULL,FALSE,FALSE,"stop handle softphone msg"+CUntiTool::GenUCID());

//	m_pHandleSoftPhoneThread = AfxBeginThread(CHandSoftPhoneThread::HandleSoftPhoneMsg,this,THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED,NULL);

    ACE_Thread_Manager::instance()->spawn(CHandSoftPhoneThread::HandleSoftPhoneMsg,
                                          this,
                                          (THR_NEW_LWP|THR_JOINABLE|THR_INHERIT_SCHED|THR_SUSPENDED),
                                          &m_pHandleSoftPhoneThread
                                         );

    if(m_pHandleSoftPhoneThread!=0)
    {
        m_pDlg->m_Log.Log("TEST_LOG->thread handlesoftphonemsg succ");
    }
    else
    {
        m_pDlg->m_Log.Log("TEST_LOG->thread handlesoftphonemsg fail");
    }

//	m_pHandleSoftPhoneThread->ResumeThread();

    ACE_Thread_Manager::instance()->resume(m_pHandleSoftPhoneThread);

    return 1;


}

LPVOID  CHandSoftPhoneThread::HandleSoftPhoneMsg(LPVOID lpvoid)
{
    CHandSoftPhoneThread* pThis = reinterpret_cast<CHandSoftPhoneThread*>(lpvoid);
//	HANDLE hEvent[2];
//	hEvent[0] = pThis->m_hStopHandleSoftPhoneMsg;
//    hEvent[1] = pThis->m_hStartHandleSoftPhoneMsg;

    while(true)
    {
        //DWORD ret = WaitForMultipleObjects(2,hEvent,FALSE,INFINITE);
        //DWORD ret = WaitForMultipleObjects(2,hEvent,FALSE,1000);//bug:12
        pThis->m_hStartHandleSoftPhoneMsg.acquire();
        if(pThis->m_hStopHandleSoftPhoneMsg)
        {
            pThis->m_pDlg->m_Log.Log("FLOW_LOG->HandleSoftPhoneMsg Thread Exit");
            break;
        }
        else
        {
            pThis->RunHandleSoftPhoeMsg();
        }
    }

    return NULL;
}

BOOL    CHandSoftPhoneThread::StopHandleSoftPhoneMsg()
{
    m_hStopHandleSoftPhoneMsg = true;
    m_hStartHandleSoftPhoneMsg.release();
    ACE_Thread_Manager::instance()->join(m_pHandleSoftPhoneThread);
    m_hStopHandleSoftPhoneMsg = false;
    m_pHandleSoftPhoneThread = 0;
    return 1;
}

void CHandSoftPhoneThread::ParaMsg(std::string& smsg)
{
    size_t index = smsg.find("\"");
    while(index !=std::string::npos)
    {
         smsg.replace(index,strlen("\""),"");
         index = smsg.find("\"");
    }
    
    std::cout<<smsg.c_str()<<std::endl;

    index = smsg.find(":");
    while(index !=std::string::npos)
    {
          smsg.replace(index,strlen(":"),"=");
          index = smsg.find(":");
    }
    std::cout<<smsg.c_str()<<std::endl;


    index = smsg.find(",");
    while(index !=std::string::npos)
    {
          smsg.replace(index,strlen(","),";");
          index = smsg.find(",");
    }
    std::cout<<smsg.c_str()<<std::endl;

    index = smsg.find("\r\n");
    while(index !=std::string::npos)
    {
          smsg.replace(index,strlen("\r\n"),"");
          index = smsg.find("\r\n");
    }
    std::cout<<smsg.c_str()<<std::endl;

    smsg.replace(0,1,"");
    smsg.replace(smsg.length()-1,1,"");
    std::cout<<smsg.c_str()<<std::endl;
  
  
    std::cout<<smsg.c_str()<<std::endl;
    
}
void CHandSoftPhoneThread::RunHandleSoftPhoeMsg()
{
    while(!m_pDlg->m_AgentMsgList.empty())
    {
        m_pDlg->m_SoftPhoneMsgCritical.acquire();
        AgentMsg* pAgentMsg = m_pDlg->m_AgentMsgList.front();
        m_pDlg->m_AgentMsgList.pop_front();
        m_pDlg->m_SoftPhoneMsgCritical.release();

        m_pDlg->m_AgentLog.Log("FLOW_LOG->@@@%s",pAgentMsg->sMsg.c_str());


        //CHeadMsg headmsg;
        //headmsg.ParaMsg(pAgentMsg->sMsg);
       

        std::string sMsg = pAgentMsg->sMsg;
        int s = pAgentMsg->s;
	//
	
	//guokx added to obs
		ParaMsg(sMsg);
        CGeneralUtils myGeneralUtil;

        char cmd[64];
        memset(cmd,0,64);
        myGeneralUtil.GetStringValue(sMsg,"Cmd",cmd);
		std::cout<<"Cmd="<<cmd<<std::endl;


		char ucid[256];
		memset(ucid,0,256);
		myGeneralUtil.GetStringValue(sMsg,"CampaignID",ucid);	
		std::cout<<"ucid="<<ucid<<std::endl;



		if(strcmp(cmd,"VoiceCampaign")==0)
		{
			int nLen = sMsg.length();
			char phones[nLen];
			memset(phones,0,nLen);
			myGeneralUtil.GetStringValue(sMsg,"PhoneNo",phones);
			std::cout<<phones<<std::endl;
		
			char voicePath[256];
			memset(voicePath,0,256);
			myGeneralUtil.GetStringValue(sMsg,"VoiceFile",voicePath);
			std::cout<<voicePath<<std::endl;
			std::string sVoicePath(voicePath);
			size_t pos = sVoicePath.find("http");
			std::string sFileName;
			if(pos != std::string::npos)
			{
				//std::cout<<"#######################################"<<std::endl;
				size_t pos = sVoicePath.rfind("/");
				sFileName = sVoicePath.substr(pos+1,std::string::npos);
				std::cout<<sFileName<<std::endl;
				pos = sFileName.find(".wav");
				sFileName = sFileName.replace(pos,strlen(".wav"),"");	
				std::cout<<sFileName<<std::endl;
				
			}

		//
			char caller[128];
			memset(caller,0,128);
			myGeneralUtil.GetStringValue(sMsg,"ANI",caller);
			std::cout<<caller<<std::endl;

			std::string sPhones(phones);
	
			sPhones += "^";
			size_t beginPos = 0;
    		size_t index = sPhones.find("^");
    		while(index !=std::string::npos)
    		{
				size_t count = m_pDlg->m_phoneMap.size();
				if(count>=10)
				{
					std::cout<<"Beyond your licences , Please contact your SP"<<std::endl;
					std::cout<<"Please Wait for call ended"<<std::endl;
					sleep(3);
		
					continue;
				}
			
				std::string phoneNo = sPhones.substr(beginPos,index-beginPos);
				std::cout<<"ext : "<<phoneNo<<std::endl;
				CPhone *pPhone = new CPhone();
				pPhone->no = phoneNo;
				pPhone->ucid = ucid;
				
				//std::string sVoiceFile(voicePath);
				//size_t index = sVoicePath.find("=");
				//sVoicePath.replace(index,strlen("="),":");

				//std::cout<<"VoicePath = "<<sVoicePath<<std::endl;
				pPhone->voicefile = sFileName;//"http://192.168.0.8/EF011F79-4000-406D-87AF-BFFB3FC39D57.wav";//sVoicePath;//voicePath;
				//
				pPhone->cmd = cmd;
				pPhone->fd = s;
				pPhone->status = "0";
				pPhone->caller = caller;
				pPhone->SetMainWnd(m_pDlg);
				pPhone->SetMainSocket(&m_pDlg->m_SockManager);

				//std::cout<<phoneNo.c_str()<<std::endl;
				m_pDlg->m_phoneMapCritical.acquire();
				m_pDlg->m_phoneMap.insert(std::make_pair(phoneNo,pPhone));
				m_pDlg->m_phoneMapCritical.release();

				pPhone->MakeCall();
				//
				//m_pDlg->m_obsCountCtitical.acquire();
				//m_pDlg->m_obsCount++;
				//m_pDlg->m_obsCountCtitical.release();
				//m_phoneMapCritical.release();	
				beginPos = index+1;			
				index = sPhones.find("^",beginPos);
				sleep(1);
				//delete pPhone;
    		}
		
		//std::string phone = sPhones.substr(beginPos,std::string::npos);
		//std::cout<<phone.c_str()<<std::endl;
		//CPhone* pPhone = new CPhone();
		//pPhone->no = phone;
		//pPhone->ucid = ucid;
		//pPhone->voicefile = voicePath;
		//pPhone->cmd = cmd;
		//pPhone->fd = s;
		//m_phoneMap.insert(std::make_pair(phone,pPhone));
		//pPhone->MakeCall();
	}
	else if(strcmp(cmd,"VoiceRecord") == 0)
	{

		std::cout<<"Enter VoiceRecord"<<std::endl;
		char szExt[32];
		memset(szExt, 0,  32);
		myGeneralUtil.GetStringValue(sMsg,"EXT",szExt);
		
		CPhone *pPhone = new CPhone;
		pPhone->cmd = cmd;
		pPhone->ucid = ucid;
		pPhone->fd = s;
		pPhone->no = szExt;
		pPhone->SetMainWnd(m_pDlg);
		pPhone->SetMainSocket(&m_pDlg->m_SockManager);
		m_pDlg->m_phoneMapCritical.acquire();
		m_pDlg->m_phoneMap.insert(std::make_pair(pPhone->no,pPhone));
		m_pDlg->m_phoneMapCritical.release();
	
		pPhone->HumanRecord();
	}

	if(pAgentMsg != NULL)
	{
		delete pAgentMsg;
		pAgentMsg  = NULL;
	}
	
    }
}



BOOL  CHandSoftPhoneThread::HandleAgentLogin(const std::string &strMsg,int sock)
{
    BOOL bRet = FALSE;
    //BOOL bReLogin =FALSE;
    CLoginEvt msg;

    std::string strTenantId;
    std::map<std::string, std::string> mapSkillAndPenalty;
    msg.ParaSoftPhoneRequestMsg(strMsg);


    CResponseMsg msgResp;
    CUntiTool tool;
    msgResp.m_strTime = tool.GetCurrTime();
    msgResp.m_strStation = msg.m_strStation;
    msgResp.m_strAgentId = msg.m_strWorkNo;
    msgResp.m_pbxIP = m_pDlg->m_SettingData.m_strPBX;
    //////////////////////////////////////////////////////////////////////////
    //2013-01-10 add by chenlin
    m_pDlg->m_Log.Log("FLOW_LOG->Source=%s",msg.m_strSource.c_str());
    //////////////////////////////////////////////////////////////////////////
    m_pDlg->m_Log.Log("FLOW_LOG->WorkNo = %s",msg.m_strWorkNo.c_str());
    m_pDlg->m_Log.Log("FLOW_LOG->Station = %s",msg.m_strStation.c_str());
    m_pDlg->m_Log.Log("FLOW_LOG->UserData = %s",msg.m_strUserData.c_str());
    m_pDlg->m_Log.Log("FLOW_LOG->password = %s",msg.m_strPwd.c_str());
    m_pDlg->m_Log.Log("FLOW_LOG->SOCKET = %d", sock);
    m_pDlg->m_Log.Log("FLOW_LOG->Version = %s",msg.m_sSoftPhoneVersion.c_str());
    m_pDlg->m_Log.Log("FLOW_LOG->ReLogin = %d",msg.m_bIsAgentReConn);
    m_pDlg->m_Log.Log("FLOW_LOG->Addr = %s",msg.m_strClientAddr.c_str());
    m_pDlg->m_Log.Log("FLOW_LOG->Status = %d",msg.m_nDefStatus);
    m_pDlg->m_Log.Log("FLOW_LOG->HangupStatus = %d", msg.m_HangupStatus);
    m_pDlg->m_Log.Log("FLOW_LOG->Skills = %s",msg.m_strSkills.c_str());
    m_pDlg->m_Log.Log("FLOW_LOG->LoginType=%s",msg.m_strLoginType.c_str());
    m_pDlg->m_Log.Log("FLOW_LOG->ViewCaller=%s",msg.m_strViewCaller.c_str());

    std::cout<<"Source "<<msg.m_strSource.c_str()<<std::endl;
    std::cout<<"WorkNo "<<msg.m_strWorkNo.c_str()<<std::endl;
    std::cout<<"Station "<<msg.m_strStation.c_str()<<std::endl;
    std::cout<<"UserData "<<msg.m_strUserData.c_str()<<std::endl;
    std::cout<<"Relogin "<<msg.m_bIsAgentReConn<<std::endl;
    std::cout<<"Skills "<<msg.m_strSkills.c_str()<<std::endl;
    std::cout<<"LoginType "<<msg.m_strLoginType.c_str()<<std::endl;
    std::cout<<"ViewCaller "<<msg.m_strViewCaller.c_str()<<std::endl;
    
    /************************************************************************/
    /*                                                                      */
    if(msg.m_strStation == "")
    {
        //防止分机为空的座席来捣乱
        msgResp.m_strRet = "Fail";
        //msgResp.m_strUserData.Format("分机不允许为空！");
        msgResp.m_strUserData = "分机不允许为空！";
        msgResp.m_strOperation = "Login";

        std::string sAskMsg = msgResp.EncodeResp();
        m_pDlg->m_IOOpt.SendMsgToUser(sock,sAskMsg);
        m_pDlg->m_Log.Log("FLOW_LOG->Login Fail, Cause %s",msgResp.m_strUserData.c_str());
//		usleep(200*1000);
//		//shutdown(sock,SD_BOTH);
//		//closesocket(sock);//关闭它
//		sock->closeServer();
//
//		delete sock;
        CloseConn(msg.m_strSource, sock);
        return FALSE;
    }
    /************************************************************************/

    if(msg.m_bIsAgentReConn)
    {
        //CTI-客户端 重连通过CAgentReConnHandle处理
        //重新关联socket句柄
        //执行重连的时候，先要锁住表


        //m_pDlg->SetLock();
        CAgent* pAgent = m_pDlg->GetAgentFromStation(msg.m_strStation);
        if (pAgent)
        {
            //坐席已经存在才重连，否则走登录验证流程
            //////////////////////////////////////////////////////////////////////////
            //2013-01-22 add by chenlin
            //既然是重连，需要重连的工号和分机号与已经登录的保持一致
            //针对手机端，PC端暂不限制，因为PC端客户端逻辑会避免此问题/
            //2013-04-28 经过测试与讨论，决定实行工号分机一一对应
            m_pDlg->m_Log.Log("FLOW_LOG->分机%s-工号%s 执行重连逻辑开始",msg.m_strStation.c_str(), msg.m_strWorkNo.c_str());
            if(pAgent->sWorkNo == msg.m_strWorkNo && pAgent->m_strSource == msg.m_strSource)
            {
                //工号和客户端类型都对应上，才认为是合法的重连
                m_pDlg->m_Log.Log("FLOW_LOG->%s客户端-分机%s-工号%s-重连验证通过，开始重连",msg.m_strSource.c_str(), msg.m_strStation.c_str(), msg.m_strWorkNo.c_str());
                pAgent->lock();
                if(pAgent->sock != sock)
                {
                    CloseConn(msg.m_strSource, pAgent->sock);
                    pAgent->sock = sock;
                }
                pAgent->m_strClientAddr = msg.m_strClientAddr;
                pAgent->m_nDefStatus = msg.m_nDefStatus;
                pAgent->unlock();
                //if(pAgent->sSendResult_Data != "")
                if(pAgent->sSendResult_Data != "" && pAgent->m_strSource == _T("SoftPhone"))
                {
                    CAgentOp op;
                    //BOOL ret = m_pDlg->m_IOOpt.SendMsgToUser(pAgent->sock,pAgent->sSendResult_Data);
                    m_pDlg->m_IOOpt.SendMsgToUser(pAgent->sock,pAgent->sSendResult_Data);
                }
                else
                {
                    //一般情况下不会为空？？除非是发送action后，ast没有响应，造成为空了！！
                    //这种情况一般为坐席被签出ast，故重新登录
                    m_pDlg->m_Log.Log("FLOW_LOG->重连时SendResult的为空");
                    pAgent->Login();
                }

                //COleDateTime ti = COleDateTime::GetCurrentTime();
                pAgent->lock();
                pAgent->preLiveTime = CUntiTool::GetCurrTimeDate();
                pAgent->preLiveTimeSecond = time(NULL);
                pAgent->unlock();
                bRet = TRUE;
            }
            else
            {
                //存在对象，但是信息不匹配的，不让登录
                msgResp.m_strRet = "Fail";
                //msgResp.m_strUserData.Format("重连失败，分机%s已经被工号%s签入",msg.m_strStation,pAgent->sWorkNo);
                msgResp.m_strUserData = "重连失败，分机" + msg.m_strStation + "已经被工号" + pAgent->sWorkNo + "签入";
                msgResp.m_strOperation = "Login";

                std::string sAskMsg = msgResp.EncodeResp();
                m_pDlg->m_IOOpt.SendMsgToUser(sock,sAskMsg);
                m_pDlg->m_Log.Log("FLOW_LOG->重连失败，原因： %s",msgResp.m_strUserData.c_str());
                usleep(200*1000);
//				//shutdown(sock,SD_BOTH);
//				//closesocket(sock);//关闭它
//				sock->closeServer();
//				delete sock;
                CloseConn(msg.m_strSource, sock);
                bRet = FALSE;

            }
            m_pDlg->m_Log.Log("FLOW_LOG->%s 执行重连逻辑结束",msg.m_strStation.c_str());
            //重连的逻辑走完，应该返回，以免继续验证登录
            pAgent->Release();
            return bRet;
        }

        else
        {
            m_pDlg->m_Log.Log("FLOW_LOG->重连-没有获取到分机%s的对象，执行登录验证",msg.m_strStation.c_str());
            //m_pDlg->SetUnLock();
        }
    }

    //
    //第0步 ，判断版本号是否被支持
    //
    if(msg.m_strSource != "C_WEB" && msg.m_strSource != "S_WEB")
    {

        if(msg.m_sSoftPhoneVersion == "")
        {
            //为了支持老版本软电话增加该配置
            msgResp.m_strRet = "Fail";
            //msgResp.m_strUserData.Format("客户端版本%s不被支持",msg.m_sSoftPhoneVersion);
            msgResp.m_strUserData = "客户端版本" + msg.m_sSoftPhoneVersion + "不被支持";
            msgResp.m_strOperation = "Login";

            std::string sAskMsg = msgResp.EncodeResp();
            m_pDlg->m_IOOpt.SendMsgToUser(sock,sAskMsg);
            m_pDlg->m_Log.Log("FLOW_LOG->Login Fail, Cause %s",msgResp.m_strUserData.c_str());
            //usleep(200*1000);
//		//shutdown(sock,SD_BOTH);
//		//closesocket(sock);//关闭它
//		sock->closeServer();
//		delete sock;
            CloseConn(msg.m_strSource, sock);
            return FALSE;
        }
        else
        {
            size_t bFind = m_pDlg->m_SettingData.m_strSupportVersion.find(msg.m_sSoftPhoneVersion);
            if(bFind == std::string::npos)
            {
                msgResp.m_strRet = "Fail";
                //msgResp.m_strUserData.Format("客户端版本%s不被支持",msg.m_sSoftPhoneVersion);
                msgResp.m_strUserData = "客户端版本" + msg.m_sSoftPhoneVersion + "不被支持";
                msgResp.m_strOperation = "Login";

                std::string sAskMsg = msgResp.EncodeResp();
                m_pDlg->m_IOOpt.SendMsgToUser(sock,sAskMsg);
                m_pDlg->m_Log.Log("FLOW_LOG->Login Fail, Cause %s",msgResp.m_strUserData.c_str());
                //usleep(200*1000);
//			//shutdown(sock,SD_BOTH);
//			//closesocket(sock);//关闭它
//			sock->closeServer();
//			delete sock;
                CloseConn(msg.m_strSource, sock);
                return FALSE;
            }

        }
    }
    //
    m_pDlg->m_Log.Log("FLOW_LOG->工号 %s 版本 %s 验证通过",msg.m_strWorkNo.c_str(), msg.m_sSoftPhoneVersion.c_str());





    BOOL bLogin = FALSE;
    //第一步，检测分机工号是否已经被签入
    m_pDlg->m_Log.Log("TEST_LOG->第一步:判断工号是否被签入");
    std::string strStation;
    bLogin = m_pDlg->AgentIsLogin(msg.m_strWorkNo,strStation);
    if(bLogin)
    {
        msgResp.m_strRet = "Fail";
        //msgResp.m_strUserData = "工号已经被签入";
        //msgResp.m_strUserData.Format("工号已经被分机%s签入",strStation);
        msgResp.m_strUserData = "工号已经被分机" + strStation + "签入";
        msgResp.m_strOperation = "Login";

        std::string sAskMsg = msgResp.EncodeResp();
        m_pDlg->m_IOOpt.SendMsgToUser(sock,sAskMsg);
        m_pDlg->m_Log.Log("FLOW_LOG->Login Fail, Cause %s",msgResp.m_strUserData.c_str());
        //usleep(200*1000);
//		//shutdown(sock,SD_BOTH);
//		//closesocket(sock);//关闭它
//		sock->closeServer();
//		delete sock;
        CloseConn(msg.m_strSource, sock);
        return FALSE;
    }
    else
    {
        //检测分机
        m_pDlg->m_Log.Log("FLOW_LOG->工号验证通过,进入下一步验证是否已经被登录过");
        m_pDlg->m_Log.Log("TEST_LOG->第二步:检测分机是否被签入");
        //m_pDlg->SetLock();
        CAgent* pAgent = m_pDlg->GetAgentFromStation(msg.m_strStation);
        if(pAgent)
        {
            std::string strRet = _T("Fail");
            std::string strCause;
            //strCause.Format(_T("分机已经被工号%s签入"),pAgent->sWorkNo);
            pAgent->lock();
            strCause = "分机已经被工号" + pAgent->sWorkNo + "签入";
            pAgent->unlock();

            msgResp.m_strRet = strRet;
            msgResp.m_strUserData = strCause;
            msgResp.m_strOperation = "Login";

            std::string sAskMsg = msgResp.EncodeResp();

            m_pDlg->m_IOOpt.SendMsgToUser(sock,sAskMsg);
            m_pDlg->m_Log.Log("FLOW_LOG->Login Fail, Cause %s",strCause.c_str());
            //usleep(200*1000);
//			//shutdown(sock,SD_BOTH);
//			//closesocket(sock);
//			sock->closeServer();
//			if(sock != pAgent->sock)
//                delete sock;
            CloseConn(msg.m_strSource, sock);
            pAgent->Release();
            return FALSE;
        }
        //m_pDlg->SetUnLock();
    }

    //第三步，密码验证

     /// 测试环境，没有密码验证，直接通过即可。
    m_pDlg->m_Log.Log("FLOW_LOG->分机验证通过,进入下一步验证是否已经被登录过");
    m_pDlg->m_Log.Log("TEST_LOG->第三步:密码验证");
    


    //3---工号所在的队列和租户信息

    m_pDlg->m_Log.Log("FLOW_LOG->密码验证通过,进入下一步验证是否已经被登录过");
    m_pDlg->m_Log.Log("TEST_LOG->第四步:请求工号所在的队列和租户信息");



    m_pDlg->m_Log.Log("FLOW_LOG->成功验证分机=%s, 允许登录",msg.m_strStation.c_str());
    m_pDlg->m_Log.Log("FLOW_LOG->第六步：获取分机类型");



    //m_pDlg->SetLock();
    m_pDlg->SetAgentToStation(msg.m_strStation);
    CAgent* pAgent = m_pDlg->GetAgentFromStation(msg.m_strStation);
    if(pAgent)
    {

        m_pDlg->m_Log.Log("TEST_LOG->HandleAgentLogin: Station=%s 签入",msg.m_strStation.c_str());

        //////////////////////////////////////////////////////////////////////////
        //2013-01-10 add by chenlin
        pAgent->lock();
        pAgent->m_strSource = msg.m_strSource;
        //////////////////////////////////////////////////////////////////////////
        pAgent->sAction = "Login";
        /************************************************************************/
        //edit by chenlin
        //pAgent->sQueueName = sQueueName;
        //------------add by jyf-------------------------------------
        //增加技能组号和对应的惩罚度

        std::string strSkills = "" ; 

	CUntiTool tool;
	std::list<std::string> li;
	tool.ParseString(msg.m_strSkills,',',li);
	
        m_pDlg->m_Log.Log("ParseSring to List %d",li.size());
	std::list<std::string>::iterator it;
	for(it=li.begin();it!=li.end();it++)
	{
		std::cout<<(*it)<<"******************"<<std::endl;
		tool.ParseString((*it),':',pAgent->m_mapSkillAndPenalty);
	}

        m_pDlg->m_Log.Log("ParseSring to map, %d",pAgent->m_mapSkillAndPenalty.size());

	std::map<std::string,std::string>::iterator it1;
	for(it1=pAgent->m_mapSkillAndPenalty.begin();it1!=pAgent->m_mapSkillAndPenalty.end();it1++)
	{
		strSkills += (*it1).first;
		strSkills += ",";
		std::cout<<strSkills.c_str()<<"########################"<<std::endl;
	}

        m_pDlg->m_Log.Log("Reset skills %s",strSkills.c_str());
	strSkills.erase(strSkills.length()-1,1);

        m_pDlg->m_Log.Log("TEST_LOG->HandleAgentLogin: Station=%s 签入",msg.m_strStation.c_str());
/*
        std::map<std::string, std::string>::iterator pos = mapSkillAndPenalty.begin();
        while(pos!=mapSkillAndPenalty.end())
        {
            //std::string skill,penalty;
            //mapSkillAndPenalty.GetNextAssoc(pos,skill,penalty);
            pAgent->m_mapSkillAndPenalty[pos->first] = pos->second;
            strSkills += pos->first + ",";
            ++pos;
            //strSkills += ",";
        }
        strSkills.erase(strSkills.length()-1, 1);
*/
        pAgent->m_tempSkill = strSkills;
        pAgent->m_strAllSkill = strSkills;
        m_pDlg->m_Log.Log("TEST_LOG->获取到的技能组：%s",msg.m_strSkills.c_str());
        // end---------------2013-01-30----------------------------------
        /************************************************************************/
        pAgent->AgentinitState = BUSY_STATE;
        pAgent->SetMainWnd(m_pDlg);
        pAgent->SetMainSocket(&m_pDlg->m_SockManager);
        pAgent->sTimeout = "60000";

        pAgent->sContext = "default";
        pAgent->sTeantid = strTenantId;
        pAgent->sock = sock;

        pAgent->m_strClientAddr = msg.m_strClientAddr;
        pAgent->m_nDefStatus = msg.m_nDefStatus;
        pAgent->m_HangupStatus = msg.m_HangupStatus;

        pAgent->sWorkNo = msg.m_strWorkNo;
        pAgent->sStation = msg.m_strStation;
        pAgent->sPwd = msg.m_strPwd;
        pAgent->sUserData = msg.m_strUserData;

        pAgent->sock = sock;

        pAgent->m_strConfigCaller=msg.m_strViewCaller;//sConfigCaller;
        pAgent->m_strpstnAgentPrefix="";//sConfigpstnAgentPrefix;

	pAgent->m_sStationType = msg.m_strLoginType;


        /************************************************************************/
        /*    PSTN号码定义变更20111223
              呼叫>6位的PSTN号码：PBX使用号码填充CalledGatewayCaller字段
              呼叫<=6位的PSTN号码：PBX使用号码填充CalledGatewayCaller字段
        	  PSTN号码定义变更20120331
        	  sip/util/getSysNumType?num=<号码>  通过Web方式获取协议
        */
        /************************************************************************/
        if(pAgent->m_sStationType=="PSTN")
        {
            //pstn号码：手机号码或者座机号码，补充20111223：不支持<=6位的PSTN号码
            //<工号ID>,<租户ID>,<当前通话数>
            pAgent->LoginTalkData();//pstn登录设置

            //"SIP/%s@192.168.20.90"
            //bug: 接入PBX从CMService获取
            //pAgent->sChan.Format("SIP/%s@%s",pAgent->sStation,m_pDlg->m_SettingData.m_strInPBX);//modify by ly
            pAgent->sChan = "SIP/" + pAgent->sStation + "@" + m_pDlg->m_SettingData.m_strInPBX;
        }
        else
        {
            pAgent->LoginSipTalkData();//sip登录设置
            //pAgent->sChan.Format("SIP/%s",pAgent->sStation);
            pAgent->sChan = "SIP/" + pAgent->sStation;
        }
        pAgent->sSipInterface = pAgent->sChan;
        pAgent->m_strAgentCDRReport = m_pDlg->m_strCdrWeb;




        //--开始登录
        pAgent->unlock();
        pAgent->Login();
        pAgent->Release();

    }
    else
    {
        msgResp.m_strRet = "Fail";
        msgResp.m_strUserData =_T("分配坐席实例失败");
        msgResp.m_strOperation = "Login";

        std::string sAskMsg = msgResp.EncodeResp();
        m_pDlg->m_IOOpt.SendMsgToUser(sock,sAskMsg);
        //usleep(200*1000);
//		//shutdown(sock,SD_BOTH);
//		//closesocket(sock);
//        sock->closeServer();
//        delete sock;
        CloseConn(msg.m_strSource, sock);
        m_pDlg->m_Log.Log("ERROR_LOG->Login Fail, Cause %s",msgResp.m_strUserData.c_str());


    }
    //m_pDlg->SetUnLock();





    return TRUE;
}

BOOL  CHandSoftPhoneThread::HandleAgentLogOut(const std::string &strMsg,int sock)
{
    CLogoutEvt msg;
    msg.ParaSoftPhoneRequestMsg(strMsg);

    m_pDlg->m_Log.Log("FLOW_LOG->Station = %s",msg.m_strStation.c_str());
    m_pDlg->m_Log.Log("FLOW_LOG->UserData = %s",msg.m_strUserData.c_str());
    m_pDlg->m_Log.Log("FLOW_LOG->SOCKET = %d", sock);

    //m_pDlg->SetLock();
    CAgent* pAgent = m_pDlg->GetAgentFromStation(msg.m_strStation);
    if(pAgent)
    {
        pAgent->lock();
        pAgent->sAction = "Logout";
        pAgent->unlock();
        if(pAgent->AgentState != AS_Notready && pAgent->AgentState != AS_Wrap)
        {
            m_pDlg->m_Log.Log("ERROR_LOG->工号%s非正常签出 签出前状态为%d",pAgent->sWorkNo.c_str(), pAgent->AgentState);
        }
        pAgent->LogOut();
        pAgent->Release();
    }
    //m_pDlg->SetUnLock();
    return TRUE;
}

BOOL  CHandSoftPhoneThread::HandleAgentIdle(const std::string &strMsg,int s)
{
    CIdleEvt evt;
    evt.ParaSoftPhoneRequestMsg(strMsg);

    m_pDlg->m_Log.Log("FLOW_LOG->Station %s Request Idle ",evt.m_strStation.c_str());

    //m_pDlg->SetLock();
    CAgent* pAgent = m_pDlg->GetAgentFromStation(evt.m_strStation);
    if(pAgent)
    {
        pAgent->lock();
        pAgent->sAction = "SetIdle";
        pAgent->unlock();
        pAgent->SetIdle();
        pAgent->Release();
    }
    //m_pDlg->SetUnLock();

    return TRUE;
}

BOOL  CHandSoftPhoneThread::HandleAgentBusy(const std::string &strMsg,int s)
{
    CBusyEvt evt;
    evt.ParaSoftPhoneRequestMsg(strMsg);

    m_pDlg->m_Log.Log("FLOW_LOG->Station %s Request busy",evt.m_strStation.c_str());

    //m_pDlg->SetLock();
    CAgent* pAgent = m_pDlg->GetAgentFromStation(evt.m_strStation);
    if(pAgent)
    {
        pAgent->lock();
        pAgent->sAction = "SetBusy";
        pAgent->sReasonCode = evt.m_strReason;
        pAgent->unlock();
        pAgent->SetBusy();
        pAgent->Release();
    }
    //m_pDlg->SetUnLock();
    return TRUE;
}

BOOL  CHandSoftPhoneThread::HandleAgentWrapup(const std::string &strMsg,int s)
{
    CWrapupEvt evt;
    evt.ParaSoftPhoneRequestMsg(strMsg);

    m_pDlg->m_Log.Log("FLOW_LOG->Station %s Request Wrapup",evt.m_strStation.c_str());

    //m_pDlg->SetLock();
    CAgent* pAgent = m_pDlg->GetAgentFromStation(evt.m_strStation);
    if(pAgent)
    {
        pAgent->lock();
        pAgent->sAction = "SetWrapup";
        pAgent->unlock();
        pAgent->SetWrapup();
        pAgent->Release();
    }
    //m_pDlg->SetUnLock();
    return TRUE;
}

BOOL  CHandSoftPhoneThread::HandleAgentCompleteWrapup(const std::string &strMsg,int s)
{
    return TRUE;
}
BOOL  CHandSoftPhoneThread::HandleAgentMakeCall(const std::string &strMsg,int s)
{
    CMakeCallEvt evt;
    evt.ParaSoftPhoneRequestMsg(strMsg);
    m_pDlg->m_Log.Log("FLOW_LOG->Station %s Request MakeCall;Dnis%s",evt.m_strStation.c_str(), evt.m_strDnis.c_str());
    if (evt.m_strDnis == "")
    {
        return FALSE;
    }
    //m_pDlg->m_Log.Log(FLOW_LOG,"Station %s Request MakeCall",evt.m_strStation);
    //m_pDlg->SetLock();
    CAgent* pAgent = m_pDlg->GetAgentFromStation(evt.m_strStation);
    if(pAgent)
    {
        pAgent->lock();
        pAgent->sAction = "MakeCall";
        pAgent->sDnis = evt.m_strDnis;
        pAgent->unlock();
        pAgent->MakeCall();
        pAgent->Release();
    }
    //m_pDlg->SetUnLock();

    return TRUE;
}

BOOL  CHandSoftPhoneThread::HandleAgentConsult(const std::string &strMsg,int s)
{
    CConsultEvt evt;
    evt.ParaSoftPhoneRequestMsg(strMsg);

    m_pDlg->m_Log.Log("FLOW_LOG->Station %s Request Consult", evt.m_strStation.c_str());

    //m_pDlg->SetLock();
    CAgent* pAgent = m_pDlg->GetAgentFromStation(evt.m_strStation);
    if(pAgent)
    {
        pAgent->lock();
        pAgent->sAction = "Consult";
        pAgent->sConsultNo = evt.m_strDnis;
        pAgent->unlock();
        pAgent->Consult();
        pAgent->Release();
    }
    //m_pDlg->SetUnLock();

    return TRUE;
}

BOOL  CHandSoftPhoneThread::HandleAgentConsultTrans(const std::string &strMsg,int s)
{
    CConsultTransEvt evt;
    evt.ParaSoftPhoneRequestMsg(strMsg);

    m_pDlg->m_Log.Log("FLOW_LOG->Station %s Request ConsultTrans",evt.m_strStation.c_str());

    //m_pDlg->SetLock();
    CAgent* pAgent = m_pDlg->GetAgentFromStation(evt.m_strStation);
    if(pAgent)
    {
        pAgent->lock();
        pAgent->sAction = "ConsultTrans";
        pAgent->unlock();
        pAgent->ConsultTrans();

        /************************************************************************/
        /*                                                                      */
        //搞个转接的假状态
        pAgent->lock();
        pAgent->sAgentStatus = AS_Transfer;
        pAgent->unlock();
        m_pDlg->m_HandleAskPBX.SetAgentStatusReportData(pAgent);
        pAgent->Release();
        /************************************************************************/

    }
    //m_pDlg->SetUnLock();

    return TRUE;
}

BOOL  CHandSoftPhoneThread::HandleAgentConsultCancel(const std::string &strMsg,int s)
{
    CConsultCancelEvt evt;
    evt.ParaSoftPhoneRequestMsg(strMsg);

    m_pDlg->m_Log.Log("FLOW_LOG->Station %s Request ConsultCancel",evt.m_strStation.c_str());

    //m_pDlg->SetLock();
    CAgent* pAgent = m_pDlg->GetAgentFromStation(evt.m_strStation);
    if(pAgent)
    {
        pAgent->lock();
        pAgent->sAction = "ConsultCancel";
        pAgent->unlock();
        pAgent->ConsultCancel();
        pAgent->Release();
    }
    //m_pDlg->SetUnLock();

    return TRUE;
}

BOOL  CHandSoftPhoneThread::HandleAgentConsultConference(const std::string &strMsg,int s)
{
    CConfEvt evt;
    evt.ParaSoftPhoneRequestMsg(strMsg);

    m_pDlg->m_Log.Log("FLOW_LOG->Station %s Request Conference",evt.m_strStation.c_str());

    //m_pDlg->SetLock();
    CAgent* pAgent = m_pDlg->GetAgentFromStation(evt.m_strStation);
    if(pAgent)
    {
        pAgent->lock();
        pAgent->sAction = "Conference";
        pAgent->unlock();
        pAgent->Conference();
        pAgent->Release();
    }
    //m_pDlg->SetUnLock();
    return TRUE;
}

BOOL  CHandSoftPhoneThread::HandleAgentSingleTrans(const std::string &strMsg,int s)
{
    CSingleTransEvt evt;
    evt.ParaSoftPhoneRequestMsg(strMsg);

    m_pDlg->m_Log.Log("FLOW_LOG->Station %s Request SingleTrans",evt.m_strStation.c_str());

    //m_pDlg->SetLock();
    CAgent* pAgent = m_pDlg->GetAgentFromStation(evt.m_strStation);
    CAgent* pTransAgent = m_pDlg->GetAgentFromStation(pAgent->sDnis);


    if(pAgent)
    {
        pAgent->lock();
        pAgent->sAction = "SingleTrans";
        pAgent->sTransNo = evt.m_strTransNo;
        pAgent->unlock();
        pAgent->SingleTrans();
        /************************************************************************/
        /*                                                                      */
        //搞个转接的假状态
        pAgent->lock();
        pAgent->sAgentStatus = AS_Transfer;
        pAgent->unlock();
        m_pDlg->m_HandleAskPBX.SetAgentStatusReportData(pAgent);
        /************************************************************************/
        pAgent->Hangup();
        pAgent->Release();
    }

    if(pTransAgent)
    {
        pTransAgent->lock();
        pTransAgent->sAction = "SingleTrans";
        pTransAgent->sDnis = evt.m_strTransNo;
        pTransAgent->unlock();
        pTransAgent->Release();
    }
    //m_pDlg->SetUnLock();

    return TRUE;
}

BOOL  CHandSoftPhoneThread::HandleAgentSingleConference(const std::string &strMsg,int s)
{
    return TRUE;
}

BOOL  CHandSoftPhoneThread::HandleAgentAnswer(const std::string &strMsg,int s)
{
    return TRUE;
}

BOOL  CHandSoftPhoneThread::HandleAgentHold(const std::string &strMsg,int s)
{
    CHoldEvt evt;
    evt.ParaSoftPhoneRequestMsg(strMsg);

    m_pDlg->m_Log.Log("FLOW_LOG->Station %s Requset Hold",evt.m_strStation.c_str());

    //m_pDlg->SetLock();
    CAgent* pAgent = m_pDlg->GetAgentFromStation(evt.m_strStation);
    if(pAgent)
    {
        pAgent->lock();
        pAgent->sAction = "Hold";
        pAgent->unlock();
        pAgent->Hold();
        pAgent->Release();
    }
    //m_pDlg->SetUnLock();
    return TRUE;
}

BOOL  CHandSoftPhoneThread::HandleAgentUnHold(const std::string &strMsg,int s)
{
    CUnHoldEvt evt;
    evt.ParaSoftPhoneRequestMsg(strMsg);

    m_pDlg->m_Log.Log("FLOW_LOG->Station %s Requset UnHold",evt.m_strStation.c_str());

    //m_pDlg->SetLock();
    CAgent* pAgent = m_pDlg->GetAgentFromStation(evt.m_strStation);
    if(pAgent)
    {
        pAgent->lock();
        pAgent->sAction = "UnHold";
        pAgent->unlock();
        pAgent->UnHold();
        pAgent->Release();
    }
    //m_pDlg->SetUnLock();

    return TRUE;
}

BOOL  CHandSoftPhoneThread::HandleAgentHangup(const std::string &strMsg,int s)
{
    CHangupEvt evt;
    evt.ParaSoftPhoneRequestMsg(strMsg);

    m_pDlg->m_Log.Log("FLOW_LOG->Station %s Request Hangup",evt.m_strStation.c_str());

    //m_pDlg->SetLock();
    CAgent* pAgent = m_pDlg->GetAgentFromStation(evt.m_strStation);
    if(pAgent)
    {
        pAgent->lock();
        pAgent->sAction = "Hangup";
        pAgent->sReasonCode = evt.m_strReason;
        pAgent->unlock();
        pAgent->Hangup();
        pAgent->Release();
    }
    //m_pDlg->SetUnLock();

    return TRUE;
}
BOOL  CHandSoftPhoneThread::HandleAgentEnforceIdle(const std::string &strMsg,int s)
{
    return TRUE;
}

BOOL  CHandSoftPhoneThread::HandleAgentEnforceBusy(const std::string &strMsg,int s)
{
    return TRUE;
}

BOOL  CHandSoftPhoneThread::HandleAgentEnforceLogOut(const std::string &strMsg,int s)
{
    CEnforceLogOutEvt evt;
    evt.ParaSoftPhoneRequestMsg(strMsg);

    if (evt.m_strStation == evt.m_strLogoutStation)
    {
        return FALSE;
    }

    m_pDlg->m_Log.Log("FLOW_LOG->强制签出坐席%s : %s",evt.m_strLogoutAgentId.c_str(), evt.m_strLogoutStation.c_str());

    //m_pDlg->SetLock();
    CAgent* pAgent = m_pDlg->GetAgentFromStation(evt.m_strStation);
    if(pAgent)
    {
        pAgent->lock();
        pAgent->sAction = "EnforceLogOut";
        pAgent->unlock();
        pAgent->EnforceLogOut(evt.m_strLogoutStation,evt.m_strLogoutAgentId);
        pAgent->Release();
    }
    //m_pDlg->SetUnLock();

    return TRUE;
}

BOOL  CHandSoftPhoneThread::HandleAgentObserverCall(const std::string &strMsg,int s)
{
    BOOL bRet = FALSE;
    CObserverEvt evt;
    evt.ParaSoftPhoneRequestMsg(strMsg);

    m_pDlg->m_Log.Log("FLOW_LOG->Station %s Request Observer %s",evt.m_strStation.c_str(), evt.m_strObserverNo.c_str());

    //m_pDlg->SetLock();
    CAgent* pAgent = m_pDlg->GetAgentFromStation(evt.m_strStation);
    if(pAgent)
    {
        pAgent->lock();
        bRet = TRUE;
        pAgent->sAction = "Observer";
        pAgent->sObserverNo = evt.m_strObserverNo;
        pAgent->unlock();
        pAgent->Observer();
        pAgent->Release();
    }
    //m_pDlg->SetUnLock();

    return bRet;

}

BOOL  CHandSoftPhoneThread::HandleAgentEnforceHangup(const std::string &strMsg,int s)
{
    return TRUE;
}

BOOL CHandSoftPhoneThread::HandleAgentMonitor(const std::string &strMsg,int s)
{
    BOOL bRet = FALSE;
    CMonitorEvt evt;
    evt.ParaSoftPhoneRequestMsg(strMsg);

    m_pDlg->m_Log.Log("FLOW_LOG->Station %s Request Monitor",evt.m_strStation.c_str());

    //m_pDlg->SetLock();
    CAgent* pAgent = m_pDlg->GetAgentFromStation(evt.m_strStation);
    if(pAgent)
    {
        pAgent->lock();
        pAgent->sMointorNo = evt.m_strMonitorNo;
        pAgent->sAction = "Monitor";
        pAgent->unlock();
        pAgent->Monitor();
        pAgent->Release();

        bRet = TRUE;
    }
    //m_pDlg->SetUnLock();

    return bRet;
}

BOOL CHandSoftPhoneThread::HandleAgentHoldupCall(const std::string &strMsg,int s)
{
    BOOL bRet = FALSE;
    CHoldupCallEvt evt;
    evt.ParaSoftPhoneRequestMsg(strMsg);

    m_pDlg->m_Log.Log("FLOW_LOG->Station %s Request Holdup",evt.m_strStation.c_str());

    //m_pDlg->SetLock();
    CAgent* pAgent = m_pDlg->GetAgentFromStation(evt.m_strStation);
    if(pAgent)
    {
        pAgent->lock();
        bRet = TRUE;
        pAgent->sHoldupCallNo = evt.m_strHoldupNo;
        pAgent->sAction = "HoldupCall";
        pAgent->unlock();
        pAgent->HoldupCall();
        pAgent->Release();
    }
    //m_pDlg->SetUnLock();
    return bRet;
}

BOOL  CHandSoftPhoneThread::HandleAgentSendDTMF(const std::string &strMsg,int s)
{
    CSendDtmfEvt evt;
    evt.ParaSoftPhoneRequestMsg(strMsg);

    m_pDlg->m_Log.Log("FLOW_LOG->HandleAgentSendDTMF %s ",evt.m_strStation.c_str());
    //m_pDlg->SetLock();
    CAgent* pAgent = m_pDlg->GetAgentFromStation(evt.m_strStation);

    if(pAgent)
    {
        pAgent->lock();
        pAgent->sAction = "SendDTMF";
        pAgent->unlock();
        pAgent->SendDTMF(evt.digits);
        pAgent->Release();
    }
    //m_pDlg->SetUnLock();

    return TRUE;
}

BOOL CHandSoftPhoneThread::HandleGetPopData(const std::string &strMsg,int sock)
{
    BOOL bRet = FALSE;
    CGetPopDataEvt evt;
    evt.ParaSoftPhoneRequestMsg(strMsg);
    m_pDlg->m_Log.Log("FLOW_LOG->Station %s Request GetPopData",evt.m_strStation.c_str());

    //m_pDlg->SetLock();
    CAgent* pAgent = m_pDlg->GetAgentFromStation(evt.m_strStation);

    if (pAgent)
    {
        //无条件从坐席链表中取一条数据
        //BOOL bCheck = TRUE;
        int ncount = 0;
        while(TRUE)
        {
            evt.m_strUserData = pAgent->GetPopData();
            evt.m_strAgentId = pAgent->sWorkNo;
            evt.m_strStation = pAgent->sStation;
            //如果对应坐席的随路数据不为空则返回数据
            if (evt.m_strUserData !="")
            {
                evt.m_strRet = "Succ";
                m_pDlg->m_Log.Log("FLOW_LOG->成功获取分机%s随路数据：%s",evt.m_strStation.c_str(), evt.m_strUserData.c_str());
                std::string askMsg = evt.EnSoftPhoneMsg();
                m_pDlg->m_IOOpt.SendMsgToUser(sock,askMsg);
                m_pDlg->m_Log.Log("FLOW_LOG->返回给坐席的随路数据:%s",askMsg.c_str());

                ncount++;
            }
            else if(ncount == 0)//一条数据都没有
            {
                evt.m_strRet = "Fail";
                evt.m_strUserData = "数据为空";
                m_pDlg->m_Log.Log("FLOW_LOG->数据为空");
                std::string askMsg = evt.EnSoftPhoneMsg();
                m_pDlg->m_IOOpt.SendMsgToUser(sock,askMsg);
                m_pDlg->m_Log.Log("FLOW_LOG->返回给坐席的随路数据:%s",askMsg.c_str());
                break;
            }
            else
            {
                break;
            }
        }
        bRet = TRUE;

    }
    else
    {
        //返回失败，原因可能是没有该对象的实例
        evt.m_strRet = "Fail";
        //evt.m_strUserData.Format("获取分机%s的随路数据失败",evt.m_strStation);
        evt.m_strUserData = "获取分机" + evt.m_strStation + "的随路数据失败";

        std::string askMsg = evt.EnSoftPhoneMsg();

        m_pDlg->m_IOOpt.SendMsgToUser(sock,askMsg);

        m_pDlg->m_Log.Log("FLOW_LOG->获取分机%s的随路数据失败",evt.m_strStation.c_str());

        bRet = FALSE;
    }
    //m_pDlg->SetUnLock();

    if(pAgent)
        pAgent->Release();

    return bRet;
}

void CHandSoftPhoneThread::CloseConn(const std::string &strMsg, int s)
{
    if(strMsg != "S_WEB" && strMsg != "C_WEB")
    {
        usleep(200*1000);
        GetApp()->m_IOOpt.m_conn.closeConn(s);
    }
}

BOOL CHandSoftPhoneThread::HandleUpdateSocket(int s)
{
    m_webSock = s;
    m_pDlg->m_MapWorkNoToStationLock.acquire();

    int old = -1;

    std::map<std::string, std::string>::iterator pos = m_pDlg->m_MapWorkNoToStation.begin();
    while(pos != m_pDlg->m_MapWorkNoToStation.end())
    {
        std::string sStation = pos->second;
        ++pos;
        CAgent *pAgent = m_pDlg->GetAgentFromStation(sStation);
        if(pAgent && (pAgent->m_strSource == "C_WEB" || pAgent->m_strSource == "S_WEB"))
        {
            old = pAgent->sock;
            pAgent->sock = s;
        }
        pAgent->Release();
    }

    if(old != s && old != -1)
    {
        GetApp()->m_IOOpt.m_conn.closeConn(old);
    }

    m_pDlg->m_MapWorkNoToStationLock.release();

    return TRUE;
}


//
BOOL CHandSoftPhoneThread::StartDispatchSoftPhoneMsg(CAskProxyDlg* pDlg)
{

    m_pDlg = pDlg;	
    ACE_Thread_Manager::instance()->spawn(CHandSoftPhoneThread::HandleDispatchSoftPhoneMsg,
                                          this,
                                          (THR_NEW_LWP|THR_JOINABLE|THR_INHERIT_SCHED|THR_SUSPENDED),
                                          &m_pDispatchSoftPhoneThread
                                         );

    if(m_pDispatchSoftPhoneThread!=0)
    {
        m_pDlg->m_Log.Log("TEST_LOG->thread handlesoftphonemsg succ");
    }
    else
    {
        m_pDlg->m_Log.Log("TEST_LOG->thread handlesoftphonemsg fail");
	return 0;
    }


    ACE_Thread_Manager::instance()->resume(m_pHandleSoftPhoneThread);

	return 1;
}

LPVOID CHandSoftPhoneThread::HandleDispatchSoftPhoneMsg(LPVOID lpvoid)
{
	CHandSoftPhoneThread* pThis = reinterpret_cast<CHandSoftPhoneThread*>(lpvoid);
	while(true)
	{
		//pThis->m_hStartDispatchSoftPhoneMsg.acquire();
		
        	if(pThis->m_hStopDispatchSoftPhoneMsg)
        	{
            		pThis->m_pDlg->m_Log.Log("FLOW_LOG->DispatchSoftPhoneMsg Thread Exit");
            		break;
        	}
        	else
        	{
			pThis->RunDispatchSoftPhoeMsg();
       		 }
		
    	}	
	return 0;
}

void  CHandSoftPhoneThread::RunDispatchSoftPhoeMsg()
{
	std::cout<<"Run Dispatch"<<std::endl;
	std::map<std::string,CPhone*>::iterator it ;
	//m_phoneMapCritical.acquire();
	for(it = m_phoneMap.begin(); it!=m_phoneMap.end();it++)
	{
		std::cout<<"111"<<std::endl;
		CPhone* pPhone = (*it).second;
		if(pPhone->status == "0")
		{
			std::cout<<"2222"<<std::endl;
			pPhone->status = "1"; //1
			if(pPhone->cmd == "VoiceCampaign")
			{
				std::cout<<"makecall"<<std::endl;
	        		pPhone->MakeCall();	
			}
		}
		
	}
	//m_phoneMapCritical.release();
	std::cout<<"Exit RunDispatch"<<std::endl;
}

BOOL  CHandSoftPhoneThread::StopDispatchSoftPhoneMsg()
{

    m_hStopDispatchSoftPhoneMsg= true;
    m_hStartDispatchSoftPhoneMsg.release();
    ACE_Thread_Manager::instance()->join(m_pDispatchSoftPhoneThread);
    m_hStopDispatchSoftPhoneMsg = false;
    m_pDispatchSoftPhoneThread = 0;
    return 1;
}
