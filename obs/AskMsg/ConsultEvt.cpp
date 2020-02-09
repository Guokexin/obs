// ConsultEvt.cpp: implementation of the CConsultEvt class.
//
//////////////////////////////////////////////////////////////////////

#include "ConsultEvt.h"
#include "../GenCMD.h"
#include "../GeneralUtils.h"
#include "XmlMarkup.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConsultEvt::CConsultEvt()
{

}

CConsultEvt::~CConsultEvt()
{

}
//
BOOL CConsultEvt::ParaSoftPhoneRequestMsg(const std::string &strDoc)
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
//

//对asterisk返回的消息进行编码，返回给软电话端
std::string    CConsultEvt::EnSoftPhoneMsg()
{
	std::string strBody;

	strBody = "\
<AskPT>\
<Source  Value=AskProxy  />\
<Event  Value=Consult  />\
<Body>\
<Ani>[Ani]</Ani>\
<Dnis>[Dnis]</Dnis>\
<UserData>[UserData]</UserData>\
</Body>\
</AskPT>";

	strBody += "\r\n\r\n";

	REPLACE(strBody, "[Ani]", m_strAni);
	REPLACE(strBody, "[Dnis]", m_strDnis);
	REPLACE(strBody, "[UserData]", m_strUserData);

	return strBody;
}
