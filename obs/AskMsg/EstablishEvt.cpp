// EstablishEvt.cpp: implementation of the CEstablishEvt class.
//
//////////////////////////////////////////////////////////////////////
#include "EstablishEvt.h"
#include "../GeneralUtils.h"
#include "../UntiTool.h"
#include "XmlMarkup.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEstablishEvt::CEstablishEvt()
{

}

CEstablishEvt::~CEstablishEvt()
{

}

//对asterisk返回的消息进行编码，返回给软电话端
std::string    CEstablishEvt::EnAskMsg()
{
    std::string strBody;

    strBody = "\
<AskPT>\
<Source  Value=AskProxy  />\
<Event  Value=Establish  />\
<Body>\
<Ani>[Ani]</Ani>\
<Dnis>[Dnis]</Dnis>\
<Ucid>[Ucid]</Ucid>\
<UserData>[UserData]</UserData>\
<AgentId>[AgentId]</AgentId>\
<Station>[Station]</Station>\
<Time>[Time]</Time>\
</Body>\
</AskPT>";

    strBody += "\r\n\r\n";

    REPLACE(strBody, "[Ani]", m_strAni);
    REPLACE(strBody, "[Dnis]", m_strDnis);
    REPLACE(strBody, "[Ucid]",m_strUCID);
    REPLACE(strBody, "[UserData]", m_strUserData);
    REPLACE(strBody, "[AgentId]",m_strAgentId);
    REPLACE(strBody, "[Station]",m_strStation);
	REPLACE(strBody, "[Time]",m_strTime);

    return strBody;
}

BOOL    CEstablishEvt::ParaAskMsg(const std::string &strAskMsg)
{

    CMarkup xml;
    xml.SetDoc(strAskMsg);

    xml.FindElem(_T("AskPT"));
    xml.IntoElem();

    xml.FindElem(_T("Event"));
    m_strEvt = xml.GetAttrib(_T("Value"));

    bool bFind = xml.FindElem(_T("Body"));
    if(bFind)
    {
        bool bInto = xml.IntoElem();
        if(bInto)
        {
            xml.FindElem(_T("Ani"));
            m_strAni = xml.GetElemContent();

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
    }
    else
    {
        return FALSE;
    }
    return TRUE;
}
