#include "SoftPhoneMsg.h"
#include "./AskMsg/XmlMarkup.h"

CSoftPhoneMsg::CSoftPhoneMsg()
{

}

CSoftPhoneMsg::~CSoftPhoneMsg()
{

}

//软电话发送消息进行编码
/*
<AskPT>\
<Source  Value=SoftPhone />\
<Action  Value=[Action] />\
<Body>\
<Station>[Station]</Station>\
<Dnis>[Dnis]</Dnis>\
<Reason>[Reason]</Reason>\
<UserData>[UserData]</UserData>\
</Body>\
</AskPT>
*/
std::string CSoftPhoneMsg::EnSoftPhoneMsg()
{
	std::string strBody = "";
	strBody = "<AskPT>\
<Source  Value=SoftPhone />\
<Action  Value=[Action] />\
<Body>\
<Station>[Station]</Station>\
<Dnis>[Dnis]</Dnis>\
<Reason>[Reason]</Reason>\
<UserData>[UserData]</UserData>\
</Body>\
</AskPT>";
	REPLACE(strBody, "[Action]", m_strAction);
	REPLACE(strBody, "[Station]", m_strStation);
	REPLACE(strBody, "[Dnis]", m_strDnis);
	REPLACE(strBody, "[UserData]", m_strUserData);

	return strBody;
}
//软电话解析Response 消息
/*
<AskPT>
<Source  Value= AskProxy />
<Response Value=SetIdle />
<Body>
<Result>Fail</Result>
<Cause>[Cause]</Cause>
<UserData>[UserData]</UserData>
</Body>
</AskPT>
*/
BOOL CSoftPhoneMsg::ParaProxyResponseMsg(const std::string &strDoc)
{
	CMarkup xml;
	xml.SetDoc(strDoc);

	xml.FindElem(_T("AskPT"));
	xml.IntoElem();

	xml.FindElem(_T("Response"));
	m_strAction = xml.GetAttrib("Value");

	bool bFind = xml.FindElem(_T("Body"));
	if(bFind)
	{
		bool bInto = xml.IntoElem();
		if(bInto)
		{
			xml.FindElem(_T("Result"));
			m_strRet = xml.GetElemContent();
			if(m_strRet == "Fail")
			{
               	xml.FindElem(_T("Cause"));
				m_strCause = xml.GetElemContent();
			}
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

