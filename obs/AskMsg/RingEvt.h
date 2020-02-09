// RingEvt.h: interface for the CRingEvt class.
//
//////////////////////////////////////////////////////////////////////

#ifndef RINGEVT_H
#define RINGEVT_H

#include <string>
#include "../owntype.h"

/*
<AskPT>
<Source  Value=AskProxy  />
<Response  Value=Ringing  />
<Body>
<Ani>[Ani]</Anis>
<Dnis>[Dnis]</Dnis>
<UCID>[UCID]</UCID>
<Director>[Director]</Director>
<UCID[UCID]</UCID>>
</Body>
</AskPT>
*/

class CRingEvt
{
public:
	CRingEvt();
	virtual ~CRingEvt();
public:
	std::string m_strAni;
	std::string m_strDnis;
	std::string m_strUCID;
	std::string m_strDirect;
	std::string m_strUserData;
	std::string m_strEvt;

	std::string m_strIvrTrack; //增加IVR轨迹的弹屏

	std::string	m_strStation;
	std::string m_strAgentId;
	std::string m_strTime;

	//askproxy 解析asterisk pbx返回的消息
	BOOL    ParaResponseMsg(const std::string &strEvt);
	//对asterisk返回的消息进行编码，返回给软电话端

	std::string    EnSoftPhoneMsg() ;
};

#endif
