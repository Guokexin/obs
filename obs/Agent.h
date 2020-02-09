#ifndef AGENT_H
#define AGENT_H
#include <iostream>
#include <string>
#include <list>
#include <map>
#include <ace/Date_Time.h>
#include <ace/Thread_Mutex.h>
//#include <asm/atomic.h>
#include "owntype.h"

class Server;
class CAskProxyDlg;

class CSocketManager;

typedef enum
{
	AS_Unknown = 0,				//登录失败
		AS_Notlogin,				//未登录状态
		AS_Login,					//登录状态
		AS_Idle,					//空闲状态
		AS_Notready,				//未就绪状态
		AS_Alerting,				//客户震铃状态
		AS_Dial,					//外拨号状态
		AS_DialAgent,				//内拨号状态//not used
		AS_Active,					//活动状态, 此时座席与客户之间通话已经建立
		AS_Preview,					//预览式外拨//not used
		AS_Hold,					//呼叫保持状态
		AS_Consulting,				//咨询状态//not used
		AS_Conferencing,			//会议状态
		AS_Manage,					//管理状态//not used
		AS_Monitor,					//监听状态 只表明监听方的状态//not used
		AS_HoldUp,					//拦截状态 只表明监听方的状态//not used
		AS_Observe,					//强插状态 相当于会议//not used
		AS_Wrap,					//后处理状态 此时不分配电话
		AS_Hangup,                  //挂断状态
		AS_Transfer					//转接状态-报表需要的假状态
} AGENT_CALL_STATUS;
//初始化进入的状态


typedef enum
{
    UNKNOWN_STATE,
	IDLE_STATE,
	BUSY_STATE,
	WRAPUP_STATE
}AGENT_INIT_STATE;

enum CONN_ID_STATE
{
    UNKNOWN_CONN_STATE,
    CONN_ACTIVE,
    CONN_HOLD,
    CONN_CONFERENCE
};

class   CONN_CHAN
{
public:
  CONN_CHAN()
  {
	  ResetCONN();
  }

  void ResetCONN()
  {
	  szOhterID = "";//对方的ChannelID;
	  szUCID = "";//对方号码
	  szANI= "";
	  szDNIS= "";
	  szDirection= "";
	  szState= "";
	  szStartTime= "";
	  szEndTime= "";
      isExist = false;
  }

  std::string szOhterID;//对方的ChannelID;
  std::string szUCID;//对方号码
  std::string szANI;
  std::string szDNIS;
  std::string szDirection;
  std::string szState;
  std::string szStartTime;
  std::string szEndTime;
  bool isExist;
};

struct ChanS
{
	std::string sAni;	
	std::string sDnis;
	std::string sHoldID;
};

class CAgent
{
public:
	CAgent();
	virtual ~CAgent();
	CAskProxyDlg* m_pDlg;

public:
    ACE_Thread_Mutex m_CriticalSection;
    int m_cRef;
    void lock();
    void unlock();
    int AddRef();
    void Release();

public:
	std::string sWorkNo;
	std::string sStation;
	std::string sPwd;


	AGENT_CALL_STATUS AgentState;
	std::string sSendResult_Data; //SendResult最新发出的信息
	int nLogoutTimeOutNum; //登出后等待心跳次数

	AGENT_INIT_STATE  AgentinitState;
	std::string sSipInterface;
	std::string sChan;
	std::string sQueueName;//技能组列表


	std::string sTimeout;
	std::string sContext;
	//Server *sock;
	int sock;



	CONN_CHAN  connChan[2];


	std::string sAni;
	std::string sDnis;
	std::string sTransNo;
	std::string sOtherNO;
	std::string sConsultNo;
	std::string sMointorNo;
	std::string sObserverNo;
	std::string sHoldupCallNo;


	std::string sCallDirect;
	std::string sHangupReason;
	std::string sTeantid;//租户ID


	
	std::string sSelfChanID;
	std::string sHoldID;
	std::string sHoldStation;//和HOLDID相关的号码
	std::string sOtherID;
	std::string sOtherIDStation;//和otherID 相关的号码
	ChanS sHoldS;//sHoldID的结构体类型


