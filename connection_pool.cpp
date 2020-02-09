/*
 * connection_pool.cpp
 *
 *  Created on: 2013-3-29
 *      Author: csc
 */

#include <stdexcept>
#include <exception>
#include <stdio.h>
#include "connection_pool.h"
//#include "ctiCfg.h"
#include "obsCfg.h"
#include "logManager.h"
using namespace std;
using namespace sql;

ConnPool *ConnPool::connPool = NULL;

//连接池的构造函数
ConnPool::ConnPool(string url, string userName, string password, int maxSize) {
	CLogManager* pLog = CLogManager::Instance();
	this->maxSize = 2;
	this->curSize = 0;
	this->username = userName;
	this->password = password;
	this->url = url;
	try {
		this->driver = sql::mysql::get_driver_instance();
	} catch (sql::SQLException&e) {
		perror("驱动连接出错;\n");
		pLog->m_rootLog.Log("驱动连接出错!");

	} catch (std::runtime_error&e) {
		perror("运行出错了\n");
		pLog->m_rootLog.Log("运行出错了!");
	}

	pLog->m_rootLog.Log("ConnPool Struct");
	this->InitConnection(maxSize / 2);
}

//获取连接池对象，单例模式
ConnPool*ConnPool::GetInstance() {
	CLogManager* pLog = CLogManager::Instance();
	if (connPool == NULL) {
		
		pLog->m_rootLog.Log("ConnPool GetInstance");
		COBSCfg* pCfg = COBSCfg::Instance();

		CStdString server;
		server.Format("tcp://%s",pCfg->dbip);
		CStdString user = pCfg->dbuser;
		CStdString pwd = pCfg->dbpwd;

		connPool = new ConnPool(server.c_str(), user.c_str(), pwd.c_str(), 101);
	}
	return connPool;
}

//初始化连接池，创建最大连接数的一半连接数量
void ConnPool::InitConnection(int iInitialSize) {
	CLogManager* pLog = CLogManager::Instance();
	Connection* conn;
	pLog->m_rootLog.Log("InitConnection ");
	//pthread_mutex_lock(&lock);
	m_mutex.acquire();
	for (int i = 0; i < iInitialSize; i++) 
	{
		CStdString sLog;
		sLog.Format("InitConnection num %d");
		pLog->m_rootLog.Log("");
		conn = this->CreateConnection();
		if (conn) {
			connList.push_back(conn);
			++(this->curSize);
		} else {
			perror("创建CONNECTION出错");
			pLog->m_rootLog.Log("创建 Connection 出错!!!");
		}
	}
	m_mutex.release();
	//pthread_mutex_unlock(&lock);
}

//创建连接,返回一个Connection
Connection* ConnPool::CreateConnection() 
{
	CLogManager* pLog = CLogManager::Instance();
	Connection*conn;
	try {
		conn = driver->connect(this->url, this->username, this->password); //建立连接
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

//在连接池中获得一个连接
Connection*ConnPool::GetConnection() {
	Connection*con;

	//pthread_mutex_lock(&lock);
	m_mutex.acquire();
	if (connList.size() > 0) {	//连接池容器中还有连接
		con = connList.front(); //得到第一个连接
		connList.pop_front(); 	//移除第一个连接
		if (con->isClosed()) {	//如果连接已经被关闭，删除后重新建立一个
			delete con;
			con = this->CreateConnection();
		}
		//如果连接为空，则创建连接出错
		if (con == NULL) {
			--curSize;
		}
		m_mutex.release();
		//pthread_mutex_unlock(&lock);
		return con;
	} else {
		if (curSize < maxSize) { //还可以创建新的连接
			con = this->CreateConnection();
			if (con) {
				++curSize;
				m_mutex.release();
				//pthread_mutex_unlock(&lock);
				return con;
			} else {
				m_mutex.release();
				//pthread_mutex_unlock(&lock);
				return NULL;
			}
		} else { //建立的连接数已经达到maxSize
			//pthread_mutex_unlock(&lock);
			m_mutex.release();
			return NULL;
		}
	}
}

//回收数据库连接
void ConnPool::ReleaseConnection(sql::Connection * conn) {
	if (conn) {
		//pthread_mutex_lock(&lock);
		m_mutex.acquire();
		connList.push_back(conn);
		//pthread_mutex_unlock(&lock);
		m_mutex.release();
	}
}

//连接池的析构函数
ConnPool::~ConnPool() {
	this->DestoryConnPool();
}

//销毁连接池,首先要先销毁连接池的中连接
void ConnPool::DestoryConnPool() {
	list<Connection*>::iterator icon;
	//pthread_mutex_lock(&lock);
	m_mutex.acquire();
	for (icon = connList.begin(); icon != connList.end(); ++icon) {
		this->DestoryConnection(*icon); //销毁连接池中的连接
	}
	curSize = 0;
	connList.clear(); //清空连接池中的连接
	m_mutex.release();
	//pthread_mutex_unlock(&lock);
}

//销毁一个连接
void ConnPool::DestoryConnection(Connection* conn) {
	if (conn) {
		try {
			conn->close();
		} catch (sql::SQLException&e) {
			perror(e.what());
		} catch (std::exception&e) {
			perror(e.what());
		}
		delete conn;
	}
}

