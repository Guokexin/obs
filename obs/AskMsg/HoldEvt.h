// HoldEvt.h: interface for the CHoldEvt class.
//
//////////////////////////////////////////////////////////////////////

#ifndef HOLEEVT_H
#define HOLEEVT_H

#include <string>
#include "../owntype.h"

class CHoldEvt
{
public:
	CHoldEvt();
	virtual ~CHoldEvt();

	std::string m_strChan;
	std::string m_strStation;
	std::string m_strRet;


	std::string m_strUserData;
	std::string m_strAgentId;
	std::string m_strTime;
	std::string m_strUCID;

	//askproxy 解析softphone消息
	BOOL ParaSoftPhoneRequestMsg(const std::string &sDoc);

	//askproxy 解析asterisk pbx返回的消息
	BOOL    ParaResponseMsg(const std::string &strEvt);

	//对asterisk返回的消息进行编码，返回给软电话端
	std::string    EnSoftPhoneMsg();

};

#endif
