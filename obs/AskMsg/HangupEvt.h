// HangupEvt.h: interface for the CHangupEvt class.
//
//////////////////////////////////////////////////////////////////////

#ifndef HANGUPEVT_H
#define HANGUPEVT_H

#include <string>
#include "../owntype.h"

class CHangupEvt
{
public:
	CHangupEvt();
	virtual ~CHangupEvt();

public:
	std::string m_strStation;
	std::string m_strReason;
	std::string m_strUserData;
	std::string m_strCause;

	std::string m_strSource;
	std::string m_strResponse;

	std::string m_strAgentId;
	std::string m_strTime;

	std::string m_strEvt;
	std::string m_strUCID;

	//解析软电话消息
	BOOL ParaSoftPhoneRequestMsg(const std::string &sDoc);
	//将请求结果返回给软电话
	std::string EnSoftPhoneMsg();



};

#endif // !defined(AFX_HANGUPEVT_H__BD27ED14_9351_42E0_BC1C_37B1A1F7D7DF__INCLUDED_)
