#ifndef _LOG_MANAGER_H_
#define _LOG_MANAGER_H_
#include "log.h"
class CLogManager
{
	public:
		static CLogManager* Instance();
		void Initialize();
		void UnInitialize();
	private:
		static CLogManager* m_singleton;
		CLogManager(){};
	public:
		~CLogManager();
	public:
		CLog  m_ctiLog;
		CLog  m_astLog;
		CLog  m_rootLog;	
    CLog  m_ttsLog;
};

#endif
