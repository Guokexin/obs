#ifndef SETTINGDATA_H
#define SETTINGDATA_H

#include <iostream>
#include <string>
#include <map>
typedef unsigned int UINT;

class CSettingData
{
public:
	CSettingData(){
	   m_strPBX = "";//服务器名
	   m_strPbxUsr = "asterisk";//用户名
	   m_strPbxPwd = "asterisk";//密码
	   m_strPbxPort = "5038";//pbx端口
	   m_strProxyPort = "6001";//Proxy端口
	   m_strlistenNum = "200";
//	   m_strTimeOut = "";

	   m_strInPBX = "";
	   m_strSupportVersion = "";

	};
	virtual ~CSettingData();
public:
	std::string  m_strPBX;//服务器名
	std::string  m_strPbxUsr;//用户名
	std::string  m_strPbxPwd;//密码
	std::string  m_strPbxPort;//pbx端口

	std::string m_strProxyPort;//Proxy端口
	std::string m_strlistenNum;//监听数
	std::map<std::string,std::string> m_mapNoType;
	UINT     m_nAgentCount;

	std::string m_strInPBX;//接入PBX
	std::string m_strLoginFromWeb;
	std::string m_strSupportVersion;

public:


	void  ReadCfg(const std::string &strPath);

	void ReadAgentCfg(const std::string &strPath);
	void WriteAgentCfg(std::string strPath);
	void SetAsteriskName(std::string sAsteriskName);
	std::string GetAsteriskName();
	void SetAsteriskPwd(std::string sPwd);
	void SetAsteriskServer(std::string sServer);
	void SetAsteriskPort(std::string sPort);
};

#endif
