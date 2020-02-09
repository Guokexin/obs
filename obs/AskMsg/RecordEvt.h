// RecordEvt.h: interface for the CRecordEvt class.
//
//////////////////////////////////////////////////////////////////////

#ifndef RECORDEVT_H
#define RECORDEVT_H

#include <string>
#include "../owntype.h"

class CRecordEvt
{
public:
	CRecordEvt();
	virtual ~CRecordEvt();
public:
	std::string m_strUCID;
	std::string m_strStation;
	std::string m_strAgentId;
	std::string m_strTime;
	std::string m_strRecordID;

	std::string EnSoftPhoneMsg();

};

#endif
