// EnforceLogOutEvt.h: interface for the CEnforceLogOutEvt class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ENFORCELOGOUTEVT_H
#define ENFORCELOGOUTEVT_H

#include <string>
#include "../owntype.h"

class CEnforceLogOutEvt
{
public:
	CEnforceLogOutEvt();
	virtual ~CEnforceLogOutEvt();

	BOOL ParaSoftPhoneRequestMsg(const std::string &strDoc);

	std::string    EnSoftPhoneMsg();

	std::string m_strStation;
	std::string m_strLogoutAgentId;
	std::string m_strLogoutStation;
	std::string m_strUserData;
};

#endif
