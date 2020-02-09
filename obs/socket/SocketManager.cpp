#include "SocketManager.h"
#include "../AskProxyDlg.h"
CSocketManager::CSocketManager()
    :m_startError(0, NULL, NULL, 1)
{
    m_RecvThreadID = 0;
    m_bIsUsing = false;
    m_bIsReconn = false;
    m_bIsStop = false;
    m_bRecvRun = false;
    m_connectType = -1;
//    pthread_mutex_init(&m_mutexErrInfo, NULL);
}

CSocketManager::~CSocketManager()
{
//    StopComm();
//    pthread_mutex_destroy(&m_mutexErrInfo);
}

bool CSocketManager::ConnectTo(const std::string &strDestination, const std::string &strServiceName, int nType)
{
//    std::cout << strServiceName.c_str() << "||||" << strDestination.c_str();
    if(nType == SOCK_STREAM)
    {
        m_connectType = SOCK_STREAM;
        m_addr = ACE_INET_Addr(atoi(strServiceName.c_str()), strDestination.c_str());
        m_timeout = ACE_Time_Value(1,0);
        if(m_connector.connect(m_stream, m_addr)!=0)
        {
            std::cout << "connect error: " << strerror(errno) << std::endl;
            m_stream.close();
            return false;
        }
        std::cout << "ConnectTo PBX 链接成功了！\n" << std::endl;
        m_reactor.register_handler(this, ACE_Event_Handler::READ_MASK);
//        std::cout << "register_handler: " << ret << std::endl;
        return true;
    }
    else if(nType == SOCK_DGRAM)
    {
        //use m_dgram sendto
        m_connectType = SOCK_DGRAM;
        m_addr = ACE_INET_Addr(atoi(strServiceName.c_str()), strDestination.c_str());
        return true;
    }

    return false;
}

bool CSocketManager::WatchComm()
{
    if(!isStart())
    {
        if(isOpen())
        {
            m_bRecvRun = true;
            ACE_Thread_Manager::instance()->spawn(CSocketManager::recv_thread,
                                                  this,
                                                  (THR_NEW_LWP|THR_JOINABLE|THR_INHERIT_SCHED|THR_SUSPENDED),
                                                  &m_RecvThreadID);
            if(m_RecvThreadID == 0)
            {
                m_bRecvRun = false;
                m_stream.close();
                std::cout << "create PBX recv thread error: " << strerror(errno) << std::endl;
                return false;
            }
            else
            {
                std::cout << "create PBX recv thread success: " << std::endl;
                ACE_Thread_Manager::instance()->resume(m_RecvThreadID);
                return true;
            }
        }
    }
    return false;
}

ssize_t CSocketManager::WriteComm(const std::string &buffer, int length)
{
    ssize_t ret;
    if(m_connectType == SOCK_STREAM)
    {
        ret = m_stream.send_n(buffer.c_str(), length);
        if(ret <= 0)
        {
            m_stream.close();
            SetErrorMsgToList(strerror(errno), errno, _send);
        }
        return ret;
    }
    else if(m_connectType == SOCK_DGRAM)
    {
        ACE_INET_Addr client;
        ACE_SOCK_Dgram  dgram(client);
        ret = dgram.send(buffer.c_str(), length, m_addr);
        dgram.close();

        return ret;
    }
    else
    {
        return -1;
    }
}

void CSocketManager::StopComm()
{
    if(isOpen())
    {
        m_stream.close();
    }

    m_bRecvRun = false;

    if(isStart())
    {
        m_bRecvRun = false;
//        std::cout << "wait recv stop" << std::endl;
        ACE_Thread_Manager::instance()->join(m_RecvThreadID);
//        std::cout << "recv stop success" << std::endl;
        m_RecvThreadID = 0;
    }
}

void* CSocketManager::recv_thread(void *par)
{
    CSocketManager *pSock = reinterpret_cast<CSocketManager*>(par);
    pSock->m_reactor.owner(ACE_OS::thr_self());
    while(true)
    {
        if(pSock->m_bRecvRun)
        {
            //ACE_Reactor::instance()->handle_events();
           ACE_Time_Value t(2,0);
           pSock->m_reactor.handle_events(&t);
        }
        else
        {

            break;
        }
    }
    std::cout << "recv thread will stop " << std::endl;
    //pSock->m_stream.close();
    return NULL;
}

