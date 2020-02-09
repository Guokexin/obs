// RecordEvt.cpp: implementation of the CRecordEvt class.
//
//////////////////////////////////////////////////////////////////////

#include "RecordEvt.h"
#include <string.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRecordEvt::CRecordEvt()
{

}

CRecordEvt::~CRecordEvt()
{

}

std::string CRecordEvt::EnSoftPhoneMsg()
{
std::string	strBody = "\
<AskPT>\
<Source Value=AskProxy />\
<Event  Value=Record />\
<Body>\
<Result>[Result]</Result>\
<UCID>[UCID]</UCID>\
<RecordID>[RecordID]</RecordID>\
<AgentId>[AgentId]</AgentId>\
<Station>[Station]</Station>\
<Time>[Time]</Time>\
</Body>\
</AskPT>";

REPLACE(strBody, "[Result]","Succ");
REPLACE(strBody, "[UCID]",m_strUCID);
REPLACE(strBody, "[RecordID]",m_strRecordID);
REPLACE(strBody, "[AgentId]",m_strAgentId);
REPLACE(strBody, "[Station]",m_strStation);
REPLACE(strBody, "[Time]",m_strTime);


strBody += "\r\n\r\n";
	return strBody;
}
