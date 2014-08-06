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
	//��ʼ��
	int32_t Initialize(const char* szHost, const char* szUser, const char* szPassword, const char* szDBName,int32_t port);
	//��������
	int32_t Connect();
	//��������
	int32_t Connect(const char* szHost, const char* szUser, const char* szPassword, const char* szDBName);
	//�Ͽ�����
	int32_t Disconnect();

	//��ʼ����־ǰ׺
	int32_t InitializeLogPrefix(const char* szLogPrefix);
	//������־�ȼ�
	void SetLogLevel(LogLevel logLevel);

public:
	//ִ�в������
	int32_t ExecuteInsert(const char* szSql, uint64_t& nAffectedRows);
	//ִ�в������
	int32_t ExecuteInsert(const char* szSql, uint64_t& nAffectedRows, uint64_t& nNewID);
	//ִ�и��²���
	int32_t ExecuteUpdate(const char* szSql, uint64_t& nAffectedRows);
	//ִ��ɾ������
	int32_t ExecuteDelete(const char* szSql, uint64_t& nAffectedRows);
	//ִ�в�ѯ����
	int32_t ExecuteQuery(const char* szSql, char arrRecordSet[][enmMaxFieldValueLength], uint32_t nRecordSetSize, uint32_t& nFieldCount, uint32_t& nRowCount);
	//ִ�в�ѯ����(��֤û�л������������)
	int32_t ExecuteQuerySafe(const char* szSql, char arrRecordSet[][enmMaxFieldValueLength], uint32_t nRecordSetSize, uint32_t& nFieldCount, uint32_t& nRowCount);

	//��ѯ��������
	int32_t ExecuteQueryRow(const char* szSql, uint32_t& nRowCount);

	//ִ�п�ʼ��ѯ����
	int32_t ExecuteQueryBegin(const char* szSql, uint32_t& nFieldCount, uint32_t& nRowCount);
	//��ȡ��һ��
	int32_t FetchRow(char arrRecordSet[][enmMaxFieldValueLength], uint32_t nRecordSetSize);
	//�����Ƿ��ȡ���
	bool IsFetchEnd();
	//ִ�н�����ѯ����
	int32_t ExecuteQueryEnd();

	MYSQL* GetMySQL()
	{
		return &m_connection;
	}

	int32_t GetLastMysqlError();

	bool LastMysqlErrorIsServerError();

	//����ǿͻ��˴�����ôsql��Ҫд���ļ�
	bool LastMysqlErrorIsClinetError();


	//дsql��־
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
