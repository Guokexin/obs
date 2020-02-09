#ifndef REGASKMSG_H
#define REGASKMSG_H

#include <string>
#include "../owntype.h"

class CAskProxyDlg;
class CRegAskMsg
{
public:
	CRegAskMsg();
	virtual ~CRegAskMsg();
	CAskProxyDlg* pdlg;

	void RegEnCode();//注册
	std::string m_sRegActionID;//注册标识

	void UnRegEnCode();//反注册-pbx会断开socket

};

#endif
