// HoldupCallEvt.cpp: implementation of the CHoldupCallEvt class.
//
//////////////////////////////////////////////////////////////////////

#include "HoldupCallEvt.h"



#include "XmlMarkup.h"
#include "../GenCMD.h"
#include "../GeneralUtils.h"
#include "../logclass/log.h"
#include "../UntiTool.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHoldupCallEvt::CHoldupCallEvt()
{

}

CHoldupCallEvt::~CHoldupCallEvt()
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
BOOL CHoldupCallEvt::ParaSoftPhoneRequestMsg(const std::string &sDoc)
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
        m_strHoldupNo = xml.GetElemContent();

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
std::string CHoldupCallEvt::EnSoftPhoneMsg()
{
    std::string strBody;

    if(m_strRet == _T("Succ"))
    {
        strBody = "<AskPT>\
<Source  Value=AskProxy />\
<Event  Value=HoldupCall />\
<Body>\
<Result>Succ</Result>\
<UserData>[UserData]</UserData>\
</Body>\
</AskPT>";
        REPLACE(strBody, "[UserData]", m_strUserData);
    }
    else if(m_strRet == _T("Fail"))
    {
        strBody = "<AskPT>\
<Source  Value=AskProxy />\
<Event  Value=HoldupCall />\
<Body>\
<Result>Fail</Result>\
<Cause>[Cause]</Cause>\
<UserData>[UserData]</UserData>\
</Body>\
</AskPT>";
        REPLACE(strBody, "[Cause]", m_strCause);
        REPLACE(strBody, "[UserData]", m_strUserData);
    }

    strBody += "\r\n\r\n";
    return strBody;
}
