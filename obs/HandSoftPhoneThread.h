#ifndef HANDSOFTPHONETHREAD_H
#define HANDSOFTPHONETHREAD_H

#include "owntype.h"
#include <ace/Thread_Manager.h>
#include <ace/Thread_Semaphore.h>
#include <string>
#include "Phone.h"
#include <map>
class CAskProxyDlg;

class CHandSoftPhoneThread
{
public:
	CHandSoftPhoneThread();
	virtual ~CHandSoftPhoneThread();
public:
	
	void ParaMsg(std::string &msg);

	ACE_Thread_Semaphore  m_hStartHandleSoftPhoneMsg;
	volatile bool m_hStopHandleSoftPhoneMsg;
	ACE_thread_t m_pHandleSoftPhoneThread;

	CAskProxyDlg* m_pDlg;

	BOOL    StartHandleSoftPhoneMsg(CAskProxyDlg* pDlg);
	static  LPVOID HandleSoftPhoneMsg(LPVOID lpvoid);
	void     RunHandleSoftPhoeMsg();
	BOOL     StopHandleSoftPhoneMsg();


   	ACE_Thread_Mutex m_phoneMapCritical;
    std::map<std::string,CPhone*>  m_phoneMap;


	ACE_Thread_Semaphore  m_hStartDispatchSoftPhoneMsg;
	volatile bool m_hStopDispatchSoftPhoneMsg;
	ACE_thread_t m_pDispatchSoftPhoneThread;

	BOOL    StartDispatchSoftPhoneMsg(CAskProxyDlg* pDlg);
	static  LPVOID HandleDispatchSoftPhoneMsg(LPVOID lpvoid);
	void     RunDispatchSoftPhoeMsg();
	BOOL     StopDispatchSoftPhoneMsg();



	BOOL  HandleAgentLogin(const std::string &strMsg,int s);
	BOOL  HandleAgentLogOut(const std::string &strMsg,int s);
	BOOL  HandleAgentIdle(const std::string &strMsg,int s);
	BOOL  HandleAgentBusy(const std::string &strMsg,int s);
	BOOL  HandleAgentWrapup(const std::string &strMsg,int s);
	BOOL  HandleAgentCompleteWrapup(const std::string &strMsg,int s);
	BOOL  HandleAgentMakeCall(const std::string &strMsg,int s);
	BOOL  HandleAgentConsult(const std::string &strMsg,int s);
	BOOL  HandleAgentConsultTrans(const std::string &strMsg,int s);
	BOOL  HandleAgentConsultCancel(const std::string &strMsg,int s);
	BOOL  HandleAgentConsultConference(const std::string &strMsg,int s);
	BOOL  HandleAgentSingleTrans(const std::string &strMsg,int s);
	BOOL  HandleAgentSingleConference(const std::string &strMsg,int s);
	BOOL  HandleAgentAnswer(const std::string &strMsg,int s);
	BOOL  HandleAgentHold(const std::string &strMsg,int s);
	BOOL  HandleAgentUnHold(const std::string &strMsg,int s);
	BOOL  HandleAgentHangup(const std::string &strMsg,int s);
	BOOL  HandleAgentEnforceIdle(const std::string &strMsg,int s);
	BOOL  HandleAgentEnforceBusy(const std::string &strMsg,int s);
	BOOL  HandleAgentEnforceLogOut(const std::string &strMsg,int s);
	BOOL  HandleAgentMonitor(const std::string &strMsg,int s);
	BOOL  HandleAgentObserverCall(const std::string &strMsg,int s);
	BOOL  HandleAgentEnforceHangup(const std::string &strMsg,int s);
	BOOL  HandleAgentHoldupCall(const std::string &strMsg,int s);
	BOOL  HandleAgentSendDTMF(const std::string &strMsg,int s);
	BOOL  HandleGetPopData(const std::string &strMsg,int s);
	BOOL  HandleUpdateSocket(int s);

private:
    void  CloseConn(const std::string &strMsg, int s);

public:
    int m_webSock;
};

#endif
