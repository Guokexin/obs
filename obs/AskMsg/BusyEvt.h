// BusyEvt.h: interface for the CBusyEvt class.
//
//////////////////////////////////////////////////////////////////////

#ifndef BUSYEVT_H
#define BUSYEVT_H

#include <string>
#include "../owntype.h"


class CBusyEvt
{
public:
	CBusyEvt();
	virtual ~CBusyEvt();
public:

	//解析软电话消息
	BOOL ParaSoftPhoneRequestMsg(const std::string &sDoc);
	//将请求结果返回给软电话
	std::string EnSoftPhoneMsg();


	std::string m_strStation;
	std::string m_strUserData;
	std::string m_strReason;
	std::string m_strRet;
	std::string m_strCause;
	std::string m_strEvt;

	std::string m_strAgentId;
	std::string m_strTime;
};

#endif
