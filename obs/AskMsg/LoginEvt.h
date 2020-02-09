// LoginEvt.h: interface for the CLoginEvt class.
//
//////////////////////////////////////////////////////////////////////
#ifndef LOGINEVT_H
#define LOGINEVT_H

#include <string>
#include "../owntype.h"
#include "../Agent.h"

class CLoginEvt
{
public:
	CLoginEvt();
	virtual ~CLoginEvt();
public:
	//解析软电话消息
	BOOL ParaSoftPhoneRequestMsg(const std::string &sDoc);
	//对PBX发送命令信息
//	BOOL EnCodePBXMsg(CAgent* pAgent);
	//解析PBX返回的事件
	BOOL ParaRetPBXEvt(const std::string &sDoc);
	//将请求结果返回给软电话
	std::string EnSoftPhoneMsg();
public:
	std::string m_strWorkNo;
	std::string m_strStation;
	std::string m_strPwd;
	std::string m_strUserData;
	std::string m_strMemberName;

	std::string m_strAgentId;
	std::string m_strTime;

	std::string m_strClientAddr;
	UINT m_nDefStatus;
	std::string m_strSource;   //2013-01-07 add by chenlin
	UINT m_HangupStatus;

	std::string m_strSkills;
	std::string m_strLoginType;
	std::string m_strViewCaller;

	BOOL m_bIsAgentReConn; //20121025 used to reconnect
	std::string m_sSoftPhoneVersion;//软电话的版本号，默认为1.0.0.1

	CAskProxyDlg* m_pDlg;


public:
	std::string m_sloginqueue;//login 技能组
	std::string m_strRet;   //登录结果
	std::string m_strCause; //登录失败原因
};

#endif
