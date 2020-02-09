#ifndef SOFTPHONEMSG_H
#define SOFTPHONEMSG_H

#include <string>
#include "owntype.h"

class CSoftPhoneMsg
{
public:
	CSoftPhoneMsg();
	virtual ~CSoftPhoneMsg();


	//�������
	std::string m_strRet;
	std::string m_strReason;
	std::string m_strCause;

	//�������
	std::string m_strStation;
	std::string m_strDnis;
	//��������
	std::string m_strUserData;
	std::string m_strAction;




	//��绰������Ϣ���б���
	std::string EnSoftPhoneMsg();
	//��绰����Response ��Ϣ
	BOOL    ParaProxyResponseMsg(const std::string &strDoc);

};

#endif
