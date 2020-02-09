// SendDtmfEvt.h: interface for the CSendDtmfEvt class.
//
//////////////////////////////////////////////////////////////////////

#ifndef SENDDTMFEVT_H
#define SENDDTMFEVT_H

#include <string>
#include "../owntype.h"

class CSendDtmfEvt
{
public:
	CSendDtmfEvt();
	virtual ~CSendDtmfEvt();

	std::string m_strStation;
	std::string digits;


	std::string m_strEvt;

	//解析软电话消息
	BOOL ParaSoftPhoneRequestMsg(const std::string &sDoc);
	//将请求结果返回给软电话
	std::string EnSoftPhoneMsg();

};

#endif
