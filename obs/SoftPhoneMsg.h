#ifndef SOFTPHONEMSG_H
#define SOFTPHONEMSG_H

#include <string>
#include "owntype.h"

class CSoftPhoneMsg
{
public:
	CSoftPhoneMsg();
	virtual ~CSoftPhoneMsg();


	//输出参数
	std::string m_strRet;
	std::string m_strReason;
	std::string m_strCause;

	//输入参数
	std::string m_strStation;
	std::string m_strDnis;
	//公共参数
	std::string m_strUserData;
	std::string m_strAction;




	//软电话发送消息进行编码
	std::string EnSoftPhoneMsg();
	//软电话解析Response 消息
	BOOL    ParaProxyResponseMsg(const std::string &strDoc);

};

#endif
