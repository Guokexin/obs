// ConfEvt.h: interface for the CConfEvt class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CONFEVT_H
#define CONFEVT_H

#include <string>
#include "../owntype.h"


class CConfEvt
{
public:
	CConfEvt();
	virtual ~CConfEvt();

	std::string m_strStation;
	std::string m_strUserData;
	std::string m_strRet;
	std::string m_strEvt;
	std::string m_strChan;

	std::string m_strAgentId;
	std::string m_strTime;

	BOOL ParaSoftPhoneRequestMsg(const std::string &strDoc);

// 	void EnCode(_AgentS* pAgent);
//
	BOOL ParaResponseMsg(const std::string &strEvt);

	std::string EnSoftPhoneMsg();
};

#endif