	std::string sUCID; //标识一路通话
	std::string sAgentStatus;

	std::string m_strAgentStatusReport;
	std::string m_strAgentCDRReport;

public:
	std::string m_strSource;	//客户端类型：手机/其他
	std::string m_strClientAddr;//客户端地址
	UINT m_nDefStatus;
	UINT m_HangupStatus;

	std::string m_callID;	//通话标识
	std::string m_recordID;	//录音标识
	std::string m_accessNumber;//接入码
	std::string m_ivrTrack;		//ivr号，等同接入码
	std::string m_callRingTime;	//振铃时间
	std::string m_callStartTime;//开始通话时间
	std::string m_callClearTime;	//通话结束时间

	std::string m_tempSkill;//2013-03-06 add by chenlin;存放座席所有技能组cdr
	struct liPopdata
	{
		std::string ani;
		std::string dnis;
		std::string callID;
		std::string ucid;
		std::string recordid;
		std::string accessNumber;
		std::string ivrTrack;
		std::string callRingTime;
		std::string callStartTime;
		std::string callClearTime;
	};

	std::list<liPopdata*> m_listPopData;

	std::string GetPopData();
	void savePopData();

	std::string sUnique;//
	std::string sUserData;
	std::string sActionID;
	std::string sAction;

	std::string sReasonCode;
	ACE_Date_Time preLiveTime;
	long int preLiveTimeSecond;

	std::string orginAni;
	std::string orginDnis;
	std::string orginCallDirect;
	std::string originUCID;
	std::string manageTime;

	BOOL    bIntoEstablish;

	BOOL    bLogin;  //座席是否登录在PBX中
    BOOL    bConsult;


	CSocketManager* m_pMainSock;

	std::map<std::string, std::string> m_MapQueueToIslogin;


	std::string m_strConfigCaller;//平台呼座席使用的主叫号码-配置台配置
	std::string m_strpstnAgentPrefix;//获取某一租户信息<pstnAgentPrefix>PSTN座席的前缀码</ pstnAgentPrefix >
	std::string m_sStationType;//分机类型: PSTN/SIP

	std::map<std::string, std::string> m_mapSkillT;
	std::map<std::string, std::string> m_mapSkillAndPenalty;
	int  IsFullSills();
	void  setSkillState(const std::string &queue, const std::string &val);
	int IsOutSkills();

    std::string m_strAllSkill;


public:
	void  reset_phone(); //重设话路数据
	void  reset_agent(); //重设座席对象数据

	void SetMainWnd(CAskProxyDlg* pDlg);
	void SetMainSocket(CSocketManager* pSocket);
	BOOL SendCMDToPBX(const std::string &sMsg);


	BOOL Login();
	BOOL LogOut();
	BOOL SetIdle();
	BOOL SetBusy();
	BOOL SetWrapup();

	BOOL MakeCall();
	BOOL Hangup();
	BOOL Hold();
	BOOL UnHold();
	BOOL ReleaseCall();
	BOOL SingleTrans();
	BOOL Consult();
	BOOL ConsultCancel();
	BOOL ConsultCancel_1();
	BOOL ConsultTrans();
	BOOL ConsultTrans_1();
	BOOL Conference();
	BOOL Confercece_1();
	BOOL Monitor();
	BOOL Observer();
	BOOL HoldupCall();
	BOOL EnforceLogOut(const std::string &sDnis, const std::string &sAgentId);
	BOOL SendDTMF(const std::string &digit);

	std::string GetUCIDFromPBX();

    //PSTN座席通话时系统将该变量的当前通话数设置为1；PSTN座席结束通话是系统将该变量的当前通话数设置为0
	BOOL LoginTalkData(); //PSTN座席login设置全局变量
	BOOL LoginSipTalkData();//sip座席login设置全局变量
	BOOL LogoutTalkData();
	BOOL LogoutSipTalkData();//sip座席logout置空全局变量
};

#endif
