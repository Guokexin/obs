#ifndef AGENTOP_H
#define AGENTOP_H

#include <string>
#include "owntype.h"
#include "Agent.h"

class CAgentOp
{
public:
	CAgentOp();
	virtual ~CAgentOp();
public:
	BOOL SendResult(CAgent* pAgent, const std::string &sMsg);

	std::string AddElemToResult(const std::string &sMsg, const std::string &selem_name, const std::string &selem_value);
};

#endif
