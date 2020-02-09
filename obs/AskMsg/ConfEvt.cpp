// ConfEvt.cpp: implementation of the CConfEvt class.
//
//////////////////////////////////////////////////////////////////////

#include "ConfEvt.h"
#include "XmlMarkup.h"
#include "../GenCMD.h"
#include "../GeneralUtils.h"
#include "../UntiTool.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfEvt::CConfEvt()
{

}

CConfEvt::~CConfEvt()
{

}
BOOL CConfEvt::ParaSoftPhoneRequestMsg(const std::string &strDoc)
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
/*
Action: Setvar
Channel: SIP/20901001-00000001
Variable: conference_no
Value: 123456
ActionID: 1234567abcdefg

  Action: Setvar
  Channel: SIP/20901002-00000002
  Variable: conference_no
  Value: 123456
  ActionID: 1234567abcdefg

	Action: Setvar
	Channel: SIP/20901003-00000003
	Variable: conference_no
	Value: 123456
	ActionID: 1234567abcdefg

*/

/*
Event: MeetmeJoin
Privilege: call,all
Channel: SIP/2000-00000001
Uniqueid: 1284880174.2
Meetme: 2001
Usernum: 2
CallerIDnum: 2000
CallerIDname: <unknown>

  Event: MeetmeJoin
  Privilege: call,all
  Channel: SIP/2002-00000002
  Uniqueid: 1284880198.4
  Meetme: 2001
  Usernum: 3
  CallerIDnum: 2002
  CallerIDname: <unknown>

	Event: MeetmeJoin
	Privilege: call,all
	Channel: SIP/2001-00000000
	Uniqueid: 1284880165.0
	Meetme: 2001
	Usernum: 1
	CallerIDnum: 2001
	CallerIDname: <unknown>

  Event=MeetmeJoin;
  Privilege=call,all;
  Channel=SIP/31002-0000002f;
  Uniqueid=1288435966.75;
  Meetme=31002;
  Usernum=3;
  CallerIDnum=27638251;
  CallerIDname=27638251
*/
BOOL CConfEvt::ParaResponseMsg(const std::string &strEvt)
{
    CGeneralUtils myGeneralUtils;

    char szUsernum[64];
    memset(szUsernum,0,64);
    myGeneralUtils.GetStringValue(strEvt,"Usernum",szUsernum);

    char szChan[64];
    memset(szChan,0,64);
    myGeneralUtils.GetStringValue(strEvt,"Channel",szChan);
    m_strChan = szChan;

    return TRUE;
}

std::string CConfEvt::EnSoftPhoneMsg()
{
    std::string	strBody = "\
<AskPT>\
<Source Value=AskProxy />\
<Event  Value=Conference />\
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
