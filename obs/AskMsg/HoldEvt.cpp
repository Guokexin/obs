// HoldEvt.cpp: implementation of the CHoldEvt class.
//
//////////////////////////////////////////////////////////////////////

#include "HoldEvt.h"
#include "../GeneralUtils.h"
#include "XmlMarkup.h"
#include "../GenCMD.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHoldEvt::CHoldEvt()
{

}

CHoldEvt::~CHoldEvt()
{

}
BOOL CHoldEvt::ParaSoftPhoneRequestMsg(const std::string &strDoc)
{
	CMarkup xml;
	xml.SetDoc(strDoc);

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

// void CHoldEvt::EnCode(_AgentS* pAgent)
// {
// 	CGenCMD cmd;
// 	cmd.Start();
// 	cmd.Gen("Action","Redirect");
// 	cmd.Gen("Channel",pAgent->chanid);
// 	cmd.Gen("ExtraChannel",pAgent->secondchanid);
// 	cmd.Gen("Context","music");
// 	cmd.Gen("Exten","s");
// 	cmd.Gen("Priority","1");
// 	cmd.Gen("Async","1");
// 	CString strTmp;
// 	strTmp.Format("Hold|%s|Redirect",pAgent->station);
// 	cmd.End(strTmp);
//
// 	cmd.SendCMD();
//
// 	CString strActionID = cmd.GetActionID();
// 	strcpy(pAgent->actionid,strActionID.GetBuffer(0));
// 	strActionID.ReleaseBuffer(-1);
// }

/*
Event: MusicOnHold
Privilege: call,all
State: Start
Channel: SIP/2000-00000037
UniqueID: 1284701356.59
*/
//askproxy 解析asterisk pbx返回的消息

BOOL    CHoldEvt::ParaResponseMsg(const std::string &strEvt)
{
	CGeneralUtils myGeneralUtils;

	char szState[64];
	memset(szState,0,64);
	myGeneralUtils.GetStringValue(strEvt,"State",szState);

	if(strcmp(szState,"Start") != 0)
	{
		return FALSE;
	}

	char szChan[64];
	memset(szChan,0,64);
	myGeneralUtils.GetStringValue(strEvt,"Channel",szChan);

	m_strChan = szChan;

	size_t index1 = m_strChan.find("/");
	size_t index2 = m_strChan.find("-");

	size_t nCount = index2-index1-1;

	m_strStation = m_strChan.substr(index1+1,nCount);

	return TRUE;
}
/*
<AskPT>
<Source Value=AskProxy />
<Response  Value=Hold />
<Body>
<Result>Succ</Result>
<UserData>[UserData]</UserData>
</Body>
</AskPT>
*/
//对asterisk返回的消息进行编码，返回给软电话端

std::string    CHoldEvt::EnSoftPhoneMsg()
{
	std::string	strBody = "\
<AskPT>\
<Source Value=AskProxy />\
<Event  Value=Hold />\
<Body>\
<Result>[Result]</Result>\
<UserData>[UserData]</UserData>\
<UCID>[UCID]</UCID>\
<AgentId>[AgentId]</AgentId>\
<Station>[Station]</Station>\
<Time>[Time]</Time>\
</Body>\
</AskPT>";

	REPLACE(strBody, "[Result]", m_strRet);
	REPLACE(strBody, "[UserData]", m_strUserData);
	REPLACE(strBody, "[UCID]",m_strUCID);
    REPLACE(strBody, "[AgentId]",m_strAgentId);
	REPLACE(strBody, "[Station]",m_strStation);
	REPLACE(strBody, "[Time]",m_strTime);

	strBody += "\r\n\r\n";
	return strBody;
}


