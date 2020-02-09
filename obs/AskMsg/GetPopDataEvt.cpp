// GetPopDataEvt.cpp: implementation of the CGetPopDataEvt class.
//
//////////////////////////////////////////////////////////////////////

#include "GetPopDataEvt.h"
#include "XmlMarkup.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGetPopDataEvt::CGetPopDataEvt()
{

}

CGetPopDataEvt::~CGetPopDataEvt()
{

}
//解析软电话消息
BOOL CGetPopDataEvt::ParaSoftPhoneRequestMsg(const std::string &sDoc)
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

		xml.FindElem(_T("Reason"));
		m_strReason = xml.GetElemContent();

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
std::string CGetPopDataEvt::EnSoftPhoneMsg()
{
	std::string strBody;

	if(m_strRet == _T("Succ"))
	{
	strBody = "<AskPT>\
<Source Value=AskProxy />\
<Response Value=GetPopData />\
<Body>\
<Result>Succ</Result>\
<UserData>[UserData]</UserData>\
<AgentId>[AgentId]</AgentId>\
<Station>[Station]</Station>\
<Time>[Time]</Time>\
</Body>\
</AskPT>";
	   REPLACE(strBody, "[UserData]", m_strUserData);
	   REPLACE(strBody, "[AgentId]",m_strAgentId);
	   REPLACE(strBody, "[Station]",m_strStation);
	   REPLACE(strBody, "[Time]",m_strTime);
	}
	else if(m_strRet == _T("Fail"))
	{
	strBody = "<AskPT>\
<Source Value=AskProxy />\
<Response Value=GetPopData />\
<Body>\
<Result>Fail</Result>\
<UserData>[UserData]</UserData>\
<AgentId>[AgentId]</AgentId>\
<Station>[Station]</Station>\
<Time>[Time]</Time>\
</Body>\
</AskPT>";
	REPLACE(strBody, "[UserData]", m_strUserData);
	REPLACE(strBody, "[AgentId]",m_strAgentId);
    REPLACE(strBody, "[Station]",m_strStation);
    REPLACE(strBody, "[Time]",m_strTime);
	}
	strBody += "\r\n\r\n";

	return strBody;
}
