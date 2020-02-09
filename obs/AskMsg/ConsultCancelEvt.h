// ConsultCancelEvt.h: interface for the CConsultCancelEvt class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CONSULTCANCELEVT_H
#define CONSULTCANCELEVT_H

#include <string>
#include "../owntype.h"


class CConsultCancelEvt
{
public:
	CConsultCancelEvt();
	virtual ~CConsultCancelEvt();

	std::string m_strStation;
	std::string m_strUserData;

	BOOL ParaSoftPhoneRequestMsg(const std::string &sDoc);
};

#endif
