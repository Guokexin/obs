#include "AskProxyDlg.h"
#include "AskMsg/XmlMarkup.h"
//#include "WS.h"


CAskProxyDlg::CAskProxyDlg()
{
    m_bCheckLog = FALSE;
    //初始化日志文件
    log4cxx::PropertyConfigurator::configure("./Config/AskProxyLogConfig.log");

    m_ReportLog.Init("ReportLog");
    m_PBXLog.Init("PBXDATALOG");
    m_Log.Init("AskProxy");
    m_keepLive.Init("keepalive");
    m_AgentLog.Init("AgentAMI");
    m_skillSnapLog.Init("SkillSnap");


    m_strAgentStatusWeb = "";
    m_strCdrWeb = "";
    m_strskillRecordWeb = "";
    m_strskillSnapWeb = "";
    m_strResportServer = ""; //报表AskProxyTOCM的IP
    m_strResportPort = ""; //报表AskProxyTOCM的Port
    m_strWSConn = "";
    m_strIsStationLogin = "";//分机是否允许登录的http验证串
    m_strdialAgentCallerNumber = "";//通过租户id获取平台拨座席的主叫号码
    m_strOverTime = "2000";	//超时设置

    m_strPBXInfo = "";//获取所有的租户和技能组
    m_strWSSipExt = "";//add by ly
    m_strGetStationType = "";//获取分机类型 sip skill agent others(pstn)
    m_strConnAgentPwd = "";


    m_bIsSopping = FALSE;
    m_RegAction.pdlg = this;
    m_SockManager.m_pdlg = this;
//	pthread_mutex_init(&m_PBXMsgCritical, NULL);
//	pthread_mutex_init(&m_AgentLock, NULL);
    m_obsCount = 0;
}

BOOL CAskProxyDlg::ReadWSCfg(const std::string &sPath)
{
    CMarkup xml;
    xml.Load(sPath);

    xml.FindElem("WS_ROOT");
    xml.IntoElem();

    bool bFind = xml.FindElem("WSOVERTIME");//座席
    if(bFind)
    {
        //超时设置
        m_strOverTime = xml.GetAttrib("value");
    }
    else
    {
        xml.AddElem("WSOVERTIME");
        xml.SetAttrib("value","");
    }

    bFind = xml.FindElem("QUEUETIME");
    if(bFind)
    {
        m_iGetQueueTime = atoi(xml.GetAttrib("value").c_str());
    }
    else
    {
        m_iGetQueueTime = 300;
    }

    bFind = xml.FindElem("WSAgentInfo");//座席
    if(bFind)
    {
        m_strWSConn = xml.GetAttrib("Conn");
    }
    else
    {
        xml.AddElem("WSAgentInfo");
        xml.SetAttrib("Conn","");
    }

    //add by ly begin
    bFind = xml.FindElem("WSSipExtInfo");//分机
    if (bFind)
    {
        m_strWSSipExt = xml.GetAttrib("Conn");
    }
    else
    {
        xml.AddElem("WSSipExtInfo");
        xml.SetAttrib("Conn","");
    }
    //add end

    bFind =xml.FindElem("WSIsStationLoginInfo");
    if (bFind)
    {
        m_strIsStationLogin = xml.GetAttrib("Conn");
    }
    else
    {
        xml.AddElem("WSIsStationLoginInfo");
        xml.SetAttrib("Conn","");
    }

    bFind = xml.FindElem("WSdialAgentCallerNumberInfo");
    if (bFind)
    {
        m_strdialAgentCallerNumber=xml.GetAttrib("Conn");
    }
    else
    {
        xml.AddElem("WSdialAgentCallerNumberInfo");
        xml.SetAttrib("Conn","");
    }




    //bug: 配置文件的增加需要通过该种方式实现-即xml文件中在所有配置文件的末尾添加
    bFind = xml.FindElem("WSGetStationType");
    if (bFind)
    {
        m_strGetStationType=xml.GetAttrib("Conn");
    }
    else
    {
        xml.AddElem("WSGetStationType");
        xml.SetAttrib("Conn","");
    }

    bFind = xml.FindElem("WSPBXInfo");//负载PBX
    if (bFind)
    {
        m_strPBXInfo = xml.GetAttrib("Conn");
    }
    else
    {
        xml.AddElem("WSPBXInfo");
        xml.SetAttrib("Conn","");
    }


    bFind = xml.FindElem("WSPBXAgentPwd");
    if(bFind)
    {
        m_strConnAgentPwd = xml.GetAttrib("Conn");
    }

    bFind = xml.FindElem("ResportServer");
    if(bFind)
    {
        m_strResportServer = xml.GetAttrib("IP");
        m_strResportPort = xml.GetAttrib("Port");
    }
    else
    {
        xml.AddElem("ResportServer");
        xml.SetAttrib("IP","");
        xml.SetAttrib("Port","");
    }


    bFind = xml.FindElem("WS");
    if(bFind)
    {
        xml.IntoElem();
        bool bfind2=xml.FindElem("AgentStatus");
        if(bfind2)
        {
            m_strAgentStatusWeb = xml.GetAttrib("para");
        }
        else
        {
            xml.AddElem("AgentStatus");
            xml.SetAttrib("para","");
        }

        bfind2=xml.FindElem("cdr");
        if(bfind2)
        {
            m_strCdrWeb = xml.GetAttrib("para");
        }
        else
        {
            xml.AddElem("cdr");
            xml.SetAttrib("para","");
        }

        bfind2=xml.FindElem("skillRecord");
        if(bfind2)
        {
            m_strskillRecordWeb = xml.GetAttrib("para");
        }
        else
        {
            xml.AddElem("skillRecord");
            xml.SetAttrib("para","");
        }

        bfind2=xml.FindElem("skillSnap");
        if(bfind2)
        {
            m_strskillSnapWeb = xml.GetAttrib("para");
        }
        else
        {
            xml.AddElem("skillSnap");
            xml.SetAttrib("para","");
        }

        bfind2 = xml.FindElem("AgentActualTimeStatus");
        if (bfind2)
        {
            m_strAgentActualTimeStatus = xml.GetAttrib("para");
        }
        else
        {
            xml.AddElem("AgentActualTimeStatus");
            xml.SetAttrib("para","");
        }
        xml.OutOfElem();
    }
    else
    {
        xml.AddElem("WS");
    }

    xml.OutOfElem();
    xml.Save(sPath);
    return TRUE;
}

