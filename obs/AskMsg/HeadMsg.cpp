// HeadMsg.cpp: implementation of the CHeadMsg class.
//
//////////////////////////////////////////////////////////////////////

#include "HeadMsg.h"
#include "XmlMarkup.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHeadMsg::CHeadMsg()
{
   m_strResponse = "";
   m_strEvt = "";
   m_strSource = "";
}

CHeadMsg::~CHeadMsg()
{

}
/*
<AskPT>
<Source  Value=SoftPhone/>
<Action  Value=Login />

*/

BOOL CHeadMsg::ParaMsg(const std::string &strDoc)
{
	if(strDoc == "")
		return FALSE;
	else
	{
		CMarkup xml;
		xml.SetDoc(strDoc);
		bool bInto = false;
		bool bFind = xml.FindElem(_T("AskPT"));
		if(bFind)
		{
			bInto = xml.IntoElem();
			if(bInto)
			{
				bFind = xml.FindElem(_T("Source"));
				if(bFind)
				{
					m_strSource = xml.GetAttrib(_T("Value"));
				}
				else
				{
					return FALSE;
				}
				bFind = xml.FindElem(_T("Action"));
				if(bFind)
				{
					m_strAction = xml.GetAttrib(_T("Value"));
				}
				else
				{
					return FALSE;
				}
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
	}
	return TRUE;
}

BOOL CHeadMsg::ParaProxyMsg(const std::string &strDoc)
{
	if(strDoc == "")
		return FALSE;
	else
	{
		CMarkup xml;
		xml.SetDoc(strDoc);
		bool bInto = false;
		bool bFind = xml.FindElem(_T("AskPT"));
		if(bFind)
		{
			bInto = xml.IntoElem();
			if(bInto)
			{
				bFind = xml.FindElem(_T("Source"));
				if(bFind)
				{
					m_strSource = xml.GetAttrib(_T("Value"));
				}

				bFind = xml.FindElem(_T("Response"));
				if(bFind)
				{
					m_strResponse  = xml.GetAttrib(_T("Value"));
				}

			    bFind = xml.FindElem(_T("Event"));
				if(bFind)
				{
					m_strEvt = xml.GetAttrib(_T("Value"));
				}
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
	}
	return TRUE;
}
