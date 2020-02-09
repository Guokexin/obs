#include <iostream>
#include <string>
#include "logManager.h"
#include "log.h"
#include "obsCfg.h"
#include "Userconn.h"
#include "HandleUserMsgThread.h"
#include "SocketManager.h"
#include "RegAskMsg.h"
#include "HandleAskPBX.h"
#include "Daemon.h"
#include <unistd.h>
#include "Utils.h"
#include "ace/OS_NS_dirent.h"
#include "un_tts.h"
#include "DBConnection.h"
//#include "license.h"

static volatile bool serviceStop = false;
void StopHandler()
{
	serviceStop = true;
}

void MainThread()
{
	
	// Avoid program exit on broken pipe
	ACE_OS::signal (SIGPIPE, (ACE_SignalHandler) SIG_IGN);

	//first : init log Manager
	CLogManager* pLogManager = CLogManager::Instance();
	pLogManager->Initialize();
	pLogManager->m_ctiLog.Log("obs start");
	pLogManager->m_astLog.Log("ast log start");
	pLogManager->m_rootLog.Log("main log start");


        CDBConnection* pDB = CDBConnection::Instance();
        int ret = pDB->Initialize();                                                                                  
        if(ret != 0)                                                                                              
        {                                                                                                         
                pLogManager->m_rootLog.Log("db connect failed");
                exit(-1);                                                                                         
        }                                                                                                         
        else                                                                                                      
        {                                                                                                         
                pLogManager->m_rootLog.Log("db connect init succ");                                                      
        }        
             
	//
//	CLicense license;
//	std::list<CStdString> ls;
//	license.FindAllDevice(ls);	
	//second : init config
	COBSCfg* pCfg = COBSCfg::Instance();
	pCfg->Initialize();
	
	//third : init client conn
	pLogManager->m_rootLog.Log("start UserConn Thread");
	CUserConn* pUserConn = CUserConn::Instance();
	pUserConn->init(6003);
	
	//four
	CHandleUserMsgThread* pHandleUserMsg = CHandleUserMsgThread::Instance();
	pHandleUserMsg->Initialize();

        //pHandleUserMsg->ReadDB();

	//five
	CHandleAskPBX* pHandleAskPBX = CHandleAskPBX::Instance();
	pHandleAskPBX->StartHandleAskPBX();
	//pHandleAskPBX->Instance();

	//six
	CSocketManager* pSocketManager = CSocketManager::Instance();
	pSocketManager->Initialize();
	bool bConn = pSocketManager->ConnectTo(pCfg->m_sLocalIP,std::string("5038"),SOCK_STREAM);
	if(bConn)
	{
		pLogManager->m_rootLog.Log("connet to ast succ");
        bool bWatch = pSocketManager->WatchComm();
        if(bWatch)
        {
            pLogManager->m_rootLog.Log("listen asterisk succ111");
        }
    }
    else
    {
        pLogManager->m_rootLog.Log("Cann't connect to asterisk");
	}	
	//senven
	CRegAskMsg reg;
	std::string s = reg.RegEnCode();

	pSocketManager->WriteComm(s, s.length());

  /*
  //eight start tts
  AliTTS tts;
  tts.accessKeySecret = "UHnCXaf5Z1ei89w5EBUwnDFiDNjExb";
  tts.accessKeyId = "LTAIItylw3hA3G39";
  tts.app_key = "LymvIJx0i4JzpVh7"; 
  tts.domain = "";
  string msg = "这是一段测试TTS";
  string file = "mytest.wav";
  tts.gen_voice(msg, file);
  */
  
	
	while(true)
	{
		ACE_OS::sleep(1);
	}
	
		
	reg.UnRegEnCode();
	
	std::cout<<"app exit"<<std::endl;
}
/*
int main(int argc,char** argv)
{

    int i,lfp;
    char str[10];
    if(getppid()==1) 
		return 0;
    i=fork();
	if (i<0) 
		exit(1); 
	if (i>0) 
		exit(0); 
	
	setsid(); 
	//for (i=getdtablesize();i>=0;--i) 
	//	close(i); 
	i=open("/dev/null",O_RDWR); 
	dup(i); 
	dup(i);
	umask(027); 
	//chdir(RUNNING_DIR); 

	char loggingPath[512] = {0};
	
	CStdString lockFile = CStdString("");

	sprintf(loggingPath , "/var/log/xjobs");
	char cmd[128] = {0};
	sprintf(cmd,"mkdir %s",loggingPath);
	system(cmd);
	
    ACE_DIR* dir = ACE_OS::opendir(loggingPath);
    if(dir) 
	{
		ACE_OS::closedir(dir);
		lockFile.Format("%s/xjobs.lock", loggingPath);
	}
	

	if(!lockFile.size()) 
	{
		lfp=open(lockFile.c_str(),O_RDWR|O_CREAT,0640);
	} else {
		lfp=open(lockFile.c_str(),O_RDWR|O_CREAT,0640);
	}

	if (lfp<0)
	{
		lfp=open("xjobs.lock",O_RDWR|O_CREAT,0640);
	}	
	if (lfp<0)
	{
		exit(1); 
	}	
	if (lockf(lfp,F_TLOCK,0)<0) exit(0); 
	
	sprintf(str,"%d\n",getpid());
	write(lfp,str,strlen(str)); 

	//signal(SIGCHLD,SIG_IGN);
	//signal(SIGTSTP,SIG_IGN); 
	//signal(SIGTTOU,SIG_IGN);
	//signal(SIGTTIN,SIG_IGN);
	//signal(SIGHUP,signal_handler); 
	MainThread();
}*/

int main(int argc,char** argv)
{
	CStdString program(argv[0]);
	std::cout<<program.c_str()<<std::endl;
	CStdString serviceNameWithExtension = FileBaseName(program);
	CStdString serviceName = FileStripExtension(serviceNameWithExtension);
	if (serviceName.IsEmpty())
	{
		printf("Error: Could not determine service name.\n");
		return -1;
	}

	Daemon::Initialize(serviceName, MainThread, StopHandler);
	CStdString argument = argv[1];
	
	if (argc>1)
	{
		if (argument.CompareNoCase("debug") == 0)
		{
			MainThread();
		}
		else if (argument.CompareNoCase("install") == 0)
		{
			Daemon::Singleton()->Install();
		}
		else if  (argument.CompareNoCase("uninstall") == 0)
		{
			Daemon::Singleton()->Uninstall();
		}
		else
		{
			printf("Argument incorrect. Possibilies are:\ndebug: run attached to tty\ntranscode <file>: convert .mcf file to storage format specified in config.xml\n\n");
		}
	}
	else
	{
		// No arguments, launch the daemon
		printf("Starting orkaudio daemon ... (type 'orkaudio debug' if you prefer running attached to tty)\n");
		Daemon::Singleton()->Start();		
	}
	return 0;
}