CSocketManager::_recvType CSocketManager::sock_recv()
{
    const int length = 1024*10;
    char buffer[length];
    bzero(buffer, length);
    ssize_t r = m_stream.recv(buffer, length, &m_timeout);
    if(r>0)
    {
//        pthread_mutex_lock(&(m_pdlg->m_PBXMsgCritical));
        m_pdlg->m_PBXMsgCritical.acquire();
        m_pdlg->m_PBXMsgList.push_back(buffer);
//        pthread_mutex_unlock(&(m_pdlg->m_PBXMsgCritical));
        m_pdlg->m_PBXMsgCritical.release();
//        sem_post(&(m_pdlg->m_HandleAskPBX.m_DispatchRun));
        m_pdlg->m_HandleAskPBX.m_DispatchRun.release();
        return _recvSuccess;
    }
    else if(r==-1 || errno==EWOULDBLOCK || errno==EAGAIN)
    {
        return _recvIdle;
    }
    else
    {
        std::cout << "recv error : " << r << std::endl;
        SetErrorMsgToList(strerror(errno), errno, _recv);
        return _recvError;
    }
}

bool CSocketManager::isStart()
{
    return m_RecvThreadID != 0;
}

bool CSocketManager::isOpen()
{
    return m_stream.get_handle () != ACE_INVALID_HANDLE;
}

void CSocketManager::SetErrorThreadStart(bool bisusing, long nerrorlistnum)
{
    m_bIsUsing = bisusing;
    m_errorListNum = nerrorlistnum;
    if(bisusing == true)
        StartThread(FALSE);

    return;
}

void CSocketManager::StartThread(BOOL run)
{
    ACE_Thread_Manager::instance()->spawn(CSocketManager::errThread,
                                          this,
                                          (THR_NEW_LWP|THR_JOINABLE|THR_INHERIT_SCHED|THR_SUSPENDED),
                                          &m_errThreadID);
    ACE_Thread_Manager::instance()->resume(m_errThreadID);
}

void* CSocketManager::errThread(void *par)
{
    CSocketManager *manager = reinterpret_cast<CSocketManager*>(par);
    while(true)
    {
        manager->m_startError.acquire();
        if(manager->m_bIsStop)
        {
            break;
        }
        else
        {
//            sem_wait(&(manager->m_startError));
            manager->GetErrorMsgStart();
        }
    }
    std::cout << "error thread stop" << std::endl;
    return NULL;
}

void CSocketManager::GetErrorMsgStart()
{
    std::cout << "error MSG START" << std::endl;
    while(true)
    {
        std::string serror = "";
        int nerrornum = 0;
        _errortype etype = _unknowntype;
        bool bret = GetErrorMsgFromList(serror,nerrornum,etype);
        if(bret==false)
        {
            break;
        }
        else
        {
//            std::cout << "get error msg true" << std::endl;
            m_pdlg->m_Log.Log("CSocketManager: errormsg=%s nerrornum=%d etype=%d",serror.c_str(),nerrornum,etype);
            BOOL bIsReconn = FALSE;

            if (etype==_send)
            {
                bIsReconn = TRUE;
            }
            else if (etype==_recv)
            {
                bIsReconn = TRUE;
            }
            else if(etype==_connect)
            {
                bIsReconn = TRUE;
            }
            else
            {
                m_pdlg->m_Log.Log("CSocketManager::GetErrorMsgStart: 程序内部错误");
            }

            if(bIsReconn==TRUE)
            {
//                std::cout << "reconnect true" << std::endl;
                m_pdlg->m_SockManager.m_bIsReconn = true;
                bool bisreconnsuc = false;
                while(bisreconnsuc==false)
                {
                    std::cout << "stop connect" << std::endl;
                    m_pdlg->m_SockManager.StopComm();
                    std::cout << "stop connect success" << std::endl;
                    usleep(100*1000);
                    BOOL bSuccess = m_pdlg->m_SockManager.ConnectTo(m_pdlg->m_SettingData.m_strPBX,
                                    m_pdlg->m_SettingData.m_strPbxPort,
                                    SOCK_STREAM);
//                    std::cout << "sockmanager connect to server" << std::endl;
                    if(bSuccess)
                    {
                        bisreconnsuc = true;
                        bool bWatch = m_pdlg->m_SockManager.WatchComm();
                        if(bWatch)
                        {
                            m_pdlg->m_Log.Log("Relisten asterisk succ");
                        }
                        m_pdlg->m_Log.Log("ReConnect astersik  Succ");
                    }
                    else
                    {
                        m_pdlg->m_Log.Log("ReConnect astersik fail: 网络异常 then Sleep 5000");
                        usleep(5000*1000);//网络异常
                    }
                }
                usleep(100*1000);
                m_pdlg->m_SockManager.DeleteAllErrorMsg();
                m_pdlg->m_SockManager.m_bIsReconn = false;
                usleep(100*1000);
                m_pdlg->m_Log.Log("GetErrorMsgStart: RegEnCode");
                m_pdlg->m_RegAction.RegEnCode();
            }
        }
    }
}

