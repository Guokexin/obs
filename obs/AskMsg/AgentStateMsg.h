// AgentStateMsg.h: interface for the CAgentStateMsg class.
//
//////////////////////////////////////////////////////////////////////

#ifndef AGENTSTATEMSG_H
#define AGENTSTATEMSG_H

#include <string>
#include "../owntype.h"


class CAgentStateMsg
{
public:
	CAgentStateMsg();
	virtual ~CAgentStateMsg();
public:
	std::string EnEvt(const std::string &sState);
	void ParaMsg(const std::string &sDoc);

	std::string m_strSource;
	std::string m_strResponse;
	std::string m_strAgentState;
};

#endif
