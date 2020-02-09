#ifndef HANDLEASKPBX_H
#define HANDLEASKPBX_H

class CAgent;
class CAskProxyDlg;
class CQueueStatic;
class CQueueCallInfo;



#include <iostream>
#include <string>
#include <list>
#include <cstring>
//#include <semaphore.h>
#include <ace/Thread_Semaphore.h>
#include <ace/Thread.h>
#include <ace/Thread_Manager.h>
#include "AskMsg/QueueEvt.h"
#include "GeneralUtils.h"
#include "owntype.h"

const size_t PBXTHREADNUM = 5;


class CHandleAskPBX
{
public:
	CHandleAskPBX();
	virtual ~CHandleAskPBX();
public:
//	HANDLE m_hStartAskPBX;
//	HANDLE m_hStopAskPBX;

//	HANDLE m_hStartDispatchPBX;
//	HANDLE m_hStopDispatchPBX;

//	CWinThread* m_lpHandleAskPBX;
//	CWinThread* m_lpDispatchAskPBX;
//    sem_t m_AskMsgRun;
    ACE_Thread_Semaphore m_AskMsgRun;
    volatile bool m_AskMsgStop;
    ACE_thread_t  m_HandAskPBXID[PBXTHREADNUM];
    int           m_HandAskPBXGrp;
    ACE_Thread_Semaphore m_hSerialEvent;
//    pthread_mutex_t m_AskMsgCritical;//临界区
    ACE_Thread_Mutex  m_AskMsgCritical;//临界区

    volatile bool m_DispatchStop;
//    sem_t m_DispatchRun;
    ACE_Thread_Semaphore m_DispatchRun;
    ACE_thread_t  m_DispatchAskPBXID;

	std::list<std::string> m_AskMsgList;
	CAskProxyDlg* m_pDlg;


public:
	BOOL    StartHandleAskPBX(CAskProxyDlg* pDlg);
	BOOL    StopHandleAskPBX();

	static void*   HandleAskPBX(LPVOID lpvoid);
	static void*   DispatchAskPBX(LPVOID lpvoid);

	void    RunHandleAskPBX();
	void    ProcessEvt(const std::string &strEvt);
	void    RunDispatchAskPBX();
	std::string m_strEvtPBX;//用于存储上一次获取的粘包数据
public:
	//处理坐席状态事件
	BOOL    HandleQueueMemberPausedEvt(const std::string &strEvt);
    //处理挂机事件
	BOOL    HandleHangupEvt(const std::string &strEvt);
	//OriginateResponse 事件
	BOOL    HandleOriginateResponseEvt(const std::string &strEvt);
	//处理NewState事件 Ringing
	BOOL    HandleNewstateEvt(const std::string &strEvt);
	//处理QueueMemberStatus事件
	BOOL    HandQueueMemberStatusEvt(const std::string &strEvt);
	//处理拨号事件
	BOOL    HandleDialEvt(const std::string &strEvt);
	//处理通话建立事件
	BOOL    HandleEstablishEvt(const std::string &strEvt);
	//处理会议事件
	BOOL    HandleConfEvt(const std::string &strEvt);
	//处理签入事件
	BOOL    HandleLoginEvt(const std::string &strEvt);
	//处理签出事件
	BOOL    HandleLogoutEvt(const std::string &strEvt);
//	    BOOL Islogoutallqueue(CAgent* pAgent);
	//
	BOOL    HandleQueueStaticEvt(const std::string &sEvt);
	//处理Join Queue事件
	BOOL    HandleJoinEvt(const std::string &sEvt);
	BOOL    HandleLeaveEvt(const std::string &sEvt);
	//处理UserEvent接口
	BOOL    HandleUserEvent(const std::string &sEvt);
	//处理GetVal的接口
	BOOL    HandleUCIDEvt(const std::string &sEvt);

	BOOL    HandleIVRLogout(const std::string &sEvt);

	std::string     GetStatusNameFromCode(int callStatus);//enum状态转换成CString
	BOOL        SetAgentStatusReportData(CAgent* pAgent);//agentStatus|

public:
	BOOL   HandleResponse(const std::string &strEvt);
    BOOL   SetWSCDRData(CAgent* pAgent);
	BOOL   SetWSSkillSnap(CQueueEvt* pQueue, const std::string &sTenantID);
};


#endif
