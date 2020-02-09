// MonitorEvt.cpp: implementation of the CMonitorEvt class.
//
//////////////////////////////////////////////////////////////////////

#include "MonitorEvt.h"

#include "XmlMarkup.h"
#include "../GenCMD.h"
#include "../GeneralUtils.h"
#include "../logclass/log.h"
#include "../UntiTool.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMonitorEvt::CMonitorEvt()
{

}

CMonitorEvt::~CMonitorEvt()
{

}

//解析软电话消息
/*
<AskPT>
   <Body>
     <Station>[Station]</Station>
	 <Dnis>[Station]</Dnis>
	 <UserData>[Station]</UserData>
   </Body>
</AskPT>
*/
BOOL CMonitorEvt::ParaSoftPhoneRequestMsg(const std::string &sDoc)
{
	CMarkup xml;
	xml.SetDoc(sDoc);

	xml.FindElem(_T("AskPT"));
	xml.IntoElem();

	bool bFind = xml.FindElem(_T("Body"));
	if(bFind)
	{
		xml.IntoElem();
		xml.FindElem(_T("Station"));
		m_strStation = xml.GetElemContent();

		xml.FindElem(_T("Dnis"));
		m_strMonitorNo = xml.GetElemContent();

		xml.FindElem(_T("UserData"));
		m_strUserData = xml.GetElemContent();
		xml.OutOfElem();
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}
//将请求结果返回给软电话
std::string CMonitorEvt::EnSoftPhoneMsg()
{
	std::string strBody;

	if(m_strRet == _T("Succ"))
	{
	strBody = "<AskPT>\
<Source  Value=AskProxy />\
<Event  Value=Monitor />\
<Body>\
<Result>Succ</Result>\
<UserData>[UserData]</UserData>\
<UCID>[UCID]</UCID>\
<AgentId>[AgentId]</AgentId>\
<Station>[Station]</Station>\
<Time>[Time]</Time>\
</Body>\
</AskPT>";
	REPLACE(strBody, "[UserData]", m_strUserData);
	REPLACE(strBody, "[UCID]",m_strUCID);
	REPLACE(strBody, "[AgentId]",m_strAgentId);
	REPLACE(strBody, "[Station]",m_strStation);
	REPLACE(strBody, "[Time]",m_strTime);
	}
	else if(m_strRet == _T("Fail"))
	{
	strBody = "<AskPT>\
<Source  Value=AskProxy />\
<Event  Value=Monitor />\
<Body>\
<Result>Fail</Result>\
<Cause>[Cause]</Cause>\
<UserData>[UserData]</UserData>\
<UCID>[UCID]</UCID>\
<AgentId>[AgentId]</AgentId>\
<Station>[Station]</Station>\
<Time>[Time]</Time>\
</Body>\
</AskPT>";
	   REPLACE(strBody, "[Cause]", m_strCause);
	   REPLACE(strBody, "[UserData]", m_strUserData);
	   REPLACE(strBody, "[UCID]",m_strUCID);
       REPLACE(strBody, "[AgentId]",m_strAgentId);
       REPLACE(strBody, "[Station]",m_strStation);
       REPLACE(strBody, "[Time]",m_strTime);
	}

	strBody += "\r\n\r\n";
   return strBody;
}
/************************************************************************/
/*
Event: ChanSpyStart
Privilege: call,all
SpyerChannel: SIP/2001-000000d5
SpyeeChannel: SIP/2000-000000d1
************************************************************************/
BOOL  CMonitorEvt::ParaRetPBXEvt(const std::string &strEvt)
{

	return TRUE;

}
