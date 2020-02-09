// UnHoldEvt.h: interface for the CUnHoldEvt class.
//
//////////////////////////////////////////////////////////////////////

#ifndef UNHOLDEVT_H
#define UNHOLDEVT_H

#include <string>
#include "../owntype.h"

class CUnHoldEvt
{
public:
	CUnHoldEvt();
	virtual ~CUnHoldEvt();

	std::string m_strStation;
	std::string m_strRet;
	std::string m_strUserData;

	BOOL ParaSoftPhoneRequestMsg(const std::string &strDoc);
};

#endif