void CSocketManager::SetErrorMsgToList(const std::string &strerror,int nerrornum,_errortype etype)
{
    if(m_bIsUsing)
    {
        if(m_bIsReconn==true)
            return;
//        pthread_mutex_lock(&m_mutexErrInfo);
        m_mutexErrInfo.acquire();
        if(m_errMsgList.size()>m_errorListNum)
        {
            delete *(m_errMsgList.begin());
            m_errMsgList.pop_front();
        }
        errInfo *p = new errInfo();
        p->errorStr  = strerror;
        p->errorNum  = nerrornum;
        p->errorType = etype;
        m_errMsgList.push_front(p);
//        pthread_mutex_unlock(&m_mutexErrInfo);
        m_mutexErrInfo.release();
//        sem_post(&m_startError);
        m_startError.release();
    }
}

bool CSocketManager::GetErrorMsgFromList(std::string &errStr, int &errNum, _errortype &errType)
{
    if(m_errMsgList.size())
    {
//        pthread_mutex_lock(&m_mutexErrInfo);
        m_mutexErrInfo.acquire();
        std::list<errInfo*>::iterator it = m_errMsgList.begin();
        if(it!=m_errMsgList.end())
        {
            errStr  = (*it)->errorStr;
            errNum  = (*it)->errorNum;
            errType = (*it)->errorType;
            delete (*it);
            m_errMsgList.pop_front();
        }
//        pthread_mutex_unlock(&m_mutexErrInfo);
        m_mutexErrInfo.release();
        return true;
    }
    else
        return false;
}

void CSocketManager::DeleteAllErrorMsg()
{
//    pthread_mutex_lock(&m_mutexErrInfo);
    m_mutexErrInfo.acquire();
    std::list<errInfo*>::iterator it = m_errMsgList.begin();
    while(it != m_errMsgList.end())
    {
        delete (*it);
        m_errMsgList.erase(it);
        it = m_errMsgList.begin();
    }
//    pthread_mutex_unlock(&m_mutexErrInfo);
    m_mutexErrInfo.release();
}

//ACE_Reactor

ACE_HANDLE CSocketManager::get_handle(void) const
{
    return m_stream.get_handle();
}

int CSocketManager::handle_input(ACE_HANDLE)
{
    const int length = 1024*10;
    char buffer[length];
    bzero(buffer, length);
    ssize_t r = m_stream.recv(buffer, length-1, &m_timeout);
    if(r>0)
    {
//        std::cout << "recv from pbx: " << buffer << std::endl;
//        pthread_mutex_lock(&(m_pdlg->m_PBXMsgCritical));
        m_pdlg->m_PBXLog.Log("FLOW_LOG->%s",buffer);
        m_pdlg->m_PBXMsgCritical.acquire();
        m_pdlg->m_PBXMsgList.push_back(buffer);
//        pthread_mutex_unlock(&(m_pdlg->m_PBXMsgCritical));
        m_pdlg->m_PBXMsgCritical.release();
//        sem_post(&(m_pdlg->m_HandleAskPBX.m_DispatchRun));
        m_pdlg->m_HandleAskPBX.m_DispatchRun.release();
//        std::cout << "handler_input:" << buffer << std::endl;
        return _recvSuccess;
    }
    else if(r==-1 || errno==EWOULDBLOCK || errno==EAGAIN)
    {
        return _recvIdle;
    }
    else
    {
//        std::cout << "recv error : " << r << std::endl;
        SetErrorMsgToList(strerror(errno), errno, _recv);
        return -1;
    }
}

int CSocketManager::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask mask)
{
    SetErrorMsgToList(strerror(errno), errno, _connect);
//    std::cout << "connect error" << std::endl;
    //return ACE_Event_Handler::handle_close(handle, mask);
    return m_reactor.remove_handler(handle, DONT_CALL);
}

void CSocketManager::StopErrorThread()
{
    DeleteAllErrorMsg();
    m_bIsStop = true;
    m_startError.release();
    ACE_Thread_Manager::instance()->join(m_errThreadID);
}

void CSocketManager::CloseSocketManager()
{
    StopErrorThread();
    StopComm();
}
