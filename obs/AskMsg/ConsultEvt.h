// ConsultEvt.h: interface for the CConsultEvt class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CONSULTEVT_H
#define CONSULTEVT_H

#include <string>
#include "../owntype.h"


class CConsultEvt
{
public:
	CConsultEvt();
	virtual ~CConsultEvt();

public:

	std::string m_strStation;

	std::string  m_strChannel1;
	std::string  m_strChannel2;

	std::string  m_strUnqueid1;
	std::string  m_strUnqueid2;

	std::string  m_strAni;
	std::string  m_strDnis;

	std::string m_strUserData;

	std::string m_strEvt;


	BOOL ParaSoftPhoneRequestMsg(const std::string &strDoc);


	std::string    EnSoftPhoneMsg();

};

#endif
