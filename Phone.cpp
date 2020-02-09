#include "Phone.h"
#include "GenCMD.h"
//#include "socket/SocketManager.h"
#include "obsCfg.h"
#include "logManager.h"
#include "SocketManager.h"
CPhone::CPhone()
{
	status = "0";
	isSucc = FALSE;
	
}

CPhone::~CPhone()
{

}


void CPhone::MakeCall()
{
	CLogManager *pLog = CLogManager::Instance();
	std::string gateWay = COBSCfg::Instance()->m_sGateWay;
  	
	if(gateWay.empty())
	{
		sChan = "SIP/"+no;
	}
	else
	{
		sChan = "SIP/"+std::string("0")+no+"@"+gateWay;//+m_pDlg->m_SettingData.m_strInPBX;
	}
  
 // sChan = "SIP/46018611380324@ims";
	char sHandle[256];
	memset(sHandle,0,256);
	sprintf(sHandle,"%d",fd);
	std::string val;
	val = "__phoneno="+no+",";
	val += "__ucid="+ucid+",";
	val += "__sock="+std::string(sHandle)+",";
	val += "__voicefile="+voicefile+",";
	val += "__cmd="+cmd+",";
        val += "__project_id"+project_id+",";
        val += "__name="+name;
	pLog->m_rootLog.Log("channel variables \r\n  %s", val.c_str());

	sTimeout="60000";
	CGenCMD cmd;
	cmd.Start();
	cmd.Gen("Action","Originate");
	cmd.Gen("Channel",sChan);
	cmd.Gen("Timeout",sTimeout);
	cmd.Gen("Callerid",caller);
	cmd.Gen("Context","default");
	cmd.Gen("Exten","s");
	cmd.Gen("Variable",val);
	cmd.Gen("Priority","1");
	cmd.Gen("Async","1");
	
	std::string sTmp = "MakeCall|" + no + std::string("|Originate|")+std::string(sHandle)+"|"+std::string(ucid)+std::string("|");
	cmd.End(sTmp);
	std::string sendCmd = cmd.GetCmd();
	pLog->m_rootLog.Log("send ami to pbx \r\n  %s",sendCmd.c_str());
        pLog->m_rootLog.Log("@@@ name %s, phone=%s, start obs", name.c_str(), no.c_str());	
       	
	CSocketManager* pSocketManager = CSocketManager::Instance();
	pSocketManager->WriteComm(sendCmd, sendCmd.length());
//	BOOL bRet = SendCMDToPBX(sendCmd);
}

void CPhone::HumanRecord()
{
	CLogManager* pLog = CLogManager::Instance();
	std::string sGateWay = COBSCfg::Instance()->m_sGateWay;

	if(sGateWay.empty())
	{
		sChan = "SIP/"+no;
	}	
	else
	{
		sChan = "SIP/"+no+"@"+sGateWay;
	}

	char sHandle[256];
	memset(sHandle,0,256);
	sprintf(sHandle,"%d",fd);
	std::string val;
	val = "__phoneno="+no+",";
	val += "__ucid="+ucid+",";
	val += "__sock="+std::string(sHandle)+",";
	val += "__cmd=HumanRecord";
	

    sTimeout="60000";
	CGenCMD cmd;
	cmd.Start();
	cmd.Gen("Action","Originate");
	cmd.Gen("Channel",sChan);
	cmd.Gen("Timeout",sTimeout);
	cmd.Gen("Callerid",no);
	cmd.Gen("Context","Record");
	cmd.Gen("Exten","s");
	cmd.Gen("Variable",val);
	cmd.Gen("Priority","1");
	cmd.Gen("Async","1");
	
	std::string sTmp = "HumanRecord|" + no + std::string("|Originate|")+std::string(sHandle)+"|"+std::string(ucid)+std::string("|");
	cmd.End(sTmp);
	std::string sendCmd = cmd.GetCmd();
	pLog->m_rootLog.Log("send ami to pbx \r\n %s",sendCmd.c_str());
	CSocketManager* pSocketManager = CSocketManager::Instance();
	pSocketManager->WriteComm(sendCmd, sendCmd.length());
//	SendCMDToPBX(sendCmd);
	
}
/*
void CPhone::SetMainSocket(CSocketManager* pSocket)
{
         m_pMainSock = pSocket;
}
*/


//send ami to pbx
/*
BOOL CPhone::SendCMDToPBX(const std::string &sendCmd)
{
    if(sendCmd.length()<0)
		return FALSE;
             
    size_t ret = 0;
    if(m_pMainSock)
	{
		ret = m_pMainSock->WriteComm(sendCmd, sendCmd.length());
    }     
   if(ret == 0)
		return FALSE;
	return TRUE;
 }       
*/

