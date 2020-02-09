#include "UntiTool.h"
#include <uuid/uuid.h>
#include <netdb.h>
#include "StdString.h"



CUntiTool::CUntiTool()
{

}

CUntiTool::~CUntiTool()
{

}

std::string CUntiTool::GetStationFromMemberName(const std::string &sDoc)
{
    int nLen = sDoc.length();
	size_t n1 = sDoc.find("/",0);
	size_t n2 = sDoc.find("@",n1+1);// /改成@

	if(n2>0)
	{
		return sDoc.substr(n1+1,n2-n1-1);
	}
	else if(n1>0 && n2<0)
	{
		return sDoc.substr(sDoc.length()-(nLen-n1-1));
	}
	else
	{
		return "";
	}
}


std::string CUntiTool::GetStationFromChannel(const std::string &sChan)
{
    size_t n1 = sChan.find("/",0);

    size_t n2 = sChan.find_first_of("-");


	std::string sRet;
	if(n1 != std::string::npos && n2 != std::string::npos)
	{
        sRet = sChan.substr(n1+1,n2-n1-1);
	}
	return sRet;
}

std::string CUntiTool::GetStateionFromDialString(const std::string &sDialString)
{
    std::string sRet = sDialString;
	size_t n;
	if((n = sDialString.find("@")) != std::string::npos)
	{
		sRet = sDialString.substr(0, n);
	}
	return sRet;
}

void CUntiTool::DecodeString(const std::string &source, std::vector<std::string> &dest, char division)
{
	size_t  index = 0;
	size_t  ret;
	dest.clear();
	while ( true )
	{
		ret = source.find(division, index);
		if (ret==std::string::npos)
		{
            dest.push_back(source.substr(index));
			return;
		}

		dest.push_back(source.substr(index, ret-index));
		index = ret+1;
	}
}

BOOL CUntiTool::ParaXML(const std::string &sDoc, const std::string &sKey, std::string& val)
{
    std::string sStartKey = "";
	std::string sEndKey = "";

	sStartKey = "&lt;" + sKey + "&gt;";
	sEndKey = "&lt;/" + sKey + "&gt;";

	size_t nStart = sDoc.find(sStartKey);
	size_t nEnd = sDoc.find(sEndKey);

	size_t nSecondStart = nStart+sStartKey.length();

	if(nSecondStart<=sDoc.length())
	    val = sDoc.substr(nSecondStart,nEnd-nSecondStart);

	return TRUE;
}

std::string CUntiTool::GetDnisFromDialString(const std::string &sDialString)
{
    std::string sRet ;
    size_t nFindPos = sDialString.find("@");
	if(nFindPos>0)
	{
		sRet = sDialString.substr(0, nFindPos);
	}
	else
	{
		sRet = sDialString;
	}
	return sRet;
}

std::string CUntiTool::GetCurrTime()
{
    ACE_Date_Time t;

    std::string strTime;
	char str[5];

	memset(str, 0, 5);
	snprintf(str, 5, "%ld", t.year());
	strTime += str;
	strTime += "-";

	memset(str, 0, 5);
	snprintf(str, 5, "%ld", t.month());
	strTime += str;
	strTime += "-";

	memset(str, 0, 5);
	snprintf(str, 5, "%ld", t.day());
	strTime += str;
	strTime += " ";

	memset(str, 0, 5);
	snprintf(str, 5, "%ld", t.hour());
	strTime += str;
	strTime += ":";

	memset(str, 0, 5);
	snprintf(str, 5, "%ld", t.minute());
	strTime += str;
	strTime += ":";

	memset(str, 0, 5);
	snprintf(str, 5, "%ld", t.second());
	strTime += str;

    return strTime;
}

std::string CUntiTool::GetCurrTimeHMS(const ACE_Date_Time *rh)
{
    ACE_Date_Time t;

    if(rh)
    {
        t = *rh;
    }

    std::string strTime;
	char str[5];

	memset(str, 0, 5);
	snprintf(str, 5, "%ld", t.hour());
	strTime += str;
	strTime += ":";

	memset(str, 0, 5);
	snprintf(str, 5, "%ld", t.minute());
	strTime += str;
	strTime += ":";

	memset(str, 0, 5);
	snprintf(str, 5, "%ld", t.second());
	strTime += str;

    return strTime;
}

ACE_Date_Time CUntiTool::GetCurrTimeDate()
{
    return ACE_Date_Time();
}

BOOL CUntiTool::GetStationFromAction(const std::string &sEvt, std::string& sValName, std::string& sStation)
{
	int n1 = sEvt.find("|",0);
	int n2 = sEvt.find("|",n1+1);
	int n3 = sEvt.find("|",n2+1);
	if(n1!=-1 && n2!=-1 && n3!=-1)
	{
		sValName = sEvt.substr(0, n1);
	    sStation = sEvt.substr(n1+1,n2-n1-1);
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CUntiTool::GetAgentInfoFromHoldVal(const std::string &sEvt, std::string& sStation, std::string& sAction, std::string &Val)
{
	int n1 = sEvt.find("|",0);
	int n2 = sEvt.find("|",n1+1);
	if(n1!=-1 && n2!=-1 )
	{
		Val = sEvt.substr(0, n1);
		sAction = sEvt.substr(n1+1,n2-n1-1);
		sStation = sEvt.substr(n2+1);
		return TRUE;
	}
	else
		return FALSE;
}

std::string CUntiTool::GenUCID()
{
    uuid_t uu;
    uuid_generate(uu);
    char guid[37];
    uuid_unparse(uu, guid);
    return std::string(guid);
}

void CUntiTool::ParseString(std::string str,char div,std::list<std::string> &li)
{
	int prePos = 0;
	int pos = 0;
	std::string sub;

	pos = str.find(div,prePos);
	
	while(pos>=0)
	{
		sub = str.substr(prePos,pos-prePos);
		li.push_back(sub);
		prePos = pos+1;
		pos = str.find(div,prePos);
	}

	if(pos<0 && !str.empty())
	{
		sub = str.substr(prePos,str.length()-prePos+1);
		li.push_back(sub);
	}

}



void CUntiTool::ParseString(std::string str,char div,std::map<std::string,std::string> & mp)
{
	int subPos = str.find(div);
	std::string key = str.substr(0,subPos);
	std::string val = str.substr(subPos+1,str.length()-subPos);
	std::pair<std::string,std::string> p;
	p = std::make_pair(key,val);
	mp.insert(p);
}

std::string CUntiTool::getValidChanFromOrigChan(const std::string &origChan)
{
	CStdString src(origChan);
	int pos1 = src.Find("AsyncGoto/");
	if(pos1>=0)
	{
		pos1 = pos1+strlen("AsyncGoto/");
	}
	else
	{
		pos1 = 0;
	}

	int pos2 = src.Find("<ZOMBIE>");
	if(pos2<0)
	{
		pos2 = src.GetLength();
	}
	
	std::string ret;
	ret  = src.Mid(pos1,pos2-pos1).c_str();
	return ret;

}

char* CUntiTool::GetLocalAddr()
{
	char szName[256];
	char* addrIP;
	memset(szName,0,256);
	struct in_addr addr;
	struct hostent * host;
	
	if(gethostname(szName,256) == 0)
	{
		host = gethostbyname(szName);
		if(host->h_addr_list[0])
		{
			memmove(&addr,host->h_addr_list[0],4);
			addrIP = inet_ntoa(addr);
		}
	}	
	return addrIP;
}











