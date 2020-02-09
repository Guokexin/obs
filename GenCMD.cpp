// GenCMD.cpp: implementation of the CGenCMD class.
//
//////////////////////////////////////////////////////////////////////
#include "GenCMD.h"
#include <stdlib.h>
#include <stdio.h>
#include "UntiTool.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGenCMD::CGenCMD()
{
	m_strCMD = "";
}

CGenCMD::~CGenCMD()
{

}
void CGenCMD::Start()
{
   m_strCMD = "";
}
void CGenCMD::Gen(const std::string &Key, const std::string &Value)
{
    std::string strInfo;
	strInfo += Key;
	strInfo += ":";
	strInfo += " ";
	strInfo += Value;
	strInfo += "\r\n";

	m_strCMD = m_strCMD + strInfo;
}
//通过ACTIONID将实际的动作的带过去
void CGenCMD::End(const std::string &sAction, const std::string &sActionID)
{
    m_strCMD += "ActionID: ";
	m_strCMD += sAction;
	m_strCMD += "|";
	if(sActionID.empty())
	    m_strCMD += GenActionID();
	else//used askproxy and pbx
		m_strCMD = m_strCMD + sActionID;
	m_strCMD += "\r\n\r\n";

}

std::string CGenCMD::GenActionID()
{
	/*BYTE *Uuid;
	GUID  Guid;
	CComBSTR   bstrKey;
	CoCreateGuid(&Guid);
	UuidToString(&Guid,&Uuid);
	std::string strUUID(Uuid);
	m_strActionID = strUUID;
	char buff[20];
	memset(buff, 0, 20);
	snprintf(buff, 20, "%ld", time(NULL));
    return std::string(buff);*/
    return CUntiTool::GenUCID();
}

//void CGenCMD::SendCMD(CSocketManager* sock)
//{
	
//	if(m_strCMD.length()<0)
//		return;
//    std::cout << m_strCMD << std::endl;

//	if(sock!=NULL)
//	    sock->WriteComm(m_strCMD, m_strCMD.length());
//}

std::string   CGenCMD::GetActionID()
{
	return m_strActionID;
}

std::string CGenCMD::GetCmd()
{
	return m_strCMD;
}
