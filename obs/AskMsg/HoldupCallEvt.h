// HoldupCallEvt.h: interface for the CHoldupCallEvt class.
//
//////////////////////////////////////////////////////////////////////

#ifndef HOLDUPCALLEVT_H
#define HOLDUPCALLEVT_H

#include <string>
#include "../owntype.h"

class CHoldupCallEvt
{
public:
	CHoldupCallEvt();
	virtual ~CHoldupCallEvt();

	std::string m_strHoldupNo;
	std::string m_strStation;
	std::string m_strUserData;
	std::string m_strRet;
	std::string m_strCause;

	std::string m_strChan1;
	std::string m_strChan2;
	std::string m_strMyChan;

	//解析软电话消息
	BOOL ParaSoftPhoneRequestMsg(const std::string &sDoc);
	//将请求结果返回给软电话
	std::string EnSoftPhoneMsg();
	//解析Monitor PBX事件
	BOOL    ParaRetPBXEvt(const std::string &strEvt);

};

#endif
