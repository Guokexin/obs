#ifndef ASKPROXYDLG_H
#define ASKPROXYDLG_H

#include <iostream>
#include <string>
#include <list>
#include <map>
#include <log4cxx/logger.h>
#include <log4cxx/logstring.h>
#include <log4cxx/propertyconfigurator.h>
#include "logclass/log.h"
#include "HandleAskPBX.h"
#include "socket/SocketManager.h"
#include "Agent.h"
#include "SettingData.h"
#include "AskMsg/RegAskMsg.h"
#include "MsgQueue.h"
#include "HandleWebResponse.h"
#include "HandleWS.h"
#include "CompletePort.h"
#include "HandSoftPhoneThread.h"
#include "CheckAgent.h"

typedef unsigned int UINT;
typedef int BOOL;

struct AgentMsg
{
	public:
//		CStdString sMsg;
        std::string sMsg;
		int s;
	AgentMsg():sMsg(""),s(-1)

	{

	}
};

typedef std::list<AgentMsg*> CAgentMsgList;
//

struct PBXInfo//skill_snap
{
    std::string ssipSkillName;//技能组ID
    std::string stenantId;//租户ID
};
typedef std::list<PBXInfo*> CPBXInfoList;


class CAskProxyDlg
{
public:
    CAskProxyDlg();

public:
    int     m_iGetQueueTime;
    BOOL	m_bCheckLog;
    //使用的对象及其说明
    CHandleAskPBX   m_HandleAskPBX; //与PBX交互
    //Send:CAgent类调函动作数据  Recv:PBX返回事件数据，通过CAgentOp类发生给座席端
    CSocketManager m_SockManager;

    ACE_Thread_Mutex     m_PBXMsgCritical;
    std::list<std::string> m_PBXMsgList; //PBX返回事件数据 to m_HandleAskPBX.m_AskMsgList

    //m_SockManager: 组串CQueueEvt类
    CHandleWS           m_HandleWSQuery; //每5分钟调用PBX接口(为了获取shillSnap信息)

    CRegAskMsg m_RegAction; //建立和断开与PBX交互的通道

    CCheckAgent m_CheckAgent;

    CHandSoftPhoneThread m_HandleSoftPhoneThread; //与座席端交互
    //调函操作: CAgent类中实现 use m_SockManager与PBX交互
    //发送数据给座席端
    //Send:m_HandleAskPBX对象的状态信息和CAgentOp类处理PBX返回事件数据  Recv:null
    CCompletePort  m_IOOpt;
    //获取座席端数据
    //Send:null  Recv:调函动作数据—— GetQueuedCompletionStatus API实现
    ACE_Thread_Mutex   m_SoftPhoneMsgCritical;//临界区
    CAgentMsgList       m_AgentMsgList; //调函动作数据

    CHandleWebResponse  m_HandleWS; //与AskProxyTOCM交互
    //发生数据给AskProxyTOCM
    //Send:skillSnap agentStatus skillSnap skillRecord cdr  Recv:null
    CSocketManager m_pSendSocket; //更改成指针形式
    CMsgQueue           m_WSQueue; //发送给AskProxyTOCM数据



    //读取配置
    BOOL      ReadWSCfg(const std::string &sPath);
    CSettingData  m_SettingData;
    std::string   m_strAgentStatusWeb;
    std::string   m_strCdrWeb;
    std::string   m_strskillRecordWeb;
    std::string   m_strskillSnapWeb;
    std::string   m_strAgentActualTimeStatus;
    std::string   m_strResportServer; //报表AskProxyTOCM的IP
    std::string   m_strResportPort; //报表AskProxyTOCM的Port
    std::string   m_strWSConn;
    std::string   m_strIsStationLogin;//分机是否允许登录的http验证串
    std::string   m_strdialAgentCallerNumber;//通过租户id获取平台拨座席的主叫号码
    std::string   m_strOverTime;//超时设置

    std::string m_strPBXInfo;//获取所有的租户和技能组
    std::string	m_strWSSipExt;
    std::string m_strGetStationType;//获取分机类型 sip skill agent others(pstn)
    std::string m_strConnAgentPwd;
    //需要将CString配置内容放入配置类CSettingData里面

    //skill_snap

    ACE_Thread_Mutex m_PBXInfoCritical;
    CPBXInfoList m_PBXInfoList; //存储所有的技能组和租户

    ACE_Thread_Mutex m_obsCountCtitical;
    int              m_obsCount;

    ACE_Thread_Mutex m_phoneMapCritical;
    std::map<std::string,CPhone*>  m_phoneMap;
public:

    ACE_Thread_Mutex m_MapQueueCallLock;
    std::map<std::string, CQueueCallInfo*> m_MapQueueCall; //HandleJoinEvt

    ACE_Thread_Mutex  m_AgentLock;
    void   SetLock(int TimeSpan=3000);
    void   SetUnLock();

    ACE_Thread_Mutex  m_MapStationToAgentLock;
    std::map<std::string, CAgent*> m_MapStationToAgent; //new CAgent
    CAgent*   GetAgentFromStation(const std::string &sStation);//用于运行
    void   SetAgentToStation(const std::string &sStation);//用于登录过程中

    ACE_Thread_Mutex m_mapChanToAgentLock;
    std::map<std::string, CAgent*> m_MapChanToAgent; //Map表  呼入呼出(签出的时候delete pAgent)
    CAgent*   GetAgentFromChan(const std::string &sChan);//根据CHAN的值返回Agent
    void SetAgentToChan(const char *szChan, CAgent* pAgent);
    BOOL RemoveChanFromMap(const char *szChan, CAgent* pAgent);

    ACE_Thread_Mutex m_MapWorkNoToStationLock;
    std::map<std::string, std::string> m_MapWorkNoToStation; //成功签入中的座席的Map
    BOOL      SetWorkNoToMap(CAgent* pAgent);  //PBX返回登录成功
    BOOL      AgentIsLogin(const std::string &sagentid, std::string& sStation); //判断分机是否被签入

    BOOL      RemoveAgentFromMap(CAgent* pAgent);

    //日志文件类
    CLog m_ReportLog;//记录报表数据
    CLog m_Log;//程序日志类
    CLog m_PBXLog;//Askrisk返回数据
    CLog m_keepLive;
    CLog m_AgentLog;
    CLog m_skillSnapLog;

    //
    BOOL m_bIsSopping;//程序是否正在结束
public:
    bool OnStart();

    void StartLogin();
    static void* RegisterLogin(void *par);
};

#endif
