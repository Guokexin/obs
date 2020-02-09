// SingleTransEvt.h: interface for the CSingleTransEvt class.
//
//////////////////////////////////////////////////////////////////////

#ifndef SINGLETRANSEVT_H
#define SINGLETRANSEVT_H

#include <string>
#include "../owntype.h"

class CSingleTransEvt
{
public:

	std::string m_strStation;
	std::string m_strTransNo;
	std::string m_strUserData;


	CSingleTransEvt();
	virtual ~CSingleTransEvt();

	BOOL ParaSoftPhoneRequestMsg(const std::string &strDoc);



};

#endif
