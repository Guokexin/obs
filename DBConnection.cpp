#include "DBConnection.h"
#include "logManager.h"
#include "obsCfg.h"
#include "connection_pool.h"

CDBConnection CDBConnection::m_singleton;

CDBConnection::CDBConnection()
{
	sDBName = "";
	sDBUser = "";
	sDBPwd = "";
	sDBSource = "";
}

CDBConnection::~CDBConnection()
{

}

CDBConnection* CDBConnection::Instance()
{
	return &m_singleton;
}

int CDBConnection::Initialize()
{
	
	CLogManager* pLog = CLogManager::Instance();
	pLog->m_rootLog.Log("DEBUG DConnection Initialize....");
	return 0;
	//ConnPool* connpool = ConnPool::GetInstance();
	//if(connpool){
                
	//	pLog->m_rootLog.Log("get db connection succ");
	//	return 0;
    //    }
	//else{
	//	pLog->m_rootLog.Log("get db connection fail");
	//	return 1;
     //   }
}

int CDBConnection::UnInitialize()
{
	//ConnPool* connpool = ConnPool::GetInstance();
	//if(connpool != NULL)
	//delete connpool;
	return 0;
}

//数据库连接
Connection* CDBConnection::CreateConnection()
{
	CLogManager* pLog = CLogManager::Instance();
	pLog->m_rootLog.Log("ConnPool GetInstance");
	COBSCfg* pCfg = COBSCfg::Instance();
	CStdString server;
	server.Format("tcp://%s:%d",pCfg->dbip, pCfg->dbport);
	CStdString user = pCfg->dbuser;
	CStdString pwd = pCfg->dbpwd;
	Driver*driver;
	try {
		driver = sql::mysql::get_driver_instance();
	} catch (sql::SQLException&e) {
		perror("驱动连接出错;\n");
		pLog->m_rootLog.Log("驱动连接出错!");

	} catch (std::runtime_error&e) {
		perror("运行出错了\n");
		pLog->m_rootLog.Log("运行出错了!");
	}
	Connection*conn;
	try {
		conn = driver->connect(server.c_str(),user.c_str(), pwd.c_str()); //建立连接
		return conn;
	} catch (sql::SQLException&e) {
		perror("创建连接出错");
		pLog->m_rootLog.Log("创建连接出错!");
		return NULL;
	} catch (std::runtime_error&e) {
		perror("运行时出错");
		return NULL;
	}
	
	
}

void CDBConnection::DestoryConnection(Connection* conn) {
	if (conn) {
		try {
			conn->close();
			//std::cout << "conn->close()"<<std::endl;
		} catch (sql::SQLException&e) {
			perror(e.what());
			//std::cout << "sql::SQLException"<<std::endl;
		} catch (std::exception&e) {
			perror(e.what());
			//std::cout << "std::exception"<<std::endl;
		}
		delete conn;
	}
	else
	{
		std::cout << "conn null"<<std::endl;
	}
}

//only one row
ResultSet* CDBConnection::ExecSelect(const CStdString sql,CStdString dbname)
{
	CLogManager* pLog = CLogManager::Instance();
	//ConnPool* connpool = ConnPool::GetInstance();
        COBSCfg* pCtiCfg = COBSCfg::Instance();
	ResultSet* result = NULL;
	Statement* state = NULL;
	Connection*  con = CreateConnection();
	try
	{
		pLog->m_rootLog.Log("execute sql : \r\n%s",sql.c_str());
		//if(connpool)
		//{
			
			if(con)
			{
				state = con->createStatement();	
				if(state)
				{
                    CStdString conn;
                    conn.Format("use %s",pCtiCfg->dbname);
					state->execute(conn);
					result = state->executeQuery(sql);
					state->close();
				}		
				delete state;
				//connpool->ReleaseConnection(con);
				DestoryConnection(con);
			}
		//}		
	}
	catch(...)
	{
		 pLog->m_rootLog.Log("Excute sql %s failed",sql.c_str());
		 state->close();
		 delete state;
		 DestoryConnection(con);
		return NULL;

	}
	return result;
}

int CDBConnection::ExecInsert(const CStdString sql,const CStdString dbname)
{
	CLogManager* pLog = CLogManager::Instance();
	COBSCfg* pCtiCfg = COBSCfg::Instance();
	pLog->m_rootLog.Log("execInsert \r\n %s",sql.c_str());
	int nret = -1;
	Statement* state = NULL;
	Connection* con = CreateConnection();
	try
	{
		//ConnPool* pConnPool = ConnPool::GetInstance();
		//if(pConnPool)
		//{			
			if(con)
			{
				Statement* state = con->createStatement();
				if(state)
				{
					CStdString conn;
					conn.Format("use %s",pCtiCfg->dbname.c_str());
					state->execute(conn);
					nret = state->executeUpdate(sql);
					state->close();
				}
				delete state;
				//pConnPool->ReleaseConnection(con);
				DestoryConnection(con);
			}
		//}

	}
	catch(...)
	{
		pLog->m_rootLog.Log("Excute sql %s failed",sql.c_str());
        //state->close();
		delete state;
        DestoryConnection(con);		
		return -1;
	}

	return nret;
}

ResultSet* CDBConnection::ExecUpdate(const CStdString sql)
{
	return 0;
}
