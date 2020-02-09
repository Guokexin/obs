#include <iostream>
#include <ace/Log_Msg.h>
#include <unistd.h>
#include "AskProxyDlg.h"

#define MAXFD 1024
//CAskProxyDlg theApp;
void getpipe(int)
{
    GetApp()->m_CheckAgent.m_CheckAgentLog.Log("ERROR_LOG->有异常socket");
}

int main ()
{
    signal(SIGPIPE, getpipe);
//    pid_t pid1;//, pid2;
//    if((pid1 = fork()) < 0)
//    {
//        std::cout << "create child process error: " << strerror(errno) << std::endl;
//        return 1;
//    }
//    else if(pid1>0)
//    {
//        exit(0);
//    }
//    else
//    {
        //setsid();

//        if((pid2 = fork()) < 0)
//        {
//            std::cout << "create child process error: " << strerror(errno) << std::endl;
//            return 2;
//        }
//        else if(pid2>0)
//        {
//            exit(0);
//        }
//        else
//        {
//            umask(0);
//            for(int i=0; i<MAXFD; i++)
//                close(i);

            CAskProxyDlg *theApp = GetApp();
            if(!theApp->OnStart())
            {
                return 1;
            }

            std::cout << "start AskProxy-linux success" << std::endl;
            theApp->m_Log.Log("start AskProxy-linux success");

            ACE_Thread_Manager::instance()->wait();
            //ACE_Thread_Manager *m = ACE_Thread_Manager::instance();
            //m->join(theApp->m_SockManager.m_RecvThreadID);
            //m->join(theApp->m_SockManager.m_errThreadID);

            theApp->m_Log.Log("AskProxy-linux stop");
       // }
    //}

    //delete theApp;

    return 0;
}
