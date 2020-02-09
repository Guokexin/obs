#ifndef _ASK_MSG_DEF_H_
#define _ASK_MSG_DEF_H_

#include <iostream>
#include <string>

struct AgentInfo
{
	CString strAgentNo;
	std::string strStation;
//	HANDLE  sock_cli;
	std::string strUniqueObjectID_1;
	std::string strUniqueObjectID_2;
	std::string strUniqueObjectID_3;

	std::string strChannel;

};

#endif
