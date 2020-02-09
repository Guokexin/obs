// WrapupEvt.h: interface for the CWrapupEvt class.
//
//////////////////////////////////////////////////////////////////////

#ifndef WRAPUPEVT_H
#define WRAPUPEVT_H

#include <string>
#include "../owntype.h"

class CWrapupEvt
{
public:
	CWrapupEvt();
	virtual ~CWrapupEvt();
public:
	//解析软电话消息
	BOOL ParaSoftPhoneRequestMsg(const std::string &sDoc);
	//将请求结果返回给软电话
	std::string EnSoftPhoneMsg();
public:
	std::string m_strStation;
	std::string m_strUserData;
	std::string m_strRet;
	std::string m_strCause;
	std::string m_strReason;
	std::string m_strEvt;

    std::string m_strAgentId;
	std::string m_strTime;
};

#endif // !defined(AFX_WRAPUPEVT_H__DB3F0C61_747D_40E1_8F00_443BD5638F02__INCLUDED_)