bool CAskProxyDlg::OnStart()
{
    m_bIsSopping = FALSE;

    m_Log.Log("==============版本号 V3.1.0A 启动==============");
    m_Log.Log("Askterisk Proxy Start");

    m_SettingData.ReadCfg("./Config/AskProxyCfg.xml");
    ReadWSCfg("./Config/WS_Conf.xml");

    //获取所有的租户和技能组
 //   CWS ws(this);
//    std::string sPBXInfoSend = m_strPBXInfo + "ip=" + m_SettingData.m_strPBX;
//	sPBXInfoSend.Format("%sip=%s",m_strPBXInfo,m_SettingData.m_strPBX);
  //  BOOL bret = ws.SendRequestPBXInfo(sPBXInfoSend);
    //BOOL bret=TRUE;//test
   // if (bret==FALSE)
   // {
        //无法获取到技能组和租户信息
   //     return false;
   // }

    //连接负载PBX
    bool bSuccess;
    m_SockManager.SetErrorThreadStart();//获取类错误日志
    bSuccess = m_SockManager.ConnectTo(m_SettingData.m_strPBX,
                                       m_SettingData.m_strPbxPort,
                                       SOCK_STREAM); // TCP
    if(bSuccess)
    {
        bool bWatch = m_SockManager.WatchComm();
        if(bWatch)
        {
            m_Log.Log("listen asterisk succ111");
        }
        m_Log.Log("Connect astersik  Succ");
    }
    else
    {
        m_Log.Log("Cann't connect to asterisk");
        return false;
    }

    BOOL bCreate=m_pSendSocket.ConnectTo(m_strResportServer,m_strResportPort,SOCK_DGRAM);
    //std::cout << "chandwebresponse ip: " << m_strResportServer << "   port: " << m_strResportPort << std::endl;
    if(!bCreate)
    {
        m_Log.Log("FLOW_LOG-> Craete send socket fail");
        return false;
    }

    m_HandleAskPBX.StartHandleAskPBX(this);

    m_HandleSoftPhoneThread.StartHandleSoftPhoneMsg(this);
    //m_HandleSoftPhoneThread.StartDispatchSoftPhoneMsg(this);

    //m_HandleWS.StartHandleWebResponseThread(this);

    //m_HandleWSQuery.StartQueryThread(this);


    //m_CheckAgent.SetWnd(this);
    //m_CheckAgent.StartCheckAgentThread();

    BOOL b = m_IOOpt.Init(this);

    if(b)
    {
        //注册
        m_RegAction.RegEnCode();
        sleep(5);
       // StartLogin();
    }
    else
    {
        return false;
    }

    return true;
}
/*
void CAskProxyDlg::StartLogin()
{
    ACE_thread_t id;
    ACE_Thread_Manager::instance()->spawn(CAskProxyDlg::RegisterLogin,
                                          this,
                                          (THR_NEW_LWP|THR_JOINABLE|THR_INHERIT_SCHED|THR_SUSPENDED),
                                          &id
                                         );
}
*/
/*
void* CAskProxyDlg::RegisterLogin(void *par)
{
    std::cout << "start error reset" << std::endl;
    CAskProxyDlg *dlg = reinterpret_cast<CAskProxyDlg*>(par);
    std::string ret;
    std::cout << dlg->m_SettingData.m_strLoginFromWeb << "\n" << dlg->m_strOverTime << std::endl;
    if(CHttp::request(dlg->m_SettingData.m_strLoginFromWeb, dlg->m_strOverTime, ret))
    {
        std::cout << ret << std::endl;
        size_t index = ret.find("&lt;AskPT&gt;");
        if(index != std::string::npos)
        {
            ret.erase(0, index);
        }

        index = ret.find("&lt;/AskPT&gt;");
        if(index != std::string::npos)
        {
            ret.erase(index+strlen("&lt;/AskPT&gt;"));
        }

        index = ret.find("&lt;");
        while(index != std::string::npos)
        {
            ret.replace(index, 4, "<");
            index = ret.find("&lt;");
        }

        index = ret.find("&gt;");
        while(index != std::string::npos)
        {
            ret.replace(index, 4, ">");
            index = ret.find("&gt;");
        }

        CMarkup xml;
        xml.SetDoc(ret.c_str());
        if(!xml.FindElem("AskPT"))
            return NULL;
        xml.IntoElem();

        std::string strSource;
        std::string strAction;

        if(xml.FindElem("Source"))
            strSource = xml.GetAttrib("Value");
        else
        {
            return NULL;
        }

        if(!xml.FindElem("Action"))
        {
            return NULL;
        }
        strAction = xml.GetAttrib("Value");

        if(strAction == "")
        {
            return NULL;
        }

        if(!xml.FindElem("Body"))
        {
            return NULL;
        }
        xml.IntoElem();

        if(strSource == "C_WEB")
        {
            bool bFind = xml.FindElem("SP");

            while(bFind)
            {
                std::string content = xml.GetElemContent();
                xml.IntoElem();

                if(!xml.FindElem("Station"))
                {
                    xml.OutOfElem();
                    bFind = xml.FindElem("SP");
                    continue;
                }

                std::string strStation = xml.GetElemContent();
                if(strStation.empty())
                {
                    xml.OutOfElem();
                    bFind = xml.FindElem("SP");
                    continue;
                }

                if(!xml.FindElem("Addr"))
                {
                    xml.OutOfElem();
                    bFind = xml.FindElem("SP");
                    continue;
                }

                std::string strAddr = xml.GetElemContent();
                if(strAddr.empty())
                {
                    xml.OutOfElem();
                    bFind = xml.FindElem("SP");
                    continue;
                }

                std::string strQuest = "<?xml version=\"1.0\" encoding=\"utf-8\" ?><AskPT><Source  Value=";
                strQuest += strSource;
                strQuest += " /><Action  Value=";
                strQuest += strAction;
                strQuest += " /><Body>";
                strQuest += content;
                strQuest += "</Body></AskPT>\r\n\r\n";

                //SENDTOCTI
                AgentMsg* msg = new AgentMsg;
                msg->s = dlg->m_HandleSoftPhoneThread.m_webSock;
                msg->sMsg = strQuest;
                dlg->m_Log.Log("FLOW_LOG->启动故障恢复: %s", strQuest.c_str());
                dlg->m_SoftPhoneMsgCritical.acquire();
                dlg->m_AgentMsgList.push_back(msg);
                dlg->m_SoftPhoneMsgCritical.release();
                dlg->m_HandleSoftPhoneThread.m_hStartHandleSoftPhoneMsg.release();
                xml.OutOfElem();
                bFind = xml.FindElem("SP");
            }
        }
    }

    return NULL;
}
*/
CAgent* CAskProxyDlg::GetAgentFromStation(const std::string &sStation)
{
    CAgent *pAgent = NULL;
    //m_MapStationToAgentLock.acquire();
    SetLock();
    std::map<std::string, CAgent*>::iterator it = m_MapStationToAgent.find(sStation);
    //auto it = m_MapStationToAgent.find(sStation);
    if(it != m_MapStationToAgent.end())
    {
        pAgent = it->second;
        pAgent->AddRef();
    }
    SetUnLock();
    return pAgent;
}

