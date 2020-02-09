#include "HandleUserMsgThread.h"
#include "logManager.h"
#include "Userconn.h"
#include "GeneralUtils.h"
#include "Userconn.h"
#include "MarkUp.h"
#include "Utils.h"
#include "DBConnection.h"
#include "UntiTool.h"
//#include "un_tts.h"

CHandleUserMsgThread CHandleUserMsgThread::m_singleton;


CHandleUserMsgThread* CHandleUserMsgThread::Instance()
{
	return &m_singleton;
}


CHandleUserMsgThread::CHandleUserMsgThread()
{

}

CHandleUserMsgThread::~CHandleUserMsgThread()
{

}

bool CHandleUserMsgThread::Initialize()
{
	CLogManager* pLog = CLogManager::Instance();
	pthread_mutex_init(&mutex,NULL);

    if(::pthread_create(&m_pid, NULL, CHandleUserMsgThread::StartThread, this) != 0)
    {
		pLog->m_ctiLog.Log("thread work create err : %s",strerror(errno));
        return false;
	}
	
    if(::pthread_create(&m_obsPid, NULL, CHandleUserMsgThread::StartOBSThread, this) != 0)
    {
	pLog->m_ctiLog.Log("thread obs create err : %s",strerror(errno));
        return false;
    }	 

    if (::pthread_create(&m_readdb_pid, NULL, CHandleUserMsgThread::StartReadDThread, this) != 0) {
      pLog->m_rootLog.Log(" thread readdb create err, %s", strerror(errno));
      return false;
    } 

    pLog->m_ctiLog.Log("handle user msg thread started");
    return true;
}


void* CHandleUserMsgThread::StartReadDThread(void* lpvoid) {
  CHandleUserMsgThread* pThis = reinterpret_cast<CHandleUserMsgThread*>(lpvoid);
  pThis->ReadDB();
}


void CHandleUserMsgThread::ReadDB() {
  CLogManager* pLog = CLogManager::Instance();
  pLog->m_rootLog.Log("ReadDB");
  CDBConnection* pDB = CDBConnection::Instance();

  while (true) {
    CStdString sql ;
    sql.Format("select * from t_1_customer where status != 'Y' and call_time < 3");
    ResultSet* res = pDB->ExecSelect(sql,"");
    int nSize = res->rowsCount();
    pLog->m_rootLog.Log(" rows size = %d", nSize);
    while (res->next())
    {
      if (m_phoneMap.size() > 3) {
        sleep(1);
        continue;
      } 

      CUntiTool tool;
      int project_id = res->getInt("project_id");
      std::string phone = res->getString("phone");
      std::string name = res->getString("name");
      std::string id = tool.GenUCID(); 

      CPhone* pPhone = new CPhone();
      pPhone->cmd = "VoiceCampaign";
      pPhone->ucid = id;
      pPhone->caller = "075582493381";
      pPhone->no = phone.c_str();
      pPhone->fd = 0;
      pPhone->source = "DB";
      
      pPhone->name = name;
      pthread_mutex_lock(&mutex);
      if (phone == "18611380324") {
        pLog->m_rootLog.Log("add name %s phone %s to obs queue", name.c_str(), phone.c_str());
        m_phoneList.push_back(pPhone);	
      }
      pthread_mutex_unlock(&mutex);
    }
    //wait 10m
    pLog->m_rootLog.Log(" current obs end, wait next time");
    sleep(600);
  }
}






void* CHandleUserMsgThread::StartThread(void* lpvoid)
{
	CHandleUserMsgThread* pThis = reinterpret_cast<CHandleUserMsgThread*>(lpvoid);
	
	pThis->WorkThread();	
	
	return 0;
}



