#include "logManager.h"
#include "stdlib.h"

CLogManager* CLogManager::m_singleton = NULL;


CLogManager::~CLogManager()
{
	delete this;
}
CLogManager* CLogManager::Instance()
{
	if(m_singleton != NULL)
	{
		return m_singleton;
	}
	else
	{
		m_singleton = new CLogManager();
		return m_singleton;
	}
}



void CLogManager::Initialize()
{
	std::cout<<"cur dir"<<std::endl;
	system("pwd");
    log4cxx::PropertyConfigurator::configure("logging.properties");
	m_ctiLog.Init("cti");
	m_astLog.Init("ast");
	m_rootLog.Init("root");
  m_ttsLog.Init("tts");

}

//void CLogManager::UnInit()
//{
//	delete this;
//}
