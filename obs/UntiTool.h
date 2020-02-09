#ifndef UNTITOOL_H
#define UNTITOOL_H

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <list>
#include <ace/Date_Time.h>
#include "owntype.h"

struct CActionID
{
	std::string sValName;
	std::string sStation;
	std::string sAction;
};

class CUntiTool
{
public:
	CUntiTool();
	virtual ~CUntiTool();

public:
	std::string GetStationFromMemberName(const std::string &sDoc);
	std::string GetStationFromChannel(const std::string &sChan);
	std::string GetStateionFromDialString(const std::string &sDialString);
	void    DecodeString(const std::string &source, std::vector<std::string> &dest, char division);
	BOOL    ParaXML(const std::string &sDoc, const std::string &sKey, std::string& val);
	std::string GetDnisFromDialString(const std::string &sDialString);

	static std::string GetCurrTime();
	static std::string GetCurrTimeHMS(const ACE_Date_Time *rh=NULL);
	static ACE_Date_Time GetCurrTimeDate();
//      CString GBKToUTF8(CString strGBK);
//	    CString UTF8ToGBK(CString strUTF8);

	BOOL GetStationFromAction(const std::string &sEvt, std::string& sValName, std::string& sStation);
	BOOL GetAgentInfoFromHoldVal(const std::string &sEvt, std::string& sStation, std::string& sAction, std::string &Val);

	void ParseString(std::string str,char div,std::list<std::string> &li);
	void ParseString(std::string str,char div,std::map<std::string,std::string> &mp);
	

	static std::string GenUCID();
	static std::string getValidChanFromOrigChan(const std::string &orgiChan);
	
	static char* GetLocalAddr();
};

#endif
