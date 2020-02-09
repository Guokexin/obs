// LogoutEvt.cpp: implementation of the CLogoutEvt class.
//
//////////////////////////////////////////////////////////////////////

#include "LogoutEvt.h"
#include "XmlMarkup.h"
#include "../GenCMD.h"
#include "../GeneralUtils.h"
#include "../UntiTool.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLogoutEvt::CLogoutEvt()
{

}

CLogoutEvt::~CLogoutEvt()
{

}

//解析软电话消息

/*
<AskPT><Source  Value=SoftPhone />
<Action  Value=Logout />
<Body>
<Station>16008</Station><Dnis></Dnis><Reason>[Reason]</Reason><UserData></UserData>
</Body>
</AskPT>
*/
BOOL CLogoutEvt::ParaSoftPhoneRequestMsg(const std::string &strDoc)
{
    CMarkup xml;
    xml.SetDoc(strDoc);

    xml.FindElem(_T("AskPT"));
    xml.IntoElem();

    bool bFind = xml.FindElem(_T("Body"));
    if(bFind)
    {
        bool bInto = xml.IntoElem();
        if(bInto)
        {
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

    }
    else
    {
        return FALSE;
    }
    return  TRUE;
}

//解析PBX返回的事件
/*
Response: Success
ActionID: Logout|2001|QueueRemove|808a7f6a-cb73-419b-b59d-a389ba1dc192
Message: Removed interface from queue

Event: QueueMemberRemoved
Privilege: agent,all
Queue: queue-test
Location: SIP/2001
MemberName: SIP/2001
*/

BOOL CLogoutEvt::ParaRetPBXEvt(const std::string &sDoc)
{
    CGeneralUtils myGeneralUtils;

    char szQueue[64];
    memset(szQueue,0,64);
    myGeneralUtils.GetStringValue(sDoc,"Queue",szQueue);//

    char szMemberName[64];
    memset(szMemberName,0,64);
    myGeneralUtils.GetStringValue(sDoc,"MemberName",szMemberName);

    CUntiTool tool;
    m_strStation = tool.GetStationFromMemberName(szMemberName);

    m_strRet = "Succ";
    m_slogoutqueue = szQueue;

    return TRUE;
}
//将请求结果返回给软电话
std::string CLogoutEvt::EnSoftPhoneMsg()
{
    std::string	strBody = "\
<AskPT>\
<Source Value=AskProxy />\
<Event  Value=Logout />\
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
