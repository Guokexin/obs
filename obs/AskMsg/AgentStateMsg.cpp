#include "AgentStateMsg.h"
#include "XmlMarkup.h"

CAgentStateMsg::CAgentStateMsg()
{

}

CAgentStateMsg::~CAgentStateMsg()
{

}

std::string CAgentStateMsg::EnEvt(const std::string &sState)
{
	std::string strBody;
	strBody ="<AskPT>\
<Source  Value=AskProxy  />\
<Response  Value=AgentState  />\
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
/*
Event: QueueMemberPaused
Privilege: agent,all
Queue: queue-test
Location: SIP/2001
MemberName: SIP/2001
Paused: 0
*/
void CAgentStateMsg::ParaMsg(const std::string &sDoc)
{
   	CMarkup xml;
	xml.SetDoc(sDoc);

	xml.FindElem("AskPT");
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
		m_strAgentState = xml.GetElemContent();
	}
}
