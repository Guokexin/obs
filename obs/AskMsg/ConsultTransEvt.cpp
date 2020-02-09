// ConsultTransEvt.cpp: implementation of the CConsultTransEvt class.
//
//////////////////////////////////////////////////////////////////////

#include "ConsultTransEvt.h"
#include "XmlMarkup.h"
#include "../GenCMD.h"
#include "../GeneralUtils.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConsultTransEvt::CConsultTransEvt()
{

}

CConsultTransEvt::~CConsultTransEvt()
{

}
BOOL CConsultTransEvt::ParaSoftPhoneRequestMsg(const std::string &sDoc)
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
//
