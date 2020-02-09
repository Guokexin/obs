#ifndef RESPONSEMSG_H
#define RESPONSEMSG_H

#include <string>

class CResponseMsg
{
public:
	CResponseMsg();
	virtual ~CResponseMsg();

	std::string m_strRet;
	std::string m_strUserData;
	std::string m_strStation;
	std::string m_strOperation;

	std::string m_strActionID;
    std::string m_strAgentId;
    std::string m_tenantID;
	std::string m_strTime;
	std::string m_pbxIP;

	std::string m_strBody;

	std::string m_strQueue;

	std::string  ParaAskPBXResponse(const std::string &sDoc);
	std::string  EncodeResp();


};

#endif
