#include "Agent.h"
#include "AskProxyDlg.h"
#include "UntiTool.h"
#include "GenCMD.h"
#include "./logclass/log.h"

class CAgentLock
{
public:
    static CLog& GetInstance()
    {
        static CLog *l = new CLog("AgentLockLog");
        return *l;
    }
};

CAgent::CAgent()
    :sock(-1)
{
    sSendResult_Data = "";
    reset_agent();
    __sync_lock_release(&m_cRef);//原子操作
}

CAgent::~CAgent()
{
    //std::cout << "Agent destory" << std::endl;
    if(!sStation.empty())
        CAgentLock::GetInstance().Log("Agent destory %s", sStation.c_str());

    std::list<liPopdata*>::iterator it;
    if (!m_listPopData.empty())
	{
		it = m_listPopData.begin();
		delete *it;
		m_listPopData.erase(it);
	}

    if(sock && m_strSource != "C_WEB" && m_strSource != "S_WEB")
    {
        GetApp()->m_IOOpt.m_conn.closeConn(sock);
        sock = -1;
    }
}

void CAgent::reset_agent()
{
    preLiveTimeSecond = time(NULL);
    sWorkNo = "";
	sStation = "";
	sPwd = "";

	AgentState = AS_Notlogin;
	sSipInterface = "";


	sContext = "default";

	sChan = "";
	sOtherID = "";
	sHoldID = "";

    m_tempSkill = "";

	sUnique = "";
	sUserData = "";
	sHangupReason = "";
	sUCID = "";

	bLogin = FALSE;

	sock = 0;
	sAgentStatus = "";
	m_pMainSock = NULL;
	m_strAgentStatusReport = "";

	bIntoEstablish = FALSE;
	bConsult = FALSE;
	orginDnis = "";
	orginAni = "";

	nLogoutTimeOutNum = 0;
}

void CAgent::reset_phone()
{
	sAni = "";
	sDnis = "";
	sSelfChanID = "";
	sOtherID = "";
	sHoldID = "";
	sUnique = "";//
	sUserData = "";
	sActionID = "";
	sAction = "";
	sUCID = "";
	m_strAgentCDRReport = m_pDlg->m_strCdrWeb;
	sReasonCode = "";

//基于手机客户端需求
	m_callID = "";	//通话标识
	m_recordID = "";	//录音标识
	m_accessNumber = "";//接入码
	m_ivrTrack = "";		//ivr号，等同接入码
	m_callRingTime = "";
	m_callStartTime = "";
	m_callClearTime = "";
	sQueueName = "";
}

BOOL CAgent::SendCMDToPBX(const std::string &sendCmd)
{
	preLiveTime.update();
    preLiveTimeSecond = time(NULL);

	if(sendCmd.length()<0)
		return FALSE;

	m_pDlg->m_AgentLog.Log(sendCmd);
//	std::cout << "send CMD: " << sendCmd << std::endl;

	size_t ret = 0;
	if(m_pMainSock)
	{
		ret = m_pMainSock->WriteComm(sendCmd, sendCmd.length());
	}
	if(ret == 0)
		return FALSE;
	return TRUE;
}

void CAgent::SetMainWnd(CAskProxyDlg* pDlg)
{
	m_pDlg = pDlg;
}

BOOL CAgent::Login()
{
    std::vector<std::string> array ;
	CUntiTool tool;
	BOOL bRet;
	//tool.DecodeString(sQueueName,array,',');
	tool.DecodeString(m_tempSkill,array,',');

	m_mapSkillT.clear();


	for(size_t i=0; i<array.size(); i++)
	{
		m_mapSkillT[array[i]] = "A";
		m_pDlg->m_Log.Log("工号%s 技能 QueueName = %s\r\n",this->sWorkNo.c_str(), (array[i]).c_str());
	}

//	POSITION pos = m_mapSkillT.GetStartPosition();
    std::map<std::string, std::string>::iterator pos = m_mapSkillT.begin();
    //auto pos = m_mapSkillT.begin();

	while(pos!=m_mapSkillT.end())
	{
		std::string sQueue = pos->first;
//		std::string sVal;
//		m_mapSkillT.GetNextAssoc(pos,sQueue,sVal);
		CGenCMD cmd;

		cmd.Start();
		cmd.Gen("Action","QueueAdd");
		cmd.Gen("Queue",sQueue);
		cmd.Gen("Interface",sSipInterface);
		cmd.Gen("Penalty",m_mapSkillAndPenalty[sQueue]);
		cmd.Gen("Paused","true");
		cmd.Gen("Async","1");

		std::string strTmp = "Login|" + sStation + "|QueueAdd|" + sQueue;
//		strTmp.Format("Login|%s|QueueAdd|%s",sStation,sQueue);
		cmd.End(strTmp);

		std::string sendCmd = cmd.GetCmd();

		bRet = SendCMDToPBX(sendCmd);
		if (!bRet)
		{
			return FALSE;
		}
		++pos;
	}

	return TRUE;
}

BOOL CAgent::LoginTalkData()
{
	CGenCMD cmd;
	cmd.Start();
	cmd.Gen("Action","Setvar");//Action: Setvar
	std::string sVal(sStation);
	sVal += "@pstn";
//	sVal.Format("%s@pstn",sStation);
	cmd.Gen("Variable",sVal);//Variable: pstn号码@pstn

	std::string sNum(sWorkNo);
    sNum += ",";
    sNum += sTeantid;
    sNum += ",0,";
    sNum += m_strpstnAgentPrefix;
//	sNum.Format("%s,%s,0,%s",sWorkNo,sTeantid,m_strpstnAgentPrefix);

	cmd.Gen("Value",sNum);//value: %s,%s,0,
	std::string strTmp("SetGloabvar");
	cmd.End(strTmp);
	std::string sendCmd = cmd.GetCmd();
	BOOL bRet = SendCMDToPBX(sendCmd);
	return bRet;
}

BOOL CAgent::LogoutTalkData()
{
	CGenCMD cmd;
	cmd.Start();
	cmd.Gen("Action","Setvar");//Action: Setvar
	std::string sVal(sStation);
	sVal += "@pstn";
//	sVal.Format("%s@pstn",sStation);
	cmd.Gen("Variable",sVal);//Variable: pstn号码@pstn
	cmd.Gen("Value","");//置空
	std::string strTmp("SetGloabvar");
	cmd.End(strTmp);
	std::string sendCmd = cmd.GetCmd();
	BOOL bRet = SendCMDToPBX(sendCmd);
	return bRet;
}

