// MakeCallEvt.cpp: implementation of the CMakeCallEvt class.
//
//////////////////////////////////////////////////////////////////////

#include "MakeCallEvt.h"
#include "XmlMarkup.h"
#include "../GenCMD.h"
#include "../GeneralUtils.h"
#include "../logclass/log.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMakeCallEvt::CMakeCallEvt()
{

}

CMakeCallEvt::~CMakeCallEvt()
{

}
//解析软电话消息
BOOL CMakeCallEvt::ParaSoftPhoneRequestMsg(const std::string &sDoc)
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
		m_strDnis = xml.GetElemContent();

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
std::string CMakeCallEvt::EnSoftPhoneMsg()
{
	std::string strBody;

	if(m_strRet == _T("Succ"))
	{
	strBody = "<AskPT>\
<Source  Value=AskProxy />\
<Event  Value=Dial />\
<Body>\
<Result>Succ</Result>\
<ANI>[ANI]</ANI>\
<DNIS>[DNIS]</DNIS>\
<Director>[Director]</Director>\
<UCID>[UCID]</UCID>\
<UserData>[UserData]</UserData>\
<AgentId>[AgentId]</AgentId>\
<Station>[Station]</Station>\
<Time>[Time]</Time>\
</Body>\
</AskPT>";
	REPLACE(strBody, "[ANI]", m_strAni);
	REPLACE(strBody, "[DNIS]", m_strDnis);
	//外呼的CallType暂定为1
	REPLACE(strBody, "[Director]", m_strDirector);
	REPLACE(strBody, "[UCID]", m_strUCID);
	REPLACE(strBody, "[UserData]", m_strUserData);
	REPLACE(strBody, "[AgentId]",m_strAgentId);
	REPLACE(strBody, "[Station]",m_strStation);
	REPLACE(strBody, "[Time]",m_strTime);
	}
	else if(m_strRet == _T("Fail"))
	{
	strBody = "<AskPT>\
<Source  Value=AskProxy />\
<Event  Value=Dial />\
<Body>\
<Result>Fail</Result>\
<Cause>[Cause]</Cause>\
<UserData>[UserData]</UserData>\
<AgentId>[AgentId]</AgentId>\
<Station>[Station]</Station>\
<Time>[Time]</Time>\
</Body>\
</AskPT>";
	   REPLACE(strBody, "[Cause]", m_strCause);
	   REPLACE(strBody, "[UserData]", m_strUserData);
	   REPLACE(strBody, "[AgentId]",m_strAgentId);
	   REPLACE(strBody, "[Station]",m_strStation);
       REPLACE(strBody, "[Time]",m_strTime);
	}


	strBody += "\r\n\r\n";
   return strBody;
}
