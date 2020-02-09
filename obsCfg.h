#ifndef _OBS_CFG_H_
#define _OBS_CFG_H_

#include "StdString.h"




class COBSCfg
{
	private:
		static COBSCfg m_singleton;
	public:
		static COBSCfg* Instance();
		void Initialize();

        public:
                COBSCfg() {
                   dbip = "10.168.0.45";
                   dbname = "mag";
                   dbport = 53306;
                   dbuser = "root";
                   dbpwd = "124!@$qweQWE";
                 }
	public:
		CStdString m_sGateWay;
		CStdString m_sLocalIP;

                CStdString dbip;
                CStdString dbname;
                int dbport;
                CStdString dbuser;
                CStdString dbpwd;
	  	
};

#endif
