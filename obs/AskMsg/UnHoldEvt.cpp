// UnHoldEvt.cpp: implementation of the CUnHoldEvt class.
//
//////////////////////////////////////////////////////////////////////

#include "UnHoldEvt.h"
#include "XmlMarkup.h"
#include "../GenCMD.h"
#include "../GeneralUtils.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUnHoldEvt::CUnHoldEvt()
{

}

CUnHoldEvt::~CUnHoldEvt()
{

}

BOOL CUnHoldEvt::ParaSoftPhoneRequestMsg(const std::string &strDoc)
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

