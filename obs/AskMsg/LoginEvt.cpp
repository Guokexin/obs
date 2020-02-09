// LoginEvt.cpp: implementation of the CLoginEvt class.
//
//////////////////////////////////////////////////////////////////////

#include "LoginEvt.h"
#include "XmlMarkup.h"
#include "../GenCMD.h"
#include "../GeneralUtils.h"
#include "../UntiTool.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLoginEvt::CLoginEvt()
{
	m_sSoftPhoneVersion = "1.0";

}

CLoginEvt::~CLoginEvt()
{

}
//////////////////////////////////////////////////////////////////////////
//2012-12-04 edit by chenlin
//解析软电话消息
//////////////////////////////////////////////////////////////////////////
BOOL CLoginEvt::ParaSoftPhoneRequestMsg(const std::string &sDoc)
{
	CMarkup xml;
	xml.SetDoc(sDoc);

//	xml.FindElem(_T("AskPT"));
//	xml.IntoElem();

	BOOL bFind = xml.FindElem(_T("AskPT"));
	if(bFind)
	{
		BOOL bInto = xml.IntoElem();
		if(bInto)
		{
			bFind = xml.FindElem(_T("Source"));
			if(bFind)
			{
				m_strSource = xml.GetAttrib(_T("Value"));
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}else{
		return FALSE;
	}


	BOOL bFind1= xml.FindElem(_T("Body"));
	if(bFind1)
	{
		BOOL bInto1 = xml.IntoElem();
		if(bInto1)
		{
			xml.FindElem(_T("WorkNo"));
			m_strWorkNo = xml.GetElemContent();
			xml.FindElem(_T("Station"));
			m_strStation = xml.GetElemContent();
			xml.FindElem(_T("Password"));
			m_strPwd = xml.GetElemContent();
			xml.FindElem(_T("UserData"));
			m_strUserData = xml.GetElemContent();
			bFind = xml.FindElem(_T("Version"));
			if(bFind1)
				m_sSoftPhoneVersion = xml.GetData();

			m_bIsAgentReConn = FALSE;
			bool bret = xml.FindElem(_T("ReLogin")); //新增未退出软电话的重登录协议-用于CTI和客户端短连操作
			if(bret)
			{
				std::string srelogin = xml.GetElemContent();
				if(srelogin == "1")
					m_bIsAgentReConn = TRUE; //重连产生的登录事件
			}

			if (xml.FindElem(_T("Addr")))
			{
				m_strClientAddr = xml.GetElemContent();
			}
			if (xml.FindElem(_T("Status")))
			{
				std::string strLoginStatus = xml.GetElemContent();
				m_nDefStatus = atoi(strLoginStatus.c_str());
			}
			if  (xml.FindElem("HangupStatus"))
			{
			    std::string HangupStatus = xml.GetElemContent();
			    m_HangupStatus = atoi(HangupStatus.c_str());
			}
			if (xml.FindElem("Skills"))
			{
			    m_strSkills = xml.GetElemContent();
			}
			if (xml.FindElem("LoginType"))
			{
			    m_strLoginType = xml.GetElemContent();
			}
			if (xml.FindElem("ViewCaller"))
			{
			    m_strViewCaller = xml.GetElemContent();
			}

			xml.OutOfElem();
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}
//对PBX发送命令信息
/*
Action: QueueAdd
Queue: queue-test
Interface: SIP/20901001
Paused: false
ActionID: 1234567abcdefg

*/
// BOOL CLoginEvt::EnCodePBXMsg(CAgent* pAgent)
// {
// 	CGenCMD cmd;
//
// 	cmd.Start();
// 	cmd.Gen("Action","QueueAdd");
// 	cmd.Gen("Queue",pAgent->sQueueName);
// 	cmd.Gen("Interface",pAgent->sSipInterface);
// 	cmd.Gen("Paused","true");
// 	cmd.Gen("Async","1");
//
// 	std::string strTmp;
// 	strTmp.Format("Login|%s|QueueAdd",pAgent->sStation);
// 	cmd.End(strTmp);
//
// 	cmd.SendCMD();
//
// 	std::string strActionID = cmd.GetActionID();
// 	pAgent->sActionID = strActionID;
//
//     return TRUE;
// }
//解析PBX返回的事件
/*
Response: Success
ActionID: 1234567abcdefg
Message: Added interface to queue

  Event: QueueMemberAdded
  Privilege: agent,all
  Queue: queue-test
  Location: SIP/2001
  MemberName: SIP/2001
  Membership: dynamic
  Penalty: 0
  CallsTaken: 0
  LastCall: 0
  Status: 5
  Paused: 0
*/
BOOL CLoginEvt::ParaRetPBXEvt(const std::string &sDoc)
{
    CGeneralUtils myGeneralUtils;

	char szQueue[64];
	memset(szQueue,0,64);
	myGeneralUtils.GetStringValue(sDoc,"Queue",szQueue);//add by ly

	char szLocation[64];
	memset(szLocation,0,64);
	myGeneralUtils.GetStringValue(sDoc,"Location",szLocation);

	char szMemberName[64];
	memset(szMemberName,0,64);
	myGeneralUtils.GetStringValue(sDoc,"MemberName",szMemberName);

	char szMemberShip[64];
	memset(szMemberShip,0,64);
	myGeneralUtils.GetStringValue(sDoc,"MemberShip",szMemberShip);

	CUntiTool tool;
	m_strStation = tool.GetStationFromMemberName(szMemberName);


	m_strMemberName = szMemberShip;


	m_strRet = "Succ";
	m_sloginqueue = szQueue;

	return TRUE;
}
//将请求结果返回给软电话
std::string CLoginEvt::EnSoftPhoneMsg()
{
std::string	strBody = "\
<AskPT>\
<Source Value=AskProxy />\
<Event  Value=Login />\
<Body>\
<Result>[Result]</Result>\
<UserData>[UserData]</UserData>\
<AgentId>[AgentId]</AgentId>\
<Station>[Station]</Station>\
<Time>[Time]</Time>\
</Body>\
</AskPT>";

REPLACE(strBody, "[Result]", m_strRet);
REPLACE(strBody, "[UserData]", m_strCause);
REPLACE(strBody, "[AgentId]", m_strAgentId);
REPLACE(strBody, "[Station]", m_strStation);
REPLACE(strBody, "[Time]", m_strTime);

strBody += "\r\n\r\n";
	return strBody;
}