void CAskProxyDlg::SetAgentToStation(const std::string &sStation)
{
    //m_MapStationToAgentLock.acquire();
    SetLock();
    CAgent *pAgent = new CAgent();
    if(pAgent)
    {
        m_MapStationToAgent[sStation] = pAgent;
        pAgent->AddRef();
    }

    //std::cout << "set agent to station: " << pAgent->AddRef() << std::endl;
    //m_MapStationToAgentLock.release();
    SetUnLock();
}

CAgent* CAskProxyDlg::GetAgentFromChan(const std::string &sChan)
{
    CAgent *pAgent = NULL;
    //m_mapChanToAgentLock.acquire();
    SetLock();
    std::map<std::string, CAgent*>::iterator it = m_MapChanToAgent.find(sChan);
    //auto it = m_MapChanToAgent.find(sChan);
    if(it != m_MapChanToAgent.end())
    {
        pAgent = it->second;
        pAgent->AddRef();
    }

    //m_mapChanToAgentLock.release();
    SetUnLock();
    return pAgent;
}

BOOL CAskProxyDlg::SetWorkNoToMap(CAgent* pAgent)
{
    m_MapWorkNoToStationLock.acquire();
    m_MapWorkNoToStation[pAgent->sWorkNo] = pAgent->sStation;
    m_MapWorkNoToStationLock.release();
    return TRUE;
}

