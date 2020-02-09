// ConsultTransEvt.h: interface for the CConsultTransEvt class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CONSULTTRANSEVT_H
#define CONSULTTRANSEVT_H

#include <string>
#include "../owntype.h"

class CConsultTransEvt
{
public:
	CConsultTransEvt();
	virtual ~CConsultTransEvt();


	std::string m_strStation;
	std::string m_strUserData;

	BOOL ParaSoftPhoneRequestMsg(const std::string &sDoc);


};

#endif
