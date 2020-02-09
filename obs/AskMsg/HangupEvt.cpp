// HangupEvt.cpp: implementation of the CHangupEvt class.
//
//////////////////////////////////////////////////////////////////////

#include "HangupEvt.h"

#include "XmlMarkup.h"
#include "../GenCMD.h"
#include "../GeneralUtils.h"
#include "../UntiTool.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHangupEvt::CHangupEvt()
{

}

CHangupEvt::~CHangupEvt()
{

}
//解析软电话消息
BOOL CHangupEvt::ParaSoftPhoneRequestMsg(const std::string &sDoc)
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
std::string CHangupEvt::EnSoftPhoneMsg()
{
	std::string strBody = "\
<AskPT>\
<Source  Value=AskProxy  />\
<Event  Value=Hangup  />\
<Body>\
<Cause>[Cause]</Cause>\
<UserData>[UserData]</UserData>\
<UCID>[UCID]</UCID>\
<AgentId>[AgentId]</AgentId>\
<Station>[Station]</Station>\
<Time>[Time]</Time>\
</Body>\
</AskPT>\
";
	REPLACE(strBody, "[Cause]", m_strReason);
	REPLACE(strBody, "[UserData]", m_strUserData);
	REPLACE(strBody, "[UCID]", m_strUCID);
	REPLACE(strBody, "[AgentId]",m_strAgentId);
	REPLACE(strBody, "[Station]",m_strStation);
	REPLACE(strBody, "[Time]",m_strTime);

	std::string strRet;
	strRet += strBody;
	strRet += "\r\n\r\n";
	return strRet;
}
