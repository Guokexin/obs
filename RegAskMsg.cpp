// RegAskMsg.cpp: implementation of the CRegAskMsg class.
//
//////////////////////////////////////////////////////////////////////
#include "RegAskMsg.h"
#include "GenCMD.h"
#include "GeneralUtils.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegAskMsg::CRegAskMsg()
{
	m_sRegActionID="";
}

CRegAskMsg::~CRegAskMsg()
{

}
/*

  Action: login
  Username: asterisk
  Secret: asterisk
  ActionID: 1234567abcdefg
  Events: off
*/
std::string CRegAskMsg::RegEnCode()
{
	CGenCMD cmd;
	cmd.Start();
	cmd.Gen("Action","login");
	cmd.Gen("Username","asterisk");
	cmd.Gen("Secret","asterisk");
	cmd.Gen("Events","on");
	cmd.End("login");

	//获取注册标识
 	m_sRegActionID=cmd.GetActionID();

	return cmd.m_strCMD;
	
//	cmd.SendCMD(&pdlg->m_SockManager);

}

void CRegAskMsg::UnRegEnCode()
{
	CGenCMD cmd;
	cmd.Start();
	cmd.Gen("Action","logoff");
	cmd.End("logoff",m_sRegActionID);//送出同一个ID

//	cmd.SendCMD(&pdlg->m_SockManager);
}
/*
Response: Success
ActionID: Reg|ec69bb06-be72-4bd5-b4e0-80de75522d25
Message: Authentication accepted
*/
/*
ActionID: Reg|96e4ef4f-06cc-44c3-b9bb-c3fa73aa75a4
Message: Authentication failed
*/


// BOOL CRegAskMsg::EvtMsg(CString sEvt)
// {
//    	CGeneralUtils  myGeneralUtils;
//
// 	char szResponse[128];
// 	memset(szResponse,0,128);
// 	myGeneralUtils.GetStringValue(sEvt,"Response",szResponse);
//
// 	if (strcmp(szResponse,_T("Success")) == 0)
// 	{
//          return TRUE;
// 	}
// 	else
// 	{
// 		char szMsg[512];
// 		memset(szMsg,0,512);
// 		myGeneralUtils.GetStringValue(sEvt,"Message",szMsg);
// 		pLog->Log(TEST_LOG,_T("%s"),szMsg);//not used
//
// 		return FALSE;
// 	}
//
//}
