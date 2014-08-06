/*
 * common_mysqlengine.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_MYSQLENGINE_H_
#define COMMON_MYSQLENGINE_H_


#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#endif
#include "mysql/mysql.h"

#include "common_typedef.h"
#include "common_def.h"


class CMysqlEngine
{
public:
	CMysqlEngine();
	virtual ~CMysqlEngine();

public:
	//初始化
	int32_t Initialize(const char* szHost, const char* szUser, const char* szPassword, const char* szDBName,int32_t port);
	//建立连接
	int32_t Connect();
	//建立连接
	int32_t Connect(const char* szHost, const char* szUser, const char* szPassword, const char* szDBName);
	//断开连接
	int32_t Disconnect();

	//初始化日志前缀
	int32_t InitializeLogPrefix(const char* szLogPrefix);
	//设置日志等级
	void SetLogLevel(LogLevel logLevel);

public:
	//执行插入操作
	int32_t ExecuteInsert(const char* szSql, uint64_t& nAffectedRows);
	//执行插入操作
	int32_t ExecuteInsert(const char* szSql, uint64_t& nAffectedRows, uint64_t& nNewID);
	//执行更新操作
	int32_t ExecuteUpdate(const char* szSql, uint64_t& nAffectedRows);
	//执行删除操作
	int32_t ExecuteDelete(const char* szSql, uint64_t& nAffectedRows);
	//执行查询操作
	int32_t ExecuteQuery(const char* szSql, char arrRecordSet[][enmMaxFieldValueLength], uint32_t nRecordSetSize, uint32_t& nFieldCount, uint32_t& nRowCount);
	//执行查询操作(保证没有缓冲区溢出现象)
	int32_t ExecuteQuerySafe(const char* szSql, char arrRecordSet[][enmMaxFieldValueLength], uint32_t nRecordSetSize, uint32_t& nFieldCount, uint32_t& nRowCount);

	//查询行数操作
	int32_t ExecuteQueryRow(const char* szSql, uint32_t& nRowCount);

	//执行开始查询操作
	int32_t ExecuteQueryBegin(const char* szSql, uint32_t& nFieldCount, uint32_t& nRowCount);
	//读取下一行
	int32_t FetchRow(char arrRecordSet[][enmMaxFieldValueLength], uint32_t nRecordSetSize);
	//数据是否读取完毕
	bool IsFetchEnd();
	//执行结束查询操作
	int32_t ExecuteQueryEnd();

	MYSQL* GetMySQL()
	{
		return &m_connection;
	}

	int32_t GetLastMysqlError();

	bool LastMysqlErrorIsServerError();

	//如果是客户端错误，那么sql需要写如文件
	bool LastMysqlErrorIsClinetError();


	//写sql日志
	void WriteMysqlLog(LogLevel logLevel, const char* szFormat, ...);


protected:
	MYSQL			m_connection;
	char			m_szHost[enmMaxIPAddressLength];
	char			m_szUser[enmMaxDBUserLength];
	char			m_szPassword[enmMaxDBPasswordLength];
	char			m_szDBName[enmMaxDBNameLength];
	int32_t			m_nPort;
	bool			m_bConnected;

	MYSQL_RES*		m_res;
	uint32_t		m_nFieldCount;
	uint32_t		m_nRowCount;
	uint32_t		m_nFetchedCount;

	LogLevel		m_logLevel;
	char			m_szLogPrefix[128];
};


#endif /* COMMON_MYSQLENGINE_H_ */
