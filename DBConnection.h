#ifndef _DB_CONNECTION_H_
#define _DB_CONNECTION_H_
#include "StdString.h"
//#include <mysql/mysql.h> 
#include "connection_pool.h"

class CDBConnection
{
	public:
		static CDBConnection* Instance();
		int Initialize();
		int UnInitialize();
		virtual ~CDBConnection();
	private:
		static CDBConnection m_singleton;
		CDBConnection();
		Connection* CreateConnection();
		void DestoryConnection(Connection* conn);
		
	private:
	//	MYSQL mysql,*sock;
	public:
		CStdString sDBName;
		CStdString sDBPwd;
		CStdString sDBUser;
		CStdString sDBSource;		
	public:
		int connectDB();
		
		ResultSet* ExecSelect(const CStdString sql,const CStdString dbname);
		int ExecInsert(const CStdString sql,const CStdString dbname);
		ResultSet* ExecUpdate(const CStdString sql);
		 
};

#endif
