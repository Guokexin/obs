// EnforceLogOutEvt.cpp: implementation of the CEnforceLogOutEvt class.
//
//////////////////////////////////////////////////////////////////////

#include "EnforceLogOutEvt.h"
#include "XmlMarkup.h"
#include "../GeneralUtils.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEnforceLogOutEvt::CEnforceLogOutEvt()
{

}

CEnforceLogOutEvt::~CEnforceLogOutEvt()
{

}
/*

*/
BOOL CEnforceLogOutEvt::ParaSoftPhoneRequestMsg(const std::string &strDoc)
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
		std::string sDnis = xml.GetElemContent();

		CGeneralUtils myGenernalUtils;

		char szDnis[32];
		memset(szDnis,0,32);
		myGenernalUtils.GetStringValue(sDnis,"dnis",szDnis);
		m_strLogoutStation = szDnis;

		char szAgentID[32];
		memset(szAgentID,0,32);
		myGenernalUtils.GetStringValue(sDnis,"agentid",szAgentID);
		m_strLogoutAgentId = szAgentID;

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

*/
std::string    CEnforceLogOutEvt::EnSoftPhoneMsg()
{

	return "";
}