//#include <iconv.h>  
/*
int code_convert(char *from_charset,char *to_charset,char *inbuf,size_t inlen,char *outbuf,size_t outlen)  
{  
        iconv_t cd;  
        int rc;  
        char **pin = &inbuf;  
        char **pout = &outbuf;  
  
        cd = iconv_open(to_charset,from_charset);  
        if (cd==0)  
                return -1;  
        memset(outbuf,0,outlen);  
        if (iconv(cd,pin,&inlen,pout,&outlen) == -1)  
                return -1;  
        iconv_close(cd);  
        return 0;  
}  
  
int u2g(char *inbuf,int inlen,char *outbuf,size_t outlen)  
{  
        return code_convert("utf-8","gb2312",inbuf,inlen,outbuf,outlen);  
}  
  
int g2u(char *inbuf,size_t inlen,char *outbuf,size_t outlen)  
{  
        return code_convert("gb2312","utf-8",inbuf,inlen,outbuf,outlen);  
}  
*/






void CHandleUserMsgThread::WorkThread()
{
       

	CLogManager* pLog = CLogManager::Instance();
	pLog->m_ctiLog.Log("Handle UserMsg Thread start");
	CUserConn* pUsrConn = CUserConn::Instance();
	m_nCount = 0;
	while(true)
	{
		if(pUsrConn->m_listOBSMsg.size() == 0 || m_phoneMap.size() >= 10)
		{
			sleep(1);
			continue;
		}

		pthread_mutex_lock(&pUsrConn->mutex);
		OBSMsg* obsMsg = pUsrConn->m_listOBSMsg.front();
		pUsrConn->m_listOBSMsg.pop_front();
		pthread_mutex_unlock(&pUsrConn->mutex);
		if(obsMsg != NULL)
		{
			CStdString tMsg = obsMsg->sMsg;
      pLog->m_ctiLog.Log(" rev msg %s", tMsg.c_str());
			int fd = obsMsg->fd;
      CMarkup xml;
      std::string phone,txt,file,id;
      std::string voice_path;
      std::string ak, sk;
      std::string path = "/var/www/html/";
      bool b = xml.SetDoc(tMsg);
	    if(b)
	    {
        id = xml.FindGetData("/request/id");
		    phone = xml.FindGetData("/request/phone/p1");
        txt = xml.FindGetData("/request/voice_txt");
        ak = xml.FindGetData("/request/ak");
        sk = xml.FindGetData("/request/sk");
        pLog->m_ctiLog.Log(" phone %s voice_txt %s", phone.c_str(), txt.c_str());
	    }
      pLog->m_ctiLog.Log(" phone %s ; txt %s ;id %s ; file %s", phone.c_str(), txt.c_str(), id.c_str(), file.c_str()); 
			CPhone* pPhone = new CPhone();
			pPhone->cmd = "VoiceCampaign";
			pPhone->ucid = id;
			pPhone->caller = "075582493381";
			pPhone->voicefile = voice_path;
			pPhone->no = phone.c_str();
			pPhone->fd = fd;
                        std::string name = a2u(txt.c_str());
                        pPhone->name = name;

			pthread_mutex_lock(&mutex);
			m_phoneList.push_back(pPhone);	
			pthread_mutex_unlock(&mutex);

			pLog->m_ctiLog.Log(tMsg.c_str());
			std::cout<<"phonesize : "<<m_phoneMap.size()<<std::endl;
			delete obsMsg;
			sleep(1);
		}
	}
}


void* CHandleUserMsgThread::StartOBSThread(void* lpvoid)
{
	CHandleUserMsgThread* pThis = reinterpret_cast<CHandleUserMsgThread*>(lpvoid);
	pThis->OBSMainThread();
	return 0;
}

void CHandleUserMsgThread::OBSMainThread()
{
  CLogManager* pLog = CLogManager::Instance();
  pLog->m_ctiLog.Log("OBS Main Thread Start"); 
  while(true) {
    pthread_mutex_lock(&mutex);
    int nCount = m_phoneList.size();
    pthread_mutex_unlock(&mutex);
		
    int nMapSize = m_phoneMap.size();
    if(nCount == 0 || nMapSize >=2 ) {
      sleep(1);
      continue;
    }
    
    pthread_mutex_lock(&mutex);
    CPhone* pPhone = m_phoneList.front();
    pthread_mutex_unlock(&mutex);
    if(pPhone != NULL) {
      m_phoneMap[pPhone->no] = pPhone;
      pLog->m_rootLog.Log(" @@@ phoneno %s MakeCall",pPhone->no.c_str());
      pPhone->MakeCall();
      m_phoneList.pop_front();
    }
  }	
}


