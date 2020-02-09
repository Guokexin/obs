// EstablishEvt.h: interface for the CEstablishEvt class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ESTABLISHEVT_H
#define ESTABLISHEVT_H

#include <string>
#include "../owntype.h"
/*
Event: Bridge
Privilege: call,all
Bridgestate: Link
Bridgetype: core
Channel1: SIP/2001-00000007
Channel2: SIP/2000-00000008
Uniqueid1: 1284660653.7
Uniqueid2: 1284660657.8
CallerID1: 2001
CallerID2: 2000
*/
class CEstablishEvt
{
public:
	CEstablishEvt();
	virtual ~CEstablishEvt();



	std::string  m_strAni;
	std::string  m_strDnis;
	std::string  m_strUCID;

	std::string m_strUserData;

    std::string m_strStation;
	std::string m_strAgentId;
	std::string m_strTime;

	std::string m_strEvt;

	//askproxy 解析asterisk pbx返回的消息
	BOOL    ParaResponseMsg(const std::string &strEvt);
	//对asterisk返回的消息进行编码，返回给软电话端
	std::string    EnAskMsg();

	BOOL    ParaAskMsg(const std::string &strAskMsg) ;

};

#endif
