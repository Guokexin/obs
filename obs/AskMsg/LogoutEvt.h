// LogoutEvt.h: interface for the CLogoutEvt class.
//
//////////////////////////////////////////////////////////////////////

#ifndef LOGOUTEVT_H
#define LOGOUTEVT_H

#include <string>
#include "../owntype.h"

class CLogoutEvt
{
public:
	CLogoutEvt();
	virtual ~CLogoutEvt();

public:
	//解析软电话消息
	BOOL ParaSoftPhoneRequestMsg(const std::string &sDoc);
	//解析PBX返回的事件
	BOOL ParaRetPBXEvt(const std::string &sDoc);
	//将请求结果返回给软电话
	std::string EnSoftPhoneMsg();

public:
	std::string m_strStation;
	std::string m_strReason;
	std::string m_strUserData;
	std::string m_strOperation;
	std::string m_strAgentId;
	std::string m_strTime;
public:
	std::string m_strRet;   //结果
	std::string m_strCause; //失败原因
	std::string m_slogoutqueue; //成功签出技能组号码

};

#endif
