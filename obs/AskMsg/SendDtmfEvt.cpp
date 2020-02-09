// SendDtmfEvt.cpp: implementation of the CSendDtmfEvt class.
//
//////////////////////////////////////////////////////////////////////

#include "XmlMarkup.h"
#include "SendDtmfEvt.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSendDtmfEvt::CSendDtmfEvt()
{

}

CSendDtmfEvt::~CSendDtmfEvt()
{

}

//解析软电话消息
BOOL CSendDtmfEvt::ParaSoftPhoneRequestMsg(const std::string &sDoc)
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
		digits = xml.GetElemContent();

		xml.OutOfElem();
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}
//将请求结果返回给软电话
std::string CSendDtmfEvt::EnSoftPhoneMsg()
{
	return "";
}
