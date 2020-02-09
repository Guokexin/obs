// MonitorEvt.h: interface for the CMonitorEvt class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MONITOREVT_H
#define MONITOREVT_H

#include <string>
#include "../owntype.h"

class CMonitorEvt
{
public:
	CMonitorEvt();
	virtual ~CMonitorEvt();

	std::string m_strMonitorNo;
	std::string m_strStation;
	std::string m_strUserData;
	std::string m_strRet;
	std::string m_strCause;

	std::string m_strUCID;
	std::string m_strAgentId;
	std::string m_strTime;

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