BOOL CAgent::LoginSipTalkData()//sip座席login设置全局变量
{
    //命令格式：ActionID: SetGloabvar|\r\nAction: Setvar\r\nVariable: %s@sip\r\nValue: %s,%s,0,\r\n\r\n\r\n
	//ActionID: SetGloabvar|
	//Action: Setvar
	//Variable: %s@sip
	//Value: %s,%s,0,
	//
	//
	//
	CGenCMD cmd;
	cmd.Start();
	cmd.Gen("Action","Setvar");//Action: Setvar
	std::string sVal(sStation);
	sVal += "@sip";
//	sVal.Format("%s@sip",sStation);
	cmd.Gen("Variable",sVal);//Variable: pstn号码@pstn
	std::string sNum(sWorkNo);
	sNum += ",";
	sNum += sTeantid;
	sNum += ",0,";

//	sNum.Format("%s,%s,0,",sWorkNo,sTeantid);

	cmd.Gen("Value",sNum);//value: %s,%s,0
	std::string strTmp("SetGloabvar");
	cmd.End(strTmp);
	std::string sendCmd = cmd.GetCmd();
	BOOL bRet = SendCMDToPBX(sendCmd);
	return bRet;
}

BOOL CAgent::LogoutSipTalkData()//sip座席logout置空全局变量
{
	CGenCMD cmd;
	cmd.Start();
	cmd.Gen("Action","Setvar");//Action: Setvar

	std::string sVal(sStation);
	sVal += "@sip";
//	sVal.Format("%s@sip",sStation);
	cmd.Gen("Variable",sVal);//Variable: pstn号码@pstn
	cmd.Gen("Value","");//置空
	std::string strTmp("SetGloabvar");
	cmd.End(strTmp);
	std::string sendCmd = cmd.GetCmd();
	BOOL bRet = SendCMDToPBX(sendCmd);
	return bRet;
}

BOOL CAgent::LogOut()
{
	if (m_sStationType=="PSTN")
	{//pstn: 手机或者座机签入
	    LogoutTalkData();
	}
	else
	{//sip
		LogoutSipTalkData();
	}

	BOOL bRet;
	std::map<std::string, std::string>::iterator pos = m_mapSkillT.begin();//m_sSkillList.GetHeadPosition();
	//auto pos = m_mapSkillT.begin();
	while(pos!=m_mapSkillT.end())
	{
		std::string strQueue = pos->first;
//		std::string strVal;
//		m_mapSkillT.GetNextAssoc(pos,strQueue,strVal);

		CGenCMD cmd;

		cmd.Start();
		cmd.Gen("Action","QueueRemove");
		cmd.Gen("Queue",strQueue);
		cmd.Gen("Interface",sSipInterface);
		cmd.Gen("Async","1");

		std::string strTmp("Logout|");
		strTmp += sStation;
		strTmp += "|QueueRemove|";
		strTmp += strQueue;
//		strTmp.Format("Logout|%s|QueueRemove|%s",sStation,strQueue);
		cmd.End(strTmp);

		std::string sendCmd = cmd.GetCmd();
		bRet = SendCMDToPBX(sendCmd);
		if (!bRet)
		{
			return FALSE;
		}
        ++pos;
	}


	return TRUE;
}

BOOL CAgent::SetIdle()
{
	CGenCMD cmd;
	cmd.Start();
	cmd.Gen("Action","QueuePause");
	cmd.Gen("Interface",sSipInterface);
	cmd.Gen("Paused","false");
	cmd.Gen("Async","1");
	std::string sTmp = "SetIdle|" + sStation + "|QueuePause";
//	sTmp.Format("SetIdle|%s|QueuePause",sStation);
	cmd.End(sTmp);

	std::string sendCmd = cmd.GetCmd();
	BOOL bRet = SendCMDToPBX(sendCmd);

	return bRet;
}

BOOL CAgent::SetBusy()
{
	CGenCMD cmd;
	cmd.Start();
	cmd.Gen("Action","QueuePause");
	cmd.Gen("Interface",sSipInterface);
	cmd.Gen("Paused","true");
	cmd.Gen("Async","1");
	std::string sTmp = "SetBusy|" + sStation + "|QueuePause";
//	sTmp.Format("SetBusy|%s|QueuePause",sStation);
	cmd.End(sTmp);

	std::string sendCmd = cmd.GetCmd();
	BOOL bRet = SendCMDToPBX(sendCmd);

	return bRet;
}

BOOL CAgent::SetWrapup()
{
	sAction = "SetWrapup";
	CGenCMD cmd;
	cmd.Start();
	cmd.Gen("Action","QueuePause");
	cmd.Gen("Interface",sSipInterface);
	cmd.Gen("Paused","true");
	cmd.Gen("Async","1");
	std::string sTmp = "SetWrapup|" + sStation + "|QueuePause";
//	sTmp.Format("SetWrapup|%s|QueuePause",sStation);
	cmd.End(sTmp);

	std::string sendCmd = cmd.GetCmd();
	BOOL bRet = SendCMDToPBX(sendCmd);

	return bRet;
}

BOOL CAgent::MakeCall()
{
	std::string sPBX = m_pDlg->m_SettingData.m_strPBX;
	std::string sPort = m_pDlg->m_SettingData.m_strPbxPort;
	std::map<std::string,std::string>::iterator it;
	it = m_pDlg->m_SettingData.m_mapNoType.find(sDnis);
	std::string tDnis("");
	std::string val("");
	val="__my-caller="+sStation+",";
	val += "__my-called="+sDnis+",";
	if( it == m_pDlg->m_SettingData.m_mapNoType.end())
	{
		val += std::string("called-chan=")+std::string("SIP/")+sDnis+std::string("@")+m_pDlg->m_SettingData.m_strInPBX + ",";
		val += "__my-callDirection=out";
		
		//tDnis.Format("SIP/%s@%s",sDnis,m_pDlg->m_strInPBX);
	}	
	else
	{
		val += std::string("called-chan=")+std::string("SIP/")+sDnis+",";
		val += "__my-callDirection=internal";
	}
//	std::string val = "callerType" + sTeantid + ",calledType=" + sStation;
//	val.Format("__x-sinosoftcrm-tenantId=%s,my-id=%s",sTeantid,sStation);
	std::string sCaller = m_strConfigCaller + " <" + m_strConfigCaller + ">";
//	sCaller.Format("%s <%s>",m_strConfigCaller,m_strConfigCaller);
	CGenCMD cmd;
	cmd.Start();
	cmd.Gen("Action","Originate");
	cmd.Gen("Channel",sChan);
	cmd.Gen("Timeout",sTimeout);
	cmd.Gen("Callerid",sCaller/*sStation*/);
	cmd.Gen("Context","originate");
	cmd.Gen("Exten",sDnis);
	cmd.Gen("Variable",val);
	cmd.Gen("Priority","1");
	cmd.Gen("Async","1");

	std::string sTmp = "MakeCall|" + sStation + "|Originate";
//	sTmp.Format("MakeCall|%s|Originate",sStation);
	cmd.End(sTmp);

	std::string sendCmd = cmd.GetCmd();
	m_pDlg->m_Log.Log("发送给PBX的数据：%s",sendCmd.c_str());
	BOOL bRet = SendCMDToPBX(sendCmd);

	return bRet;
}

