// ChannelStateMsg.cpp: implementation of the CChannelStateMsg class.
//
//////////////////////////////////////////////////////////////////////

#include "ChannelStateMsg.h"
#include "./XmlMarkup.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChannelStateMsg::CChannelStateMsg()
{

}

CChannelStateMsg::~CChannelStateMsg()
{

}

std::string CChannelStateMsg::EnEvt(const std::string &sState)
{

	std::string strBody;
	strBody ="<AskPT>\
<Source  Value=AskProxy  />\
<Response  Value=ChanState  />\
<Body>\
<State>[State]</State>\
</Body>\
</AskPT>";

	REPLACE(strBody, "[State]", sState);

	std::string sRet;
	sRet += strBody;
	sRet += "\r\n\r\n";

	return sRet;
}
void CChannelStateMsg::ParaMsg(const std::string &sDoc)
{
	CMarkup xml;
	xml.SetDoc(sDoc);

	xml.FindElem(_T("AskPT"));
	xml.IntoElem();

	xml.FindElem("Source");
    m_strSource = xml.GetElemContent();

	xml.FindElem("Response");
	m_strResponse = xml.GetElemContent();

	bool bFind = xml.FindElem("Body");
	if(bFind)
	{
		xml.IntoElem();
        xml.FindElem("State");
		m_strChanState = xml.GetElemContent();
	}
}
