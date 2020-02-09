#ifndef GENCMD_H
#define GENCMD_H

#include <iostream>
#include <string>
//#include "socket/SocketManager.h"

class CGenCMD
{
public:
	CGenCMD();
	virtual ~CGenCMD();

	void    Start();
	void    Gen(const std::string &Key, const std::string &Value);
	void    End(const std::string &sAction = "", const std::string &sActionID="");

    std::string   GenActionID();

//	void      SendCMD(CSocketManager* sock);

	std::string   GetActionID();

	std::string GetCmd();



public:
	std::string m_strCMD;
	std::string m_strActionID;

};

#endif
