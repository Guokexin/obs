
#ifndef __DAEMON_H__
#define __DAEMON_H__

#include "StdString.h"
#include "ace/Singleton.h"
#include "ace/Thread_Mutex.h"

typedef void (*DaemonHandler)(void);

class  Daemon
{
public:
	static void Initialize(CStdString serviceName, DaemonHandler runHandler, DaemonHandler stopHandler);
	static Daemon* Singleton();
	void Start();
	void Stop();
	void Install();
	void Uninstall();
	bool IsStopping();

	void SetShortLived();
	bool GetShortLived();

private:
	Daemon();
	static Daemon* m_singleton;

	static void Run();

	DaemonHandler m_runHandler;
	DaemonHandler m_stopHandler;
	CStdString m_serviceName;

	bool m_stopping;
	bool m_shortLived;
};

//typedef ACE_Singleton<Daemon, ACE_Thread_Mutex> DaemonSingleton;

#endif

