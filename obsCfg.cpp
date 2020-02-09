#include "obsCfg.h"
#include "MarkUp.h"
#include "logManager.h"
COBSCfg COBSCfg::m_singleton;

COBSCfg* COBSCfg::Instance()
{
	return &m_singleton;
}

void COBSCfg::Initialize()
{
	CLogManager* pLog  = CLogManager::Instance();
	CMarkup xml;
	bool b = xml.Load("obsCfg.xml");
	if(b)
	{
		
		m_sGateWay = xml.FindGetData("/Document/GateWay");
		m_sLocalIP = xml.FindGetData("/Document/AstIP");
		pLog->m_rootLog.Log("GateWay :" + m_sGateWay);
		pLog->m_rootLog.Log("AstIP :" + m_sLocalIP);
	}
	else
	{
		pLog->m_rootLog.Log("read config file failed");
	}
}
