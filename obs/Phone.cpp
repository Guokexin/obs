
#include "GenCMD.h"
#include "socket/SocketManager.h"
#include "AskProxyDlg.h"

CPhone::CPhone()
{
	status = "0";
	
}

CPhone::~CPhone()
{

}


void CPhone::MakeCall()
{
	
	//std::cout<<"sock = "<<fd<<std::endl;
	//m_pDlg->m_IOOpt.SendMsgToUser(fd,"ddddddddd");
	//std::cout<<"Enter make call"<<std::endl;
	sChan = "SIP/"+no+"@"+m_pDlg->m_SettingData.m_strInPBX;
	char sHandle[256];
	memset(sHandle,0,256);
	sprintf(sHandle,"%d",fd);
	std::string val;
	val = "__phoneno="+no+",";
	val += "__ucid="+ucid+",";
	val += "__sock="+std::string(sHandle)+",";
	val += "__voicefile="+voicefile+",";
	val += "__cmd="+cmd;

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
	m_pDlg->m_Log.Log("发送给PBX的数据：%s",sendCmd.c_str());
	std::cout<<"before send cmd to pbx"<<std::endl;
	BOOL bRet = SendCMDToPBX(sendCmd);
	
	std::cout<<sendCmd.c_str()<<std::endl;
}

void CPhone::HumanRecord()
{
	std::cout<<"humanRecord"<<std::endl;
	
	sChan = "SIP/"+no+"@"+m_pDlg->m_SettingData.m_strInPBX;
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
	
	std::cout<<" end 1"<<std::endl;
	std::string sTmp = "HumanRecord|" + no + std::string("|Originate|")+std::string(sHandle)+"|"+std::string(ucid)+std::string("|");
	std::cout<<" end 2"<<std::endl;
	cmd.End(sTmp);
	std::cout<<" end 3"<<std::endl;
	std::string sendCmd = cmd.GetCmd();
	std::cout<<" end 4"<<std::endl;
	m_pDlg->m_Log.Log("发送给PBX的数据：%s",sendCmd.c_str());
	std::cout<<" end 5"<<std::endl;
	std::cout<<"before send cmd to pbx"<<std::endl;
	SendCMDToPBX(sendCmd);
	std::cout<<" end 6"<<std::endl;
	std::cout<<"ffffffffffffffffff"<<std::endl;
	std::cout<<" end 7"<<std::endl;
	
	std::cout<<sendCmd.c_str()<<std::endl;
}


 void CPhone::SetMainWnd(CAskProxyDlg* pDlg)
 {
         m_pDlg = pDlg;
 }

void CPhone::SetMainSocket(CSocketManager* pSocket)
{
         m_pMainSock = pSocket;
}



//send ami to pbx
BOOL CPhone::SendCMDToPBX(const std::string &sendCmd)
{
	std::cout<<"Enter SendCMDToPBX"<<std::endl;
    	if(sendCmd.length()<0)
             return FALSE;
             
      	m_pDlg->m_AgentLog.Log(sendCmd);
         size_t ret = 0;
         if(m_pMainSock)
          {
		std::cout<<"Enter send program"<<std::endl;
                  ret = m_pMainSock->WriteComm(sendCmd, sendCmd.length());
          }     
          if(ret == 0)
                  return FALSE;
          return TRUE;
 }       


