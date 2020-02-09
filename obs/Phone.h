#ifndef _PHONE_H_
#define _PHONE_H_

#include  <string>
#include "owntype.h"
class CSocketManager;
class CAskProxyDlg;
class CPhone
{
public:
    CPhone();
    ~CPhone();
    
    std::string no;
    std::string ucid;
    std::string voicefile;
    std::string cmd;
    std::string caller;

    std::string sTimeout;
    std::string sChan;	
    std::string status;
    
    int fd;

    void MakeCall();
    void HumanRecord();
    BOOL SendCMDToPBX(const std::string& cmd);
    void SetMainWnd(CAskProxyDlg* pDlg);
    void SetMainSocket(CSocketManager* pSocket);
    CSocketManager* m_pMainSock; 
    CAskProxyDlg* m_pDlg;
};

#endif
