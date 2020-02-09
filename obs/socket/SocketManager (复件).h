#ifndef SOCKETMANAGER_H
#define SOCKETMANAGER_H

#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Dgram.h>
#include <ace/SOCK_Connector.h>
#include <ace/INET_Addr.h>
#include <ace/Thread_Mutex.h>
#include <iostream>
#include <string>
#include <list>

#include "../owntype.h"

class CAskProxyDlg;

class CSocketManager
{
public:
    CSocketManager();
    ~CSocketManager();

private:
    enum _recvType
    {
        _recvError=0,
        _recvIdle,
        _recvSuccess
    };

public:
    bool ConnectTo(const std::string &strDestination,
                  const std::string &strServiceName,
                  int nType);
    bool WatchComm();
    ssize_t WriteComm(const std::string &buffer, int length);
    void StopComm();
    static void* recv_thread(void *par);
    _recvType sock_recv();

public:
    enum _errortype
    {
        _unknowntype=0,
        _selectsend,
        _selectrecv,
        _send,_recv
    };

    typedef struct _errInof
    {
        std::string errorStr;
        int errorNum;
        _errortype errorType;
    } errInfo;

    void SetErrorThreadStart(bool bisusing=true, long nerrorlistnum=10);
    void StartThread(BOOL run);
    static void* errThread(void *par);
    void GetErrorMsgStart();
    void SetErrorMsgToList(const std::string &strerror, int nerrornum, _errortype etype);
    bool GetErrorMsgFromList(std::string &errStr, int &errNum, _errortype &errType);
    void DeleteAllErrorMsg();

public:
    bool isStart();
    bool isOpen();

private:
    ACE_INET_Addr m_addr;
    ACE_SOCK_Connector m_connector;
    ACE_Time_Value m_timeout;
    ACE_SOCK_Stream m_stream;
    ACE_thread_t m_RecvThreadID;
    volatile bool m_bRecvRun;
    int m_connectType;

private:
    bool m_bIsReconn;
    bool m_bIsUsing;
    volatile bool m_bIsStop;
    UINT  m_errorListNum;
    sem_t m_startError;
//    pthread_mutex_t m_mutexErrInfo;
    ACE_Thread_Mutex  m_mutexErrInfo;
    ACE_thread_t m_errThreadID;
    std::list<errInfo*> m_errMsgList;

public:
    CAskProxyDlg *m_pdlg;
};

#endif
