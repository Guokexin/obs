// RingEvt.cpp: implementation of the CRingEvt class.
//
//////////////////////////////////////////////////////////////////////

#include "RingEvt.h"
#include "../GeneralUtils.h"
#include "./XmlMarkup.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRingEvt::CRingEvt()
{

}

CRingEvt::~CRingEvt()
{

}
/*
Event: Dail-Ringing
Privilege: call,all
CallerId: 2000
Channel1: SIP/2000-00000008
Channel2: SIP/2001-00000009
DialString: guokx
*/
BOOL    CRingEvt::ParaResponseMsg(const std::string &strEvt)
{
   CGeneralUtils myGeneralUtils;

   char szAni[64];
   memset(szAni,0,64);
   myGeneralUtils.GetStringValue(strEvt,"CallerId",szAni);
   m_strAni = szAni;

   char szChan2[64];
   memset(szChan2,0,64);
   myGeneralUtils.GetStringValue(strEvt,"Channel2",szChan2);
   std::string strChan2(szChan2);
   size_t index1 = strChan2.find("/");
   size_t index2 = strChan2.find("-");
   m_strDnis = strChan2.substr(index1+1,index2-index1-1);

   char szChan1[64];
   memset(szChan1,0,64);
   myGeneralUtils.GetStringValue(strEvt,"Channel1",szChan1);





   return TRUE;
}
//对asterisk返回的消息进行编码，返回给软电话端
/*
<AskPT>
<Source  Value=AskProxy  />
<Response  Value=Ringing  />
<Body>
<Ani>[Ani]</Anis>
<Dnis>[Dnis]</Dnis>
<UserData>[UserData]</UserData>
</Body>
</AskPT>

*/
std::string    CRingEvt::EnSoftPhoneMsg()
{
    std::string strBody;
	strBody = "<AskPT>\
<Source  Value=AskProxy  />\
<Event  Value=Ringing  />\
<Body>\
<Ani>[Ani]</Ani>\
<Dnis>[Dnis]</Dnis>\
<Ucid>[Ucid]</Ucid>\
<Director>[Director]</Director>\
<UserData>[UserData]</UserData>\
<IvrTrack>[IvrTrack]</IvrTrack>\
<AgentId>[AgentId]</AgentId>\
<Station>[Station]</Station>\
<Time>[Time]</Time>\
</Body>\
</AskPT>";
	strBody += "\r\n\r\n";

	/*
	std::string m_strAni;
	std::string m_strDnis;
	std::string m_strUCID;
	std::string m_strDirect;
	std::string m_strUserData;
	*/

	REPLACE(strBody, "[Ani]", m_strAni);
	REPLACE(strBody, "[Dnis]", m_strDnis);
	REPLACE(strBody, "[Ucid]",m_strUCID);
	REPLACE(strBody, "[Director]",m_strDirect);
	REPLACE(strBody, "[UserData]", m_strUserData);
	REPLACE(strBody, "[IvrTrack]",m_strIvrTrack);
	REPLACE(strBody, "[AgentId]",m_strAgentId);
	REPLACE(strBody, "[Station]",m_strStation);
	REPLACE(strBody, "[Time]",m_strTime);
	return strBody;
}
