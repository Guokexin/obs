// ConsultCancelEvt.cpp: implementation of the CConsultCancelEvt class.
//
//////////////////////////////////////////////////////////////////////

#include "ConsultCancelEvt.h"
#include "XmlMarkup.h"
#include "../GenCMD.h"
#include "../GeneralUtils.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConsultCancelEvt::CConsultCancelEvt()
{

}

CConsultCancelEvt::~CConsultCancelEvt()
{

}
BOOL CConsultCancelEvt::ParaSoftPhoneRequestMsg(const std::string &sDoc)
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
