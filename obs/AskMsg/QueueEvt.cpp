#include "QueueEvt.h"
#include "../GenCMD.h"
#include "../GeneralUtils.h"
#include "../UntiTool.h"

CQueueEvt::CQueueEvt()
{

}

CQueueEvt::~CQueueEvt()
{

}

std::string CQueueEvt::CmdQueue()
{
	CGenCMD cmd;
	cmd.Start();
	cmd.Gen("Action","QueueStatus");
	CUntiTool tool;
	std::string sUCID = tool.GenUCID();
	cmd.End(sUCID);

	return cmd.GetCmd();
}

BOOL CQueueEvt::ParaQueueParams(const std::string &sEvt)
{
	CGeneralUtils myGeneral;

	char szQueue[32];
	memset(szQueue,0,32);
	myGeneral.GetStringValue(sEvt,"Queue",szQueue);

	char szWaitCalls[32];
	memset(szWaitCalls,0,32);
	myGeneral.GetStringValue(sEvt,"Calls",szWaitCalls);


	char szCompleteCalls[32];
	memset(szCompleteCalls,0,32);
	myGeneral.GetStringValue(sEvt,"Completed",szCompleteCalls);

	char szDiscardCalls[32];
	memset(szDiscardCalls,0,32);
	myGeneral.GetStringValue(sEvt,"Abandoned",szDiscardCalls);

	char szActiveCalls[32];
	memset(szActiveCalls,0,32);
	myGeneral.GetStringValue(sEvt,"ActiveCallCounter",szActiveCalls);

	//还少一个字段，需要加入。
	char szagentAvailableCount[32];
	memset(szagentAvailableCount,0,32);
	myGeneral.GetStringValue(sEvt,"AvailableMembers",szagentAvailableCount);

	char szPausedMembers[32];
	memset(szPausedMembers,0,32);
	myGeneral.GetStringValue(sEvt,"PausedMembers",szPausedMembers);


    sQueue = szQueue;
	sWaitCalls = szWaitCalls;
	sCompleteCalls = szCompleteCalls;
	sDiscardCalls = szDiscardCalls;
	sActiveCalls = szActiveCalls;
	sagentAvailableCount = szagentAvailableCount;
	spausedMembers = szPausedMembers;

	return TRUE;

}

