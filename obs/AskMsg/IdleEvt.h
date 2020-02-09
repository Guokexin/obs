// IdleEvt.h: interface for the CIdleEvt class.
//
//////////////////////////////////////////////////////////////////////

#ifndef IDLEEVT_H
#define IDLEEVT_H

#include <string>
#include "../owntype.h"
#include "../Agent.h"

class CIdleEvt
{
public:
	CIdleEvt();
	virtual ~CIdleEvt();

public:
	//解析软电话消息
	BOOL ParaSoftPhoneRequestMsg(const std::string &sDoc);
	//对PBX发送命令信息
//	BOOL EnCodePBXMsg(CAgent* pAgent);
	//解析PBX返回的事件
	BOOL ParaRetPBXEvt(const std::string &sDoc);
	//将请求结果返回给软电话
	std::string EnSoftPhoneMsg();

public:
	std::string m_strStation;
	std::string m_strUserData;
	std::string m_strRet;
	std::string m_strCause;
	std::string m_strEvt;
    std::string m_strAgentId;
	std::string m_strTime;

};

#endif
