#include "CheckAgent.h"
#include "AskProxyDlg.h"
#include "UntiTool.h"

CCheckAgent::CCheckAgent()
    :m_hStartCheckAgentEvent(0, NULL, NULL, 1)
{
	m_hStopCheckAgentEvent = false;
	m_lpThread = 0;
}

CCheckAgent::~CCheckAgent()
{

}

void CCheckAgent::SetWnd(CAskProxyDlg* pDlg)
{
	m_pDlg = pDlg;
}
//启动检测坐席的线程
BOOL CCheckAgent::StartCheckAgentThread()
{
	CUntiTool tool;
	m_CheckAgentLog.Init("CheckAgentLog");
//	m_hStartCheckAgentEvent = CreateEvent(NULL,FALSE,FALSE,"startcheck"+tool.GenUCID());
//	m_hStopCheckAgentEvent = CreateEvent(NULL,FALSE,FALSE,"stopcheck"+tool.GenUCID());

//	m_lpThread = AfxBeginThread(CCheckAgent::CheckAgentFun,this,THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED,NULL);

    ACE_Thread_Manager::instance()->spawn(CCheckAgent::CheckAgentFun,
                                          this,
                                          (THR_NEW_LWP|THR_JOINABLE|THR_INHERIT_SCHED|THR_SUSPENDED),
                                          &m_lpThread
                                         );

	if(m_lpThread)
	{
        ACE_Thread_Manager::instance()->resume(m_lpThread);
        m_CheckAgentLog.Log("FLOW_LOG->创建检测坐席心跳线程成功");
        return TRUE;
	}
	else
	{
	    m_CheckAgentLog.Log("FLOW_LOG->创建检测坐席心跳线程失败");
		return FALSE;
	}
}
//停止检测坐席的线程
void CCheckAgent::StopCheckAgentThread()
{
//	SetEvent(m_hStopCheckAgentEvent);
    m_hStopCheckAgentEvent = true;
    m_hStartCheckAgentEvent.release();
//	WaitForSingleObject(m_lpThread->m_hThread,20000);
    ACE_Thread_Manager::instance()->join(m_lpThread);
//	CloseHandle(m_hStopCheckAgentEvent);
    m_hStopCheckAgentEvent = false;
    m_lpThread = 0;
//	CloseHandle(m_hStartCheckAgentEvent);
	//CloseHandle(m_lpThread->m_hThread);
}
//检查坐席线程主函数
LPVOID CCheckAgent::CheckAgentFun(LPVOID lpvoid)
{
	CCheckAgent* pThis = reinterpret_cast<CCheckAgent*>(lpvoid);

	while(true)
	{
		//每60S检测一次
//		DWORD ret = WaitForSingleObject(pThis->m_hStopCheckAgentEvent,10000);
//-------------------------
        sleep(60);
//-------------------
//        ACE_Time_Value timeout(time(NULL)+10, 0);
//        pThis->m_hStartCheckAgentEvent.acquire();
		if(pThis->m_hStopCheckAgentEvent)
		{
			break;
		}
		else
		{
			//1、检测卡分机现象
			//pThis->m_pDlg->SetLock(3000);
//			std::cout << "check Agent" << std::endl;
            pThis->m_pDlg->m_MapWorkNoToStationLock.acquire();
            pThis->m_CheckAgentLog.Log("FLOW_LOG->CheckAgent Begin");
            std::map<std::string, std::string>::iterator pos = pThis->m_pDlg->m_MapWorkNoToStation.begin();
            //auto pos = pThis->m_pDlg->m_MapStationToAgent.begin();
			while(pos != pThis->m_pDlg->m_MapWorkNoToStation.end())
			{
			    //std::cout << "check agent find agent" << std::endl;

				std::string sStation = pos->second;
				++pos;
				CAgent* pAgent = pThis->m_pDlg->GetAgentFromStation(sStation);
//				pThis->m_pDlg->m_MapStationToAgent.GetNextAssoc(pos,sStation,(CObject*&)pAgent);
                //pAgent = pos->second;
				if(pAgent)
				{
					//添加判断：是否为手机客户端登录，若手机登录则不检测心跳
					if (pAgent->m_strSource == "TelSoftPhone")
					{
						pThis->m_CheckAgentLog.Log("FLOW_LOG->座席%s为手机客户端登录，不检测心跳",pAgent->sStation.c_str());
					}
					else if(pAgent->m_strSource == "C_WEB")
					{
					    pThis->m_CheckAgentLog.Log("FLOW_LOG->座席%s为C_WEB端登录，不检测心跳",pAgent->sStation.c_str());
					}
					else if(pAgent->m_strSource == "S_WEB")
					{
					    pThis->m_CheckAgentLog.Log("FLOW_LOG->座席%s为S_WEB端登录，不检测心跳",pAgent->sStation.c_str());
					}
					else if(pAgent->m_strSource == "ivr")
					{
					    pThis->m_CheckAgentLog.Log("FLOW_LOG->座席%s为IVR端登录，不检测心跳",pAgent->sStation.c_str());
					}
					else if(pAgent->m_strSource == "SoftPhone")
					{
					    long int timeNow = time(NULL);
						ACE_Date_Time currTime;
//						CString sCurTime= currTime.Format("%H:%M:%S");
                        std::string sCurTime = CUntiTool::GetCurrTimeHMS(&currTime);
//						CString sPreTime = pAgent->preLiveTime.Format("%H:%M:%S");
                        std::string sPreTime = CUntiTool::GetCurrTimeHMS(&(pAgent->preLiveTime));
						int totalSeconds = timeNow - pAgent->preLiveTimeSecond;

						pThis->m_CheckAgentLog.Log("FLOW_LOG->检测分机%s,当前时间 %s 上一次心跳时间 %s SpanTime=%d",
                                 pAgent->sStation.c_str(),
                                 sCurTime.c_str(),
                                 sPreTime.c_str(),
                                 totalSeconds);
						if(totalSeconds>120)//如果超过120 没有收到再次的心跳，则发出签出的命令
						{//
							pThis->m_CheckAgentLog.Log("FLOW_LOG->座席%s心跳超时,强制签出",pAgent->sStation.c_str());

							pAgent->lock();
							pAgent->sAction = "Logout";
							pAgent->unlock();

							pAgent->LogOut();//正常签出: 不对签出进行限制
						}
					}
					else
					{
					    pThis->m_CheckAgentLog.Log("ERROR_LOG->座席%s未知端登录，不检测心跳",pAgent->sStation.c_str());
					}
                    pAgent->Release();
				}
			}
			pThis->m_CheckAgentLog.Log("FLOW_LOG->CheckAgent End");
			pThis->m_pDlg->m_MapWorkNoToStationLock.release();
			//pThis->m_pDlg->SetUnLock();
		}
	}
	return NULL;
}
//检查函数
void CCheckAgent::run()
{

}
