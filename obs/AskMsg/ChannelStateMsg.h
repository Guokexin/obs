// ChannelStateMsg.h: interface for the CChannelStateMsg class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CHANNELSTATEMSG_H
#define CHANNELSTATEMSG_H

#include <string>
#include "../owntype.h"


class CChannelStateMsg
{
public:
	CChannelStateMsg();
	virtual ~CChannelStateMsg();
public:
	std::string EnEvt(const std::string &sState);
	void ParaMsg(const std::string &sDoc);

	std::string m_strSource;
	std::string m_strResponse;
	std::string m_strChanState;

};

#endif
