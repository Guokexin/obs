#include "SettingData.h"
#include "AskMsg/XmlMarkup.h"
#include "owntype.h"
#include <iostream>
CSettingData::~CSettingData()
{

}

void CSettingData::ReadCfg(const std::string &strPath)
{
    CMarkup mark;
    mark.Load(strPath);

	bool bFind = mark.FindElem("askproxy");
	if(bFind)
	{
		mark.IntoElem();
		bFind = mark.FindElem("pbx");
		if(bFind)
		{
			m_strPBX = mark.GetAttrib("pbxip");
			m_strPbxUsr = mark.GetAttrib("pbxusr");
			m_strPbxPwd = mark.GetAttrib("pbxpwd");
			m_strPbxPort = mark.GetAttrib("pbxport");
		}

		bFind = mark.FindElem("proxy");
		if(bFind)
		{
			m_strProxyPort = mark.GetAttrib("port");
			m_strlistenNum = mark.GetAttrib("listen");
		}
		bFind = mark.FindElem("inpbx");
		if (bFind)
		{
			m_strInPBX = mark.GetAttrib("inpbxip");
		}

		bFind = mark.FindElem("loginformweb");
               if(bFind)
               {
            		m_strLoginFromWeb = mark.GetAttrib("loginurl");
            		REPLACE(m_strLoginFromWeb, "[addr]", m_strPBX);
        	}

		bFind = mark.FindElem("support_version");
		if(bFind)
		{
			m_strSupportVersion = mark.GetElemContent();
		}
		bFind = mark.FindElem("pbx_no");
		if(bFind)
		{
			mark.IntoElem();
			while(mark.FindElem("ID"))
			{
				std::string no = mark.GetAttrib("no");
				std::string type = mark.GetAttrib("type");
				std::pair<std::string,std::string> p = std::make_pair(no,type);
				m_mapNoType.insert(p);
				std::cout<<"no"<<no.c_str()<<std::endl;
				std::cout<<"type"<<type.c_str()<<std::endl;
				
			}
		}
	}
}

void CSettingData::ReadAgentCfg(const std::string &strPath)
{
	CMarkup mark;
	bool bLoad = mark.Load(strPath);
	mark.FindElem("ProxyStation");
	mark.IntoElem();
	if(bLoad)
	{
		bool bFind = mark.FindElem("StationCount");
		if(bFind)
		{
			std::string strCount = mark.GetData();
			m_nAgentCount = atoi(strCount.c_str());
			//strCount.ReleaseBuffer(-1);
		}

		bFind = mark.FindElem("StationInfo");
		if(bFind)
		{
			mark.IntoElem();
			while(mark.FindElem("Station"))
			{
				std::string sID = mark.GetAttrib("id");
				std::string sStation = mark.GetAttrib("Station");
			//	m_MapStationToIndex.SetAt(sStation,sID);
			}
		}
	}
}

