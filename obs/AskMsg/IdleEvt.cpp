// IdleEvt.cpp: implementation of the CIdleEvt class.
//
//////////////////////////////////////////////////////////////////////

#include "IdleEvt.h"
#include "XmlMarkup.h"
#include "../GenCMD.h"
#include "../GeneralUtils.h"
#include "../UntiTool.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIdleEvt::CIdleEvt()
{

}

CIdleEvt::~CIdleEvt()
{

}


//解析软电话消息
BOOL CIdleEvt::ParaSoftPhoneRequestMsg(const std::string &sDoc)
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
//对PBX发送命令信息
// BOOL CIdleEvt::EnCodePBXMsg(CAgent* pAgent)
// {
//    return TRUE;
// }
//解析PBX返回的事件
BOOL CIdleEvt::ParaRetPBXEvt(const std::string &sDoc)
{

	return TRUE;
}
//将请求结果返回给软电话
std::string CIdleEvt::EnSoftPhoneMsg()
{

std::string	strBody = "\
<AskPT>\
<Source Value=AskProxy />\
<Event  Value=SetIdle />\
<Body>\
<Result>[Result]</Result>\
<UserData>[UserData]</UserData>\
<AgentId>[AgentId]</AgentId>\
<Station>[Station]</Station>\
<Time>[Time]</Time>\
</Body>\
</AskPT>";

REPLACE(strBody, "[Result]", m_strRet);
REPLACE(strBody, "[UserData]", m_strUserData);
REPLACE(strBody, "[AgentId]",m_strAgentId);
REPLACE(strBody, "[Station]",m_strStation);
REPLACE(strBody, "[Time]",m_strTime);

strBody += "\r\n\r\n";
	return strBody;
}