BOOL CAgent::Hangup()
{
	if(sHoldID == "")
	{
		CGenCMD cmd;
		cmd.Start();
		cmd.Gen("Action","Hangup");
		cmd.Gen("Channel",sSelfChanID);
	    cmd.Gen("Async","1");
		std::string strTmp = "Hangup|" + sStation + "|Hangup";
//		strTmp.Format("Hangup|%s|Hangup",sStation);
		cmd.End(strTmp);

		std::string sendCmd = cmd.GetCmd();
		BOOL bRet = SendCMDToPBX(sendCmd);
		m_pDlg->m_Log.Log("%s",sendCmd.c_str());

		return bRet;
	}
	else if(sHoldID != "")
	{
	   sAction = "ConsultCancel";
	   ConsultCancel();
	   return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CAgent::Hold()
{
	CGenCMD cmd1;
	cmd1.Start();
	cmd1.Gen("Action","Setvar");
	cmd1.Gen("Channel",sSelfChanID);
	cmd1.Gen("Variable","my-holdDisplay");
	cmd1.Gen("Value","1");
	std::string strTmp = "Hold|" + sStation + "|Redirect";
//	strTmp.Format("Hold|%s|Redirect",sStation);
	cmd1.End(strTmp);
	std::string sendCmd = cmd1.GetCmd();
	BOOL bRet = SendCMDToPBX(sendCmd);


	CGenCMD cmd2;
	cmd2.Start();
	cmd2.Gen("Action","Setvar");
	cmd2.Gen("Channel",sOtherID);
	cmd2.Gen("Variable","my-holdDisplay");
	cmd2.Gen("Value","1");
	cmd2.Gen("Async","1");
	strTmp = "Hold|" + sStation + "|Redirect";
//	strTmp.Format("Hold|%s|Redirect",sStation);
	cmd2.End(strTmp);
	sendCmd = cmd2.GetCmd();
	bRet = SendCMDToPBX(sendCmd);



	CGenCMD cmd;
	cmd.Start();
	cmd.Gen("Action","Redirect");
	cmd.Gen("Channel",sSelfChanID);
	cmd.Gen("ExtraChannel",sOtherID);
	cmd.Gen("Context","hold");
	cmd.Gen("Exten","s");
	cmd.Gen("Priority","1");
	cmd.Gen("Async","1");

    strTmp = "Hold|" + sStation + "|Redirect";
//	strTmp.Format("Hold|%s|Redirect",sStation);
	cmd.End(strTmp);
	sendCmd = cmd.GetCmd();
	bRet = SendCMDToPBX(sendCmd);
	sHoldID = sOtherID;

	return bRet;

}

BOOL CAgent::UnHold()
{

	CGenCMD cmd1;
	cmd1.Start();
	cmd1.Gen("Action","Setvar");
	cmd1.Gen("Channel",sSelfChanID);
	cmd1.Gen("Variable","my-bridgedChannel");
	cmd1.Gen("Value",sOtherID);
	cmd1.Gen("Async","1");


	std::string strTmp = "UnHoldSetVar|" + sStation;
//	strTmp.Format("UnHoldSetVar|%s",sStation);
	cmd1.End(strTmp);

	std::string sendCmd = cmd1.GetCmd();
	BOOL bRet = SendCMDToPBX(sendCmd);

	sHoldID = "";

	CGenCMD cmd2;
	cmd2.Start();
	cmd2.Gen("Action","Redirect");
	cmd2.Gen("Channel",sSelfChanID);
	cmd2.Gen("Context","bridge");
	cmd2.Gen("Exten","s");
	cmd2.Gen("Async","1");
	cmd2.Gen("Priority","1");

    strTmp = "UnHold|" + sStation + "|Redirect";
//	strTmp.Format("UnHold|%s|Redirect",sStation);
	cmd2.End(strTmp);

	sendCmd = cmd2.GetCmd();
	bRet = SendCMDToPBX(sendCmd);

	return bRet;
}

BOOL CAgent::SingleTrans()
{
	
	CGenCMD cmd1;
	std::string sVal;
	std::string strTmp;
	std::string sendCmd;

	cmd1.Start();
	cmd1.Gen("Action","Setvar");
	cmd1.Gen("Channel",sHoldID);
	cmd1.Gen("Variable","__my-caller");
	cmd1.Gen("Value",sHoldStation);
	cmd1.Gen("Async","1");
	strTmp = "SingleTrans|" + sStation + "|SetVal";
	cmd1.End(strTmp);
	sendCmd = cmd1.GetCmd();
    	SendCMDToPBX(sendCmd);

	
	cmd1.Start();
	cmd1.Gen("Action","Setvar");
	cmd1.Gen("Channel",sHoldID);
	cmd1.Gen("Variable","__my-called");
	cmd1.Gen("Value",sTransNo);
	cmd1.Gen("Async","1");
	strTmp = "SingleTrans|" + sStation + "|SetVal";
	cmd1.End(strTmp);
	sendCmd = cmd1.GetCmd();
    	SendCMDToPBX(sendCmd);




	std::string sPBX = m_pDlg->m_SettingData.m_strPBX;
	std::string sPort = m_pDlg->m_SettingData.m_strPbxPort;
	std::map<std::string,std::string>::iterator it;
	it = m_pDlg->m_SettingData.m_mapNoType.find(sDnis);
	std::string val("");
	if( it == m_pDlg->m_SettingData.m_mapNoType.end())
	{
		val = std::string("SIP/")+sTransNo+std::string("@")+m_pDlg->m_SettingData.m_strInPBX;
	}	
	else
	{
		val = std::string("SIP/")+sTransNo;
	}

	
	cmd1.Start();	
	cmd1.Gen("Action","Setvar");
	cmd1.Gen("Channel",sHoldID);
	cmd1.Gen("Variable","called-chan");
	cmd1.Gen("Value",val);
	cmd1.Gen("Async","1");
	strTmp = "SingleTrans|" + sStation + "|SetVal";
	cmd1.End(strTmp);
	sendCmd = cmd1.GetCmd();
	SendCMDToPBX(sendCmd);

	CGenCMD cmd;
	cmd.Start();
	cmd.Gen("Action","Redirect");
	cmd.Gen("Channel",sHoldID);
	cmd.Gen("Context","blind");
	cmd.Gen("Exten",sTransNo);
	cmd.Gen("Priority","1");
	cmd.Gen("Async","1");

    strTmp = "SingleTrans|" + sStation + "|Redirect";
//	strTmp.Format("SingleTrans|%s|Redirect",sStation);
	cmd.End(strTmp);

	sendCmd = cmd.GetCmd();

	sHoldID = "";

	SendCMDToPBX(sendCmd);
	

	return TRUE;
}

BOOL CAgent::Consult()
{
	
	CGenCMD cmd1;
	std::string sVal;
	std::string strTmp;

	cmd1.Start();
	cmd1.Gen("Action","Setvar");
	cmd1.Gen("Channel",sSelfChanID);
	cmd1.Gen("Variable","__my-caller");
	cmd1.Gen("Value",sStation);
	cmd1.Gen("Async","1");
	strTmp = "Consult|" + sStation + "|SetVal";
	cmd1.End(strTmp);
	std::string sendCmd = cmd1.GetCmd();
    	SendCMDToPBX(sendCmd);

	
	cmd1.Start();
	cmd1.Gen("Action","Setvar");
	cmd1.Gen("Channel",sSelfChanID);
	cmd1.Gen("Variable","__my-called");
	cmd1.Gen("Value",sConsultNo);
	cmd1.Gen("Async","1");
	strTmp = "Consult|" + sStation + "|SetVal";
	cmd1.End(strTmp);
	sendCmd = cmd1.GetCmd();
    	SendCMDToPBX(sendCmd);

	


	std::string sPBX = m_pDlg->m_SettingData.m_strPBX;
	std::string sPort = m_pDlg->m_SettingData.m_strPbxPort;
	std::map<std::string,std::string>::iterator it;
	it = m_pDlg->m_SettingData.m_mapNoType.find(sDnis);
	std::string val("");
	if( it == m_pDlg->m_SettingData.m_mapNoType.end())
	{
		val = std::string("SIP/")+sConsultNo+std::string("@")+m_pDlg->m_SettingData.m_strInPBX;
	}	
	else
	{
		val = std::string("SIP/")+sConsultNo;
	}

	
	cmd1.Start();	
	cmd1.Gen("Action","Setvar");
	cmd1.Gen("Channel",sSelfChanID);
	cmd1.Gen("Variable","called-chan");
	cmd1.Gen("Value",val);
	cmd1.Gen("Async","1");
	strTmp = "Consult|" + sStation + "|SetVal";
	cmd1.End(strTmp);
	sendCmd = cmd1.GetCmd();
    	SendCMDToPBX(sendCmd);

	
    	bConsult = TRUE;
	CGenCMD cmd;
	cmd.Start();
	cmd.Gen("Action","Redirect");
	cmd.Gen("Channel",sSelfChanID);
	cmd.Gen("Context","consult");
	cmd.Gen("Exten",sConsultNo);
	cmd.Gen("Priority","1");
	cmd.Gen("Async","1");
	strTmp = "Consult|" + sStation + "|Redirect";
//	strTmp.Format("Consult|%s|Redirect",sStation);
	cmd.End(strTmp);

	sendCmd = cmd.GetCmd();

//	BOOL bRet = SendCMDToPBX(sendCmd);
    SendCMDToPBX(sendCmd);

    return TRUE;
}

BOOL CAgent::ConsultCancel()
{
	CGenCMD cmd1;
	std::string sVal;
	std::string strTmp;

    sVal = "0|ConsultCancel|" + sStation;
//	sVal.Format("0|ConsultCancel|%s",sStation);
	cmd1.Start();
	cmd1.Gen("Action","Setvar");
	cmd1.Gen("Channel",sSelfChanID);
	cmd1.Gen("Variable","my-holdDisplay");
	cmd1.Gen("Value",sVal);
	cmd1.Gen("Async","1");

    strTmp = "Hold|" + sStation + "|Redirect";
//	strTmp.Format("Hold|%s|Redirect",sStation);
	cmd1.End(strTmp);
	std::string sendCmd = cmd1.GetCmd();
//	BOOL bRet = SendCMDToPBX(sendCmd);
    SendCMDToPBX(sendCmd);

	CGenCMD cmd;
	cmd.Start();
	cmd.Gen("Action","Redirect");
	cmd.Gen("Channel",sSelfChanID);
	cmd.Gen("Context","hold");
	cmd.Gen("Exten","s");
	cmd.Gen("Priority","1");
	cmd.Gen("Async","1");

    strTmp = "Hold|" + sStation + "|Redirect";
//	strTmp.Format("Hold|%s|Redirect",sStation);
	cmd.End(strTmp);
	sendCmd = cmd.GetCmd();
//	bRet = SendCMDToPBX(sendCmd);
    SendCMDToPBX(sendCmd);

	return TRUE;

}

BOOL CAgent::ConsultCancel_1()
{
	
	
	CGenCMD cmd1;
	std::string strTmp;
	std::string sendCmd;
	cmd1.Start();
	cmd1.Gen("Action","Setvar");
	cmd1.Gen("Channel",sSelfChanID);
	cmd1.Gen("Variable","my-bridgedChannel");
	cmd1.Gen("Value",sHoldID);
	cmd1.Gen("Async","1");

	strTmp = "ConsultCancel|" + sStation;
	cmd1.End(strTmp);
	sendCmd = cmd1.GetCmd();
        SendCMDToPBX(sendCmd);

	
	std::string sVal;

	std::cout<<"HoldID ani "<<sHoldS.sAni<<std::endl;
	std::cout<<"HoldID dnis "<<sHoldS.sDnis<<std::endl;

	cmd1.Start();
	cmd1.Gen("Action","Setvar");
	cmd1.Gen("Channel",sHoldID);
	cmd1.Gen("Variable","__my-caller");
	cmd1.Gen("Value",sHoldS.sAni);
	cmd1.Gen("Async","1");
	strTmp = "Consult|" + sStation + "|SetVal";
	cmd1.End(strTmp);
	sendCmd = cmd1.GetCmd();
    	SendCMDToPBX(sendCmd);



	cmd1.Start();
	cmd1.Gen("Action","Setvar");
	cmd1.Gen("Channel",sHoldID);
	cmd1.Gen("Variable","__my-called");
	cmd1.Gen("Value",sHoldS.sDnis);
	cmd1.Gen("Async","1");
	strTmp = "Consult|" + sStation + "|SetVal";
	cmd1.End(strTmp);
	sendCmd = cmd1.GetCmd();
    	SendCMDToPBX(sendCmd);


	cmd1.Start();
	cmd1.Gen("Action","Setvar");
	cmd1.Gen("Channel",sSelfChanID);
	cmd1.Gen("Variable","__my-caller");
	cmd1.Gen("Value",sHoldS.sAni);
	cmd1.Gen("Async","1");
	strTmp = "Consult|" + sStation + "|SetVal";
	cmd1.End(strTmp);
	sendCmd = cmd1.GetCmd();
    	SendCMDToPBX(sendCmd);


	cmd1.Start();
	cmd1.Gen("Action","Setvar");
	cmd1.Gen("Channel",sSelfChanID);
	cmd1.Gen("Variable","__my-called");
	cmd1.Gen("Value",sHoldS.sDnis);
	cmd1.Gen("Async","1");
	strTmp = "Consult|" + sStation + "|SetVal";
	cmd1.End(strTmp);
	sendCmd = cmd1.GetCmd();
    	SendCMDToPBX(sendCmd);



	cmd1.Start();
	cmd1.Gen("Action","Redirect");
	cmd1.Gen("Channel",sSelfChanID);
	cmd1.Gen("Context","bridge");
	cmd1.Gen("Exten","s");
	cmd1.Gen("Async","1");
	cmd1.Gen("Priority","1");

	strTmp = "ConsultCancel|" + sStation + "|Redirect";
	cmd1.End(strTmp);

	sendCmd = cmd1.GetCmd();
//	BOOL bRet2 = SendCMDToPBX(sendCmd);
    SendCMDToPBX(sendCmd);

	sOtherID = sHoldID;
	sHoldID = "";
	return 1;
}

BOOL CAgent::ConsultTrans()
{
	CGenCMD cmd;
	std::string strTmp;
	std::string sendCmd;

	cmd.Start();
	cmd.Gen("Action","Setvar");
	cmd.Gen("Channel",sHoldID);
	cmd.Gen("Variable","my-bridgedChannel");
	cmd.Gen("Value",sOtherID);
	cmd.Gen("Async","1");

    strTmp = "ConsultTrans|" + sStation;
//	strTmp.Format("ConsultTrans|%s",sStation);
	cmd.End(strTmp);
	sendCmd = cmd.GetCmd();
//	bRet = SendCMDToPBX(sendCmd);
    SendCMDToPBX(sendCmd);

	cmd.Start();
	cmd.Gen("Action","Setvar");
	cmd.Gen("Channel",sOtherID);
	cmd.Gen("Variable","my-holdDisplay");

    strTmp = "0|ConsultTrans|" + sStation;
//	strTmp.Format("0|ConsultTrans|%s",sStation);
	cmd.Gen("Value",strTmp);
	cmd.Gen("Async","1");

	m_pDlg->m_Log.Log("测试HOLD CHAN %s  Val %s",sHoldID.c_str(), strTmp.c_str());

	cmd.End(strTmp);
    sendCmd = cmd.GetCmd();
	SendCMDToPBX(sendCmd);

	cmd.Start();
	cmd.Gen("Action","Redirect");
	cmd.Gen("Channel",sOtherID);
	cmd.Gen("Context","hold");
	cmd.Gen("Exten","s");
	cmd.Gen("Priority","1");
	cmd.Gen("Async","1");

    strTmp = "ConsultTrans|" + sStation + "|Redirect";
//	strTmp.Format("ConsultTrans|%s|Redirect",sStation);
	cmd.End(strTmp);
	sendCmd = cmd.GetCmd();

//	bRet = SendCMDToPBX(sendCmd);
    SendCMDToPBX(sendCmd);

	return TRUE;
}

BOOL CAgent::ConsultTrans_1()
{
//	BOOL bRet = FALSE;
	CGenCMD cmd;
	std::string strTmp;
	std::string sendCmd;

	cmd.Start();
	cmd.Gen("Action","Redirect");
	cmd.Gen("Channel",sHoldID);
	cmd.Gen("Context","bridge");
	cmd.Gen("Exten","s");
	cmd.Gen("Async","1");
	cmd.Gen("Priority","1");

	strTmp = "ConsultTrans|" + sStation + "|Redirect";
//	strTmp.Format("ConsultTrans|%s|Redirect",sStation);
	cmd.End(strTmp);


	sendCmd = cmd.GetCmd();

	SendCMDToPBX(sendCmd);

	cmd.Start();
	cmd.Gen("Action","Hangup");
	cmd.Gen("Channel",sSelfChanID);
	cmd.Gen("Async","1");

	strTmp = "Hangup|" + sStation + "|Hangup";
//	strTmp.Format("Hangup|%s|Hangup",sStation);
	cmd.End(strTmp);


	sendCmd = cmd.GetCmd();

	SendCMDToPBX(sendCmd);
//	bRet = SendCMDToPBX(sendCmd);
	m_pDlg->m_Log.Log(sendCmd);

	return TRUE;
}

BOOL CAgent::Conference()
{
	CGenCMD cmd;
	std::string sendCmd;
	std::string strTmp;
	//设置主信道
	cmd.Start();
	cmd.Gen("Action","Setvar");
	cmd.Gen("Channel",sSelfChanID);
	cmd.Gen("Variable","my-confMasterChannel");
	cmd.Gen("Value",sSelfChanID);
	cmd.End("Setvar|Conf");
	sendCmd = cmd.GetCmd();
	SendCMDToPBX(sendCmd);

    //设置会议号码
	cmd.Start();
	cmd.Gen("Action","Setvar");
	cmd.Gen("Channel",sSelfChanID);
	cmd.Gen("Variable","my-confNo");
	cmd.Gen("Value",sStation);
	cmd.End("Setvar|Conf");
	sendCmd = cmd.GetCmd();
	SendCMDToPBX(sendCmd);

	cmd.Start();
	cmd.Gen("Action","Setvar");
	cmd.Gen("Channel",sHoldID);
	cmd.Gen("Variable","my-confNo");
	cmd.Gen("Value",sStation);
	cmd.End("Setvar|Conf");
	sendCmd = cmd.GetCmd();
	SendCMDToPBX(sendCmd);

	//设置主信道
	cmd.Start();
	cmd.Gen("Action","Setvar");
	cmd.Gen("Channel",sHoldID);
	cmd.Gen("Variable","my-confMasterChannel");
	cmd.Gen("Value",sSelfChanID);
	cmd.End("Setvar|Conf");
	sendCmd = cmd.GetCmd();
	SendCMDToPBX(sendCmd);

	cmd.Start();
	cmd.Gen("Action","Setvar");
	cmd.Gen("Channel",sOtherID);
	cmd.Gen("Variable","my-confNo");
	cmd.Gen("Value",sStation);
	cmd.End("Setvar|Conf");
	sendCmd = cmd.GetCmd();
	SendCMDToPBX(sendCmd);

	//设置主信道
	cmd.Start();
	cmd.Gen("Action","Setvar");
	cmd.Gen("Channel",sOtherID);
	cmd.Gen("Variable","my-confMasterChannel");
	cmd.Gen("Value",sSelfChanID);
	cmd.End("Setvar|Conf");
	sendCmd = cmd.GetCmd();
	SendCMDToPBX(sendCmd);

	//设置信号的 HOLD变量
	cmd.Start();
	cmd.Gen("Action","Setvar");
	cmd.Gen("Channel",sSelfChanID);
	cmd.Gen("Variable","my-holdDisplay");

	strTmp = "0|Conference|" + sStation;
//	strTmp.Format("0|Conference|%s",sStation);
	cmd.Gen("Value",strTmp);
	cmd.Gen("Async","1");
	cmd.End(strTmp);
    sendCmd = cmd.GetCmd();
	SendCMDToPBX(sendCmd);

	//设置信号的 HOLD变量
	cmd.Start();
	cmd.Gen("Action","Setvar");
	cmd.Gen("Channel",sOtherID);
	cmd.Gen("Variable","my-holdDisplay");
	cmd.Gen("Value","1");
	cmd.Gen("Async","1");
	cmd.End("Conference");
    sendCmd = cmd.GetCmd();
	SendCMDToPBX(sendCmd);

	cmd.Start();
	cmd.Gen("Action","Redirect");
	cmd.Gen("Channel",sSelfChanID);
	cmd.Gen("ExtraChannel",sOtherID);
	cmd.Gen("Context","hold");
	cmd.Gen("Exten","s");
	cmd.Gen("Priority","1");
	cmd.Gen("Async","1");

    strTmp = "Conference|" + sStation + "|Redirect";
//	strTmp.Format("Conference|%s|Redirect",sStation);
	cmd.End(strTmp);
	sendCmd = cmd.GetCmd();
	SendCMDToPBX(sendCmd);

	return TRUE;
}

BOOL CAgent::Confercece_1()
{
	CGenCMD cmd;
	std::string strTmp;
	std::string sendCmd;
	cmd.Start();
	cmd.Gen("Action","Redirect");
	cmd.Gen("Channel",sSelfChanID);
	cmd.Gen("Context","conference");
	cmd.Gen("Exten","s");
	cmd.Gen("Priority","1");
	cmd.Gen("Async","1");

    strTmp = "ConsultConference-chanid|" + sStation + "|Redirect";
//	strTmp.Format("ConsultConference-chanid|%s|Redirect",sStation);
	cmd.End(strTmp);
	sendCmd = cmd.GetCmd();
	SendCMDToPBX(sendCmd);

	cmd.Start();
	cmd.Gen("Action","Redirect");
	cmd.Gen("Channel",sHoldID);
	cmd.Gen("ExtraChannel",sOtherID);
	cmd.Gen("Context","conference");
	cmd.Gen("Exten","s");
	cmd.Gen("Priority","1");
	cmd.Gen("Async","1");

	strTmp = "ConsultConference|" + sStation + "|Redirect";
//	strTmp.Format("ConsultConference|%s|Redirect",sStation);
	cmd.End(strTmp);

	sendCmd = cmd.GetCmd();
	SendCMDToPBX(sendCmd);

    return TRUE;
}

BOOL CAgent::Monitor()
{
	CGenCMD cmd;
	std::string sendCmd;
	BOOL bRet = FALSE;
	CUntiTool tool;
//	m_pDlg->SetLock();
	CAgent* pAgent = m_pDlg->GetAgentFromStation(sMointorNo);

	if(pAgent)
	{
		std::string sCaller = m_strConfigCaller + " <" + m_strConfigCaller + ">";
//	    sCaller.Format("%s <%s>",m_strConfigCaller,m_strConfigCaller);

		cmd.Start();
		cmd.Gen("Action","Originate");
		cmd.Gen("Channel",sChan);
		cmd.Gen("Timeout",sTimeout);
		cmd.Gen("Callerid",sCaller);
		cmd.Gen("Context","spy");
		cmd.Gen("Exten","s");
		cmd.Gen("Priority","1");
		std::string sVal = "my-spyChannel=" + pAgent->sSelfChanID + ",my-spyOptions=q,my-id=" + sStation;
//		sVal.Format("my-spyChannel=%s,my-spyOptions=q,my-id=%s",pAgent->sSelfChanID,sStation);

		cmd.Gen("Variable",sVal);
		cmd.Gen("Async","1");

		std::string sTmp = "Monitor|" + sStation + "|Originate";
//		sTmp.Format("Monitor|%s|Originate",sStation);
		cmd.End(sTmp);

		sendCmd = cmd.GetCmd();
	    bRet = SendCMDToPBX(sendCmd);
	    pAgent->Release();
	}
//	m_pDlg->SetUnLock();

	return bRet;
}

BOOL CAgent::Observer()
{
	CGenCMD cmd;
	std::string sendCmd;
	CUntiTool tool;

//	m_pDlg->SetLock();
	CAgent* pAgent = m_pDlg->GetAgentFromStation(sObserverNo);

	if(pAgent)
	{
		std::string sCaller = m_strConfigCaller + " <" + m_strConfigCaller + ">";
//	    sCaller.Format("%s <%s>",m_strConfigCaller,m_strConfigCaller);
		cmd.Start();
		cmd.Gen("Action","Originate");
		cmd.Gen("Channel",sChan);
		cmd.Gen("Timeout",sTimeout);
		cmd.Gen("Callerid",sCaller);
		cmd.Gen("Context","spy");
		cmd.Gen("Exten","s");
		cmd.Gen("Priority","1");
		std::string sVal;
		//w 低语模式
		//B  会议模式

		sVal = "my-spyChannel=" + pAgent->sSelfChanID + ",my-spyOptions=Bq,my-id=" + sStation;
//		sVal.Format("my-spyChannel=%s,my-spyOptions=Bq,my-id=%s",pAgent->sSelfChanID,sStation);
		cmd.Gen("Variable",sVal);
		cmd.Gen("Async","1");

		std::string sTmp = "Observer|" + sStation + "|Originate";
//		sTmp.Format("Observer|%s|Originate",sStation);
		cmd.End(sTmp);


		sendCmd = cmd.GetCmd();
		SendCMDToPBX(sendCmd);
		pAgent->Release();
	}
//	m_pDlg->SetUnLock();

    return FALSE;
}

BOOL CAgent::HoldupCall()
{
	CGenCMD cmd;
	std::string sendCmd;
	BOOL bRet = FALSE;
	CUntiTool tool;

//	m_pDlg->SetLock();
	CAgent* pAgent = m_pDlg->GetAgentFromStation(sHoldupCallNo);

	if(pAgent)
	{
		std::string sCaller = m_strConfigCaller + " <" + m_strConfigCaller + ">";
//		sCaller.Format("%s <%s>",m_strConfigCaller,m_strConfigCaller);
		cmd.Start();
		cmd.Gen("Action","Originate");
		cmd.Gen("Channel",sChan);
		cmd.Gen("Timeout",sTimeout);
		cmd.Gen("Callerid",sCaller);
		cmd.Gen("Context","intercept");
		cmd.Gen("Exten","s");
		cmd.Gen("Priority","1");
		std::string sVal = "my-interceptedChannel=" + pAgent->sOtherID + ",my-spyOptions=q,my-id=" + sStation;
//		sVal.Format("my-interceptedChannel=%s,my-spyOptions=q,my-id=%s",pAgent->sOtherID,sStation);
		cmd.Gen("Variable",sVal);
		cmd.Gen("Async","1");

		std::string sTmp = "HoldupCall|" + sStation + "|Originate";
//		sTmp.Format("HoldupCall|%s|Originate",sStation);
		cmd.End(sTmp);

		sendCmd = cmd.GetCmd();
		bRet = SendCMDToPBX(sendCmd);
		pAgent->Release();
	}
//	m_pDlg->SetUnLock();

	return bRet;
}

std::string CAgent::GetUCIDFromPBX()
{
	CGenCMD cmd;
	cmd.Start();
	cmd.Gen("Action","GetVar");
	cmd.Gen("Channel",sSelfChanID);
	cmd.Gen("Variable","x-sinosoftcrm-ucid");
	std::string sTmp = "ucid|" + sStation + "|GetVar";
//	sTmp.Format("ucid|%s|GetVar",sStation);
	cmd.End(sTmp);

	std::string sSendCmd = cmd.GetCmd();

	SendCMDToPBX(sSendCmd);
	return std::string();
}

BOOL CAgent::EnforceLogOut(const std::string &sDnis, const std::string &sAgentId)
{
	//m_pDlg->SetLock();
	CAgent* pAgent = m_pDlg->GetAgentFromStation(sDnis);

	if(pAgent)
	{
		if(pAgent->sTeantid == sTeantid)
		{
			if (pAgent->m_sStationType=="PSTN")
			{//pstn: 手机或者座机签入
				pAgent->LogoutTalkData();
			}
			else
			{//sip
				pAgent->LogoutSipTalkData();
			}
			pAgent->lock();
			pAgent->sAction = "Logout";
			pAgent->unlock();
			if(pAgent->AgentState != AS_Notready && pAgent->AgentState != AS_Wrap)
			{
				m_pDlg->m_Log.Log("工号%s非正常签出 签出前状态为%d",pAgent->sWorkNo.c_str(), pAgent->AgentState);
			}
			pAgent->LogOut();//正常签出: 不对签出进行限制
		}
		pAgent->Release();
	}
	else
	{
		//这个坐席已经不再CTI中，只是在交换机中被卡死了，就不做任何的权限限制了！！
		//直接发送Remove命令
        std::string sipNo ;
		if(sDnis.length()>6)
		{
		    sipNo = "SIP/" + sDnis + "/" + sDnis;
//			sipNo.Format("SIP/%s/%s",sDnis,sDnis);
		}
		else
		{
		    sipNo = "SIP/" + sDnis;
//			sipNo.Format("SIP/%s",sDnis);
		}

		std::vector<std::string> sArray;
//		sArray.SetSize(0,100);
		CUntiTool tool;
		std::string strQueue;

		//tool.DecodeString(sQueueName,sArray,',');
		tool.DecodeString(m_tempSkill,sArray,',');

		for(size_t i=0; i<sArray.size(); i++)
		{
			strQueue = sArray.at(i);
		//end
			CGenCMD cmd;

			cmd.Start();
			cmd.Gen("Action","QueueRemove");
			//cmd.Gen("Queue",sQueueName);
			cmd.Gen("Queue",strQueue);
			cmd.Gen("Interface",sipNo);
			cmd.Gen("Async","1");

			std::string strTmp = "Logout|" + sipNo + "|QueueRemove";
//			strTmp.Format("Logout|%s|QueueRemove",sipNo);
			cmd.End(strTmp);

			std::string sendCmd = cmd.GetCmd();
			SendCMDToPBX(sendCmd);
		}
	}
	//m_pDlg->SetUnLock();

	return TRUE;
}

void CAgent::SetMainSocket(CSocketManager* pSocket)
{
	m_pMainSock = pSocket;
}

int  CAgent::IsFullSills()
{
	//第一步判断是否有未登陆的
	std::map<std::string, std::string>::iterator pos = m_mapSkillT.begin();
	//auto pos = m_mapSkillT.begin();
	BOOL bRet = FALSE;
	while(pos!=m_mapSkillT.end())
	{
		bRet = TRUE;
//		std::string sQueue;
		std::string sVal = pos->second;
//		m_mapSkillT.GetNextAssoc(pos,sQueue,sVal);

		if(sVal == "A")
		{
			return 0;
		}
		++pos;
	}

	//第二步判断是否有签入失败的
	pos = m_mapSkillT.begin();
	while(pos!=m_mapSkillT.end())
	{
		bRet = TRUE;
//		std::string sQueue;
		std::string sVal = pos->second;
//		m_mapSkillT.GetNextAssoc(pos,sQueue,sVal);

		if(sVal == "F")
		{
			return 1;
		}
		++pos;
	}

	if(bRet)
	{
		return 2;
	}
	else
	{
		return -1;
	}
}

void  CAgent::setSkillState(const std::string &queue, const std::string &val)
{
	m_mapSkillT[queue] = val;
}

int CAgent::IsOutSkills()
{
	std::map<std::string, std::string>::iterator pos = m_mapSkillT.begin();
	//auto pos = m_mapSkillT.begin();
	BOOL bRet = FALSE;
	while(pos!=m_mapSkillT.end())
	{
		bRet = TRUE;
//		std::string sQueue;
		std::string sVal = pos->second;
//		m_mapSkillT.GetNextAssoc(pos,sQueue,sVal);

		if(sVal != "C")
		{
			return 1;//存在未迁出的坐席
		}
		++pos;
	}

	if(bRet)
	{
		return 2;//坐席全部迁出了
	}
	else{
		return -1;//map为空！
	}
}

BOOL CAgent::SendDTMF(const std::string &dtmf)
{
	CGenCMD cmd;
	cmd.Start();
	cmd.Gen("Action","PlayDTMF");
	cmd.Gen("Channel",sOtherID);
	cmd.Gen("Digit",dtmf);
	std::string sTmp = "SendDTMF|" + sStation + "|PlayDTMF";
//	sTmp.Format("SendDTMF|%s|PlayDTMF",sStation);
	cmd.End(sTmp);

	std::string sSendCmd = cmd.GetCmd();

	BOOL bRet = SendCMDToPBX(sSendCmd);
	return bRet;
}

std::string CAgent::GetPopData()
{
	std::string popdata = "";
	if (!m_listPopData.empty())
	{
		std::list<liPopdata*>::iterator it = m_listPopData.end();
		//auto it = m_listPopData.end();
		--it;
/*
		std::string ani				= (*it)->ani;
		std::string dnis			= (*it)->dnis;
		std::string callID			= (*it)->callID;
		std::string ucid			= (*it)->ucid;
		std::string recordid		= (*it)->recordid;
		std::string accessNumber	= (*it)->accessNumber;
		std::string ivrTrack		= (*it)->ivrTrack;
		std::string callRingTime	= (*it)->callRingTime;
		std::string callStartTime	= (*it)->callStartTime;
		std::string callClearTime	= (*it)->callClearTime;

*/
        popdata += "ani=" +	           (*it)->ani;
		popdata += ";dnis=" +          (*it)->dnis;
		popdata += ";callid=" +        (*it)->callID;
		popdata += ";ucid=" +          (*it)->ucid;
		popdata += ";recordid=" +      (*it)->recordid;
		popdata += ";accessNumber=" +  (*it)->accessNumber;
		popdata += ";ivrTrack=" +      (*it)->ivrTrack;
		popdata += ";callRingTime=" +  (*it)->callRingTime;
		popdata += ";callStartTime=" + (*it)->callStartTime;
		popdata += ";callEndTime=" +   (*it)->callClearTime;

        delete *it;
		m_listPopData.erase(it);

//		popdata.Format("ani=%s;dnis=%s;callid=%s;ucid=%s;recordid=%s;accessNumber=%s;ivrTrack=%s;callRingTime=%s;callStartTime=%s;callEndTime=%s",ani,dnis,callID,ucid,recordid,accessNumber,ivrTrack,callRingTime,callStartTime,callClearTime);
	}
	return popdata;
}

void CAgent::savePopData()
{
	liPopdata* popdata = new liPopdata;
	popdata->ani = orginAni;
	popdata->dnis = orginDnis;
	popdata->callID = m_callID;
	popdata->ucid = originUCID;
	popdata->recordid = m_recordID;
	popdata->accessNumber = m_accessNumber;
	popdata->ivrTrack = m_ivrTrack;
	popdata->callRingTime = m_callRingTime;
	popdata->callStartTime = m_callStartTime;
	popdata->callClearTime = m_callClearTime;

	m_listPopData.push_back(popdata);
}

void CAgent::lock()
{
    //std::cout << "lock" << std::endl;
    if(!sStation.empty())
        CAgentLock::GetInstance().Log("Agent lock %s", sStation.c_str());
    else
        CAgentLock::GetInstance().Log("Agent lock");
    m_CriticalSection.acquire();
}


void CAgent::unlock()
{
    //std::cout << "unlock" << std::endl;
    if(!sStation.empty())
        CAgentLock::GetInstance().Log("Agent unlock %s", sStation.c_str());
    m_CriticalSection.release();
}

int CAgent::AddRef()
{
    int n = __sync_add_and_fetch(&m_cRef, 1);
    //std::cout << "agent addref " << n << std::endl;
    if(!sStation.empty())
        CAgentLock::GetInstance().Log("Agent %s: AddRef=%d", sStation.c_str(), n);
    return n;
    //return __sync_add_and_fetch(&m_cRef, 1);
}

void CAgent::Release()
{
    int n = __sync_sub_and_fetch(&m_cRef, 1);
    //std::cout << "agent release " << n << std::endl;
    if(!sStation.empty())
        CAgentLock::GetInstance().Log("Agent %s: Release=%d", sStation.c_str(), n);
    //if(__sync_sub_and_fetch(&m_cRef, 1) < 1)
    if(n < 1)
    {
        //sock->closeServer();
        delete this;
    }
}
