#ifndef QUEUEEVT_H
#define QUEUEEVT_H

#include <string>
#include "../owntype.h"

typedef struct
{
	std::string sWaitCalls;
	std::string sActiveCalls;
	std::string sCompleteCalls;
	std::string sDiscardCalls;

}S_QueueCalls;


class CQueueEvt
{
public:
	CQueueEvt();
	virtual ~CQueueEvt();

	std::string CmdQueue();
	BOOL ParaQueueParams(const std::string &sEvt);
//	BOOL ParaQueueMember(const std::string &sEvt);

	std::string sQueue;
	std::string sWaitCalls;
	std::string sActiveCalls;
	std::string sCompleteCalls;
	std::string sDiscardCalls;
	std::string sagentAvailableCount;
	std::string spausedMembers;
};

#endif