BOOL CAskProxyDlg::AgentIsLogin(const std::string &sagentid, std::string& sStation)
{
//    pthread_mutex_lock(&m_MapWorkNoToStationLock);
    m_MapWorkNoToStationLock.acquire();
    std::map<std::string, std::string>::iterator it = m_MapWorkNoToStation.find(sagentid);
    //auto it = m_MapWorkNoToStation.find(sagentid);
    BOOL ret = TRUE;
    if(it != m_MapWorkNoToStation.end())
    {
        sStation = it->second;
    }
    else
    {
        ret = FALSE;
    }
//    pthread_mutex_unlock(&m_MapWorkNoToStationLock);
    m_MapWorkNoToStationLock.release();
    return ret;
}

BOOL CAskProxyDlg::RemoveAgentFromMap(CAgent* pAgent)
{
    //m_MapStationToAgentLock.acquire();
    //std::map<std::string, CAgent*>::iterator it1 = m_MapStationToAgent.find(pAgent->sStation);
    //auto it1 = m_MapStationToAgent.find(pAgent->sStation);
    //m_MapStationToAgentLock.release();

    BOOL bRemove1 = m_MapStationToAgent.erase(pAgent->sStation);
    pAgent->Release();
//    if(it1 != m_MapStationToAgent.end())
//    {
//        bRemove1 = TRUE;
//        m_MapStationToAgent.erase(it1);
//        pAgent->Release();
//    }

//    BOOL bRemove2 = FALSE;

    m_MapWorkNoToStationLock.acquire();
    //std::map<std::string, std::string>::iterator it2 = m_MapWorkNoToStation.find(pAgent->sWorkNo);
//    auto it2 = m_MapWorkNoToStation.find(pAgent->sWorkNo);
//    if(it2 != m_MapWorkNoToStation.end())
//    {
//        bRemove2 = TRUE;
//        m_MapWorkNoToStation.erase(it2);
//    }
    BOOL bRemove2 = m_MapWorkNoToStation.erase(pAgent->sWorkNo);
    m_MapWorkNoToStationLock.release();

//    m_mapChanToAgentLock.acquire();
    std::map<std::string, CAgent*>::iterator it3;
    CAgent *pAgent1 = NULL;
    bool remove = false;
    for(it3=m_MapChanToAgent.begin(); it3!=m_MapChanToAgent.end(); )
    {
        pAgent1 = it3->second;
        if(pAgent1 == pAgent)
        {
            m_MapChanToAgent.erase(it3++);
            pAgent1->Release();
            remove = true;
        }
        if(remove)
            remove = false;
        else
            ++it3;

    }
//    m_mapChanToAgentLock.release();

    if(bRemove1 && bRemove2)
    {
        return TRUE;
    }
    else
        return FALSE;
}

void CAskProxyDlg::SetLock(int v)
{
//    pthread_mutex_lock(&m_AgentLock);
    ACE_Time_Value t(time(NULL) + v/1000, 0);
    m_AgentLock.acquire(&t);
}

void CAskProxyDlg::SetUnLock()
{
//    pthread_mutex_unlock(&m_AgentLock);
    m_AgentLock.release();
}

void CAskProxyDlg::SetAgentToChan(const char *szChan,CAgent* pAgent)
{
    SetLock();
    //m_MapChanToAgent.SetAt(szChan,(CObject*&)pAgent);
    m_MapChanToAgent[szChan] = pAgent;
    pAgent->AddRef();
    SetUnLock();

}

BOOL CAskProxyDlg::RemoveChanFromMap(const char *szChan,CAgent* pAgent)
{
    BOOL bRemove = m_MapChanToAgent.erase(szChan);

    if(bRemove)
    {
        pAgent->Release();
    }
    return bRemove;
}




