// GetPopDataEvt.h: interface for the CGetPopDataEvt class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GETPOPDATAEVT_H
#define GETPOPDATAEVT_H

#include <string>
#include "../owntype.h"

class CGetPopDataEvt
{
public:
	CGetPopDataEvt();
	virtual ~CGetPopDataEvt();

public:
	//解析软电话消息
	BOOL ParaSoftPhoneRequestMsg(const std::string &sDoc);
	//将请求结果返回给软电话
	std::string EnSoftPhoneMsg();


	std::string m_strStation;
	std::string m_strDnis;
	std::string m_strUserData;
	std::string m_strReason;
	std::string m_strRet;

    std::string m_strAgentId;
	std::string m_strTime;
};

#endif // !defined(AFX_GETPOPDATAEVT_H__A4B16441_8EEB_4C9D_AEF9_EFC0A2BC066A__INCLUDED_)
