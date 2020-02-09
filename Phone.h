#ifndef _PHONE_H_
#define _PHONE_H_

#include  <string>
#include "owntype.h"
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
    std::string name;

    std::string sTimeout;
    std::string sChan;	
    std::string status;
	BOOL isSucc;
    
    int fd;
    std::string source;
    std::string project_id;

    void MakeCall();
    void HumanRecord();
    //BOOL SendCMDToPBX(const std::string& cmd);
    //void SetMainSocket(CSocketManager* pSocket);
    //CSocketManager* m_pMainSock; 
};

#endif
