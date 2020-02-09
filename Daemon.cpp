#include "ace/OS_NS_dirent.h"
#include "Utils.h"
#include "ace/OS_NS_signal.h"
#include "Daemon.h"

void handle_signal(int sig_num)
{
    signal(SIGUSR1, handle_signal);
	Daemon::Singleton()->Stop();
}


Daemon* Daemon::m_singleton;

Daemon::Daemon()
{
}

Daemon* Daemon::Singleton()
{
	return m_singleton;
}

void Daemon::Initialize(CStdString serviceName, DaemonHandler runHandler, DaemonHandler stopHandler)
{
	m_singleton = new Daemon();

	m_singleton->m_runHandler = runHandler;
	m_singleton->m_stopHandler = stopHandler;
	m_singleton->m_serviceName = serviceName;

	m_singleton->m_stopping = false;
	m_singleton->m_shortLived = false;
}

void Daemon::Start()
{
    signal(SIGUSR1, handle_signal);
	Daemon::Run();
}

void Daemon::Run()
{
    int i,lfp;
    char str[10];
    if(getppid()==1) 
		return; /* already a daemon */
    i=fork();
	if (i<0) 
		exit(1); /* fork error */
	if (i>0) 
		exit(0); /* parent exits */
	/* child (daemon) continues */
	setsid(); /* obtain a new process group */
	//for (i=getdtablesize();i>=0;--i) 
	//	close(i); /* close all descriptors */
	i=open("/dev/null",O_RDWR); 
	dup(i); 
	dup(i); /* handle standart I/O */
	umask(027); /* set newly created file permissions */
	//chdir(RUNNING_DIR); /* change running directory */

	char loggingPath[96] = {0};
	
	CStdString lockFile = CStdString("");

	//sprintf(loggingPath , "/var/log/xjobs");
	//char cmd[128] = {0};
	//sprintf(cmd,"mkdir %s",loggingPath);
	//system(cmd);
	
    //ACE_DIR* dir = ACE_OS::opendir(loggingPath);
    //if(dir) 
	//{
	//	ACE_OS::closedir(dir);
	//	lockFile.Format("%s/xjobs.lock", loggingPath);
	//}
	
	system("pwd");

	lockFile.Format("xjobs.lock", loggingPath);
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
		exit(1); /* can not open */
	}	
	if (lockf(lfp,F_TLOCK,0)<0) exit(0); /* can not lock */
	/* first instance continues */
	sprintf(str,"%d\n",getpid());
	write(lfp,str,strlen(str)); /* record pid to lockfile */

	//signal(SIGCHLD,SIG_IGN); /* ignore child */
	//signal(SIGTSTP,SIG_IGN); /* ignore tty signals */
	//signal(SIGTTOU,SIG_IGN);
	//signal(SIGTTIN,SIG_IGN);
	//signal(SIGHUP,signal_handler); /* catch hangup signal */

	Daemon::Singleton()->m_runHandler();

}

void Daemon::Stop()
{
	m_stopping = true;
	m_stopHandler();
}

void Daemon::Install()
{

}

void Daemon::Uninstall()
{
}

bool Daemon::IsStopping()
{
	return m_stopping;
}

void Daemon::SetShortLived()
{
	m_shortLived = true;
}

bool Daemon::GetShortLived()
{
	return m_shortLived;
}
