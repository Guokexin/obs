#ifndef HANDLEASKPBX_H
#define HANDLEASKPBX_H

class CPhone;
#include <iostream>
#include <string>
#include <list>
#include <cstring>
#include <ace/Thread_Semaphore.h>
#include <ace/Thread.h>
#include <ace/Thread_Manager.h>
#include "GeneralUtils.h"
#include "owntype.h"

const size_t PBXTHREADNUM = 5;


class CHandleAskPBX
{
private:
	static CHandleAskPBX m_singleton;
public:
	static CHandleAskPBX* Instance();
private:
	CHandleAskPBX();
public:
	virtual ~CHandleAskPBX();
public:
    ACE_Thread_Semaphore m_AskMsgRun;
    volatile bool m_AskMsgStop;
    ACE_thread_t  m_HandAskPBXID[PBXTHREADNUM];
    int           m_HandAskPBXGrp;
    ACE_Thread_Semaphore m_hSerialEvent;
    ACE_Thread_Mutex  m_AskMsgCritical;//临界区

    volatile bool m_DispatchStop;
    ACE_Thread_Semaphore m_DispatchRun;
    ACE_thread_t  m_DispatchAskPBXID;
	std::list<std::string> m_AskMsgList;
	//pbx msg list
    ACE_Thread_Mutex     m_PBXMsgCritical;
    std::list<std::string> m_PBXMsgList; //PBX返回事件数据 to m_HandleAskPBX.m_AskMsgList
public:
	BOOL    StartHandleAskPBX();
	BOOL    StopHandleAskPBX();

	static void*   HandleAskPBX(LPVOID lpvoid);
	static void*   DispatchAskPBX(LPVOID lpvoid);

	void    RunHandleAskPBX();
	void    ProcessEvt(const std::string &strEvt);
	void    RunDispatchAskPBX();
	std::string m_strEvtPBX;//用于存储上一次获取的粘包数据
public:
    //处理挂机事件
	BOOL    HandleHangupEvt(const std::string &strEvt);
	BOOL 	HandleDialEvt(const std::string &strEvt);
	BOOL 	HandleEstablishEvt(const std::string &strEvt);
    BOOL    HandleNewstateEvt(const std::string &strEvt);
	//OriginateResponse 事件
	BOOL    HandleOriginateResponseEvt(const std::string &strEvt);
public:
	BOOL   HandleResponse(const std::string &strEvt);
};


#endif
