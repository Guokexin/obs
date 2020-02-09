#include "AgentOp.h"

#include "AskProxy.h"
#include "AgentOp.h"
#include "AskProxyDlg.h"

CAgentOp::CAgentOp()
{

}

CAgentOp::~CAgentOp()
{

}

BOOL CAgentOp::SendResult(CAgent* pAgent,const std::string &sMsg)
{
    CAskProxyDlg* pDlg = GetApp();
    //CAskProxyDlg* pDlg = &theApp;

	pAgent->sSendResult_Data = sMsg;
    BOOL ret = pDlg->m_IOOpt.SendMsgToUser(pAgent->sock,sMsg);
	if(ret)
		return TRUE;
	else
		return FALSE;

    return TRUE;
}


std::string CAgentOp::AddElemToResult(const std::string &sMsg, const std::string &selem_name, const std::string &selem_value)
{

	std::string sReturn = "";
	size_t nfind = sMsg.find("</Body>");
	if(nfind != std::string::npos)
	{
		std::string sleft = sMsg.substr(0, nfind);
		std::string sright = sMsg.substr(nfind);
		std::string s1 = "<" + selem_name + ">" + selem_value + "</" + selem_name + ">";
//		s1.Format("<%s>%s</%s>",selem_name,selem_value,selem_name);
		sReturn = sleft + s1;
		sReturn = sReturn + sright;
	}
	return sReturn;
}
