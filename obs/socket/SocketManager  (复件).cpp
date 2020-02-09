#include "SocketManager.h"
#include "../AskProxyDlg.h"
CSocketManager::CSocketManager()
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
    StopComm();
//    pthread_mutex_destroy(&m_mutexErrInfo);
}

bool CSocketManager::ConnectTo(const std::string &strDestination, const std::string &strServiceName, int nType)
{
    if(nType == SOCK_STREAM)
    {
        m_connectType = SOCK_STREAM;
        m_addr = ACE_INET_Addr(atoi(strServiceName.c_str()), strDestination.c_str());
        m_timeout = ACE_Time_Value(1,0);
        if(m_connector.connect(m_stream, m_addr)!=0)
        {
            std::cout << "connect error: " << strerror(errno) << std::endl;
            return false;
        }
        std::cout << "ConnectTo 链接成功了！\n" << std::endl;
        return true;
    }
    else if(nType == SOCK_DGRAM)
    {
        //use m_dgram sendto
        m_connectType = SOCK_DGRAM;
        m_addr = ACE_INET_Addr(atoi(strServiceName.c_str()), strDestination.c_str());
        std::cout << "create UDP sock" << std::endl;
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
            SetErrorMsgToList(strerror(errno), errno, _send);
        }
        return ret;
    }
    else if(m_connectType == SOCK_DGRAM)
    {
        ACE_SOCK_Dgram  dgram;
        ret = dgram.send(buffer.c_str(), length, m_addr);
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

    if(isStart())
    {
        m_bRecvRun = false;
        ACE_Thread_Manager::instance()->join(m_RecvThreadID);
        m_RecvThreadID = 0;
    }
}

void* CSocketManager::recv_thread(void *par)
{
    CSocketManager *pSock = reinterpret_cast<CSocketManager*>(par);
    while(true)
    {
        if(pSock->m_bRecvRun)
        {
            _recvType ret = pSock->sock_recv();
            if(ret == _recvError)
                break;
            else if(ret == _recvIdle)
                sleep(0);
        }
        else
        {
            break;
        }
    }
    return NULL;
}

CSocketManager::_recvType CSocketManager::sock_recv()
{
    const int length = 1024*10;
    char buffer[length];
    bzero(buffer, length);
    int r = m_stream.recv(buffer, length, &m_timeout);
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
    sem_init(&m_startError, 0, run);
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
        if(manager->m_bIsStop)
        {
            break;
        }
        else
        {
            sem_wait(&(manager->m_startError));
            manager->GetErrorMsgStart();
        }
    }
    return NULL;
}

void CSocketManager::GetErrorMsgStart()
{
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
            m_pdlg->m_Log.Log("CSocketManager: errormsg=%s nerrornum=%d etype=%d",serror.c_str(),nerrornum,etype);
            BOOL bIsReconn = FALSE;
            if(etype==_selectsend || etype==_selectrecv)
            {
            }
            else if (etype==_send)
            {
                bIsReconn = TRUE;
            }
            else if (etype==_recv)
            {
                bIsReconn = TRUE;
            }
            else
            {
                m_pdlg->m_Log.Log("CSocketManager::GetErrorMsgStart: 程序内部错误");
            }
            if(bIsReconn==TRUE)
            {
                m_pdlg->m_SockManager.m_bIsReconn = true;
                bool bisreconnsuc = false;
                while(bisreconnsuc==false)
                {
                    m_pdlg->m_SockManager.StopComm();
                    usleep(100);
                    BOOL bSuccess = m_pdlg->m_SockManager.ConnectTo(m_pdlg->m_SettingData.m_strPBX,
                                    m_pdlg->m_SettingData.m_strPbxPort,
                                    SOCK_STREAM);
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
                        usleep(5000);//网络异常
                    }
                }
                usleep(100);
                m_pdlg->m_SockManager.DeleteAllErrorMsg();
                m_pdlg->m_SockManager.m_bIsReconn = false;
                usleep(100);
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
        sem_post(&m_startError);
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












