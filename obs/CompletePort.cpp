#include "CompletePort.h"
#include "AskProxyDlg.h"

//CCompletePort::CCompletePort()
//    :m_Proactor(NULL)
//{
////    m_eventThreadID = 0;
////    m_stop = true;
//    m_Proactor = new ACE_Proactor();
//}

CCompletePort::CCompletePort()
{

}


CCompletePort::~CCompletePort()
{
//    if(m_Proactor)
//    {
//        delete m_Proactor;
//        m_Proactor = NULL;
//    }
}

bool CCompletePort::Init(CAskProxyDlg *dlg)
{
//    if(!m_Proactor)
//    {
//        std::cout << "new ACE_Proactor error: " << strerror(errno) << std::endl;
//        return false;
//    }
//    m_myTask.proactor_ = m_Proactor;
//
    if(!dlg) return false;
    m_pDlg = dlg;
//    m_addrAccept.set(atoi(m_pDlg->m_SettingData.m_strProxyPort.c_str()));
//    if(m_myTask.start(2) == 0)
//    {
//        if(m_acceptor.open(m_addrAccept, 0, 0, ACE_DEFAULT_ASYNCH_BACKLOG, 0, m_Proactor)!=0)
//            return false;
//    }
//    else
//        return false;
//    //std::cout << "accept return: " << n << "  .error: " << strerror(errno) << std::endl;
////    ACE_Thread_Manager::instance()->resume(m_eventThreadID);
    return m_conn.init(atoi(m_pDlg->m_SettingData.m_strProxyPort.c_str()));
}

bool CCompletePort::SendMsgToUser(int s, const std::string &msg)
{
    m_conn.send_to_softphone(s, msg);
    return true;
}

/*
bool CCompletePort::startThread()
{
    m_stop = false;
    ACE_Thread_Manager::instance()->spawn(CCompletePort::event_thread,
                                          this,
                                          (THR_NEW_LWP|THR_JOINABLE|THR_INHERIT_SCHED|THR_SUSPENDED),
                                          &m_eventThreadID
                                         );
    if(m_eventThreadID > 0)
    {
        return true;
    }
    else
    {
        m_stop = true;
        return false;
    }
}

void* CCompletePort::event_thread(void *p)
{
    CCompletePort *ptr = reinterpret_cast<CCompletePort*>(p);

    while(true)
    {
        if(ptr->m_stop)
            break;
        else
            ACE_Proactor::instance()->handle_events();
    }
    return NULL;
}
*/
