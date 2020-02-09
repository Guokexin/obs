// MakeCallEvt.h: interface for the CMakeCallEvt class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MAKECALLEVT_H
#define MAKECALLEVT_H

#include <string>
#include "../owntype.h"

class CMakeCallEvt
{
public:
	CMakeCallEvt();
	virtual ~CMakeCallEvt();
public:
	std::string m_strStation;
	std::string m_strDnis;
	std::string m_strAni;
	std::string m_strDirector;
	std::string m_strUserData;
	std::string m_strUCID;
	std::string m_strAgentId;
	std::string m_strTime;

	std::string m_strRet;
	std::string m_strCause;
	std::string m_strEvt;

	//解析软电话消息
	BOOL ParaSoftPhoneRequestMsg(const std::string &sDoc);
	//将请求结果返回给软电话
	std::string EnSoftPhoneMsg();
};

#endif
