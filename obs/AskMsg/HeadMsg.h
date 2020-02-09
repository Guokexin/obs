// HeadMsg.h: interface for the CHeadMsg class.
//
//////////////////////////////////////////////////////////////////////

#ifndef HEADMSG_H
#define HEADMSG_H

#include <string>
#include "../owntype.h"

class CHeadMsg
{
public:
	CHeadMsg();
	virtual ~CHeadMsg();
public:
	std::string m_strSource;
	std::string m_strAction;

	std::string m_strResponse;
	std::string m_strEvt;

	BOOL ParaMsg(const std::string &sDoc);

	BOOL ParaProxyMsg(const std::string &sDoc);


};

#endif
