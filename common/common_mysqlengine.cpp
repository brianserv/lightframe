/***************************************************************************************************
*	file name:	common_mysqlengine.cpp
*	created:	2008-12-01
*	author:		teras xiao
*	version:	1.0
*	purpose:	
*
*	revision:
*
***************************************************************************************************/


#include <memory.h>
#include <stdarg.h>
#include "common/common_mysqlengine.h"
#include "common/common_errordef.h"
#include "common/common_api.h"
#include "common/common_datetime.h"


//#ifndef WIN32

CMysqlEngine::CMysqlEngine()
{
	memset(&m_connection, 0, sizeof(m_connection));
	memset(m_szHost, 0, sizeof(m_szHost));
	memset(m_szUser, 0, sizeof(m_szUser));
	memset(m_szPassword, 0, sizeof(m_szPassword));
	memset(m_szDBName, 0, sizeof(m_szDBName));

	m_bConnected = false;

	m_res = NULL;
	m_nFieldCount = 0;
	m_nRowCount = 0;
	m_nFetchedCount = 0;
	m_nPort = 0;

	m_logLevel = enmLogLevel_Info;
	memset(m_szLogPrefix, 0, sizeof(m_szLogPrefix));
}

CMysqlEngine::~CMysqlEngine()
{

}

//初始化
int32_t CMysqlEngine::Initialize(const char* szHost, const char* szUser, const char* szPassword, const char* szDBName,int32_t port)
{
	if ((NULL == szHost) || (NULL == szUser) || (NULL == szPassword))
	{
		return E_INVALIDARGUMENT;
	}

	MYSQL *pMysql = mysql_init(&m_connection);
	if (NULL == pMysql)
	{
		return E_MYSQLINIT;
	}

	int ret = mysql_options(&m_connection, MYSQL_OPT_RECONNECT, "1");
	if (0 != ret)
	{
		return E_MYSQLOPTION;
	}

	strcpy(m_szHost, szHost);
	strcpy(m_szUser, szUser);
	strcpy(m_szPassword, szPassword);
	if (szDBName != NULL)
	{
		strcpy(m_szDBName, szDBName);
	}
	m_nPort = port;

	return S_OK;
}

//建立连接
int32_t CMysqlEngine::Connect()
{
	MYSQL *pMysql = NULL;
	if (m_szDBName[0] == '\0')
	{
		pMysql = mysql_real_connect(&m_connection, m_szHost, m_szUser, m_szPassword, "", uint32_t(m_nPort), NULL, 0);
		if (NULL == pMysql)
		{
			return E_MYSQLCONNECT;
		}
	}
	else
	{
		pMysql = mysql_real_connect(&m_connection, m_szHost, m_szUser, m_szPassword, m_szDBName, uint32_t(m_nPort), NULL, 0);
		if (NULL == pMysql)
		{
			return E_MYSQLCONNECT;
		}
	}

	if(0 != mysql_set_character_set(pMysql, "utf8"))
	{
		return E_SET_MYSQL_CHARSET;
	}

	return S_OK;
}

//建立连接
int32_t CMysqlEngine::Connect(const char* szHost, const char* szUser, const char* szPassword, const char* szDBName)
{
	if ((NULL == szHost) || (NULL == szUser) || (NULL == szPassword) || (NULL == szDBName))
	{
		return E_INVALIDARGUMENT;
	}

	strcpy(m_szHost, szHost);
	strcpy(m_szUser, szUser);
	strcpy(m_szPassword, szPassword);
	if (m_szDBName[0] != '\0')
	{
		strcpy(m_szDBName, szDBName);
		MYSQL *pMysql = mysql_real_connect(&m_connection, m_szHost, m_szUser, m_szPassword, m_szDBName, uint32_t(m_nPort), NULL, 0);
		if (NULL == pMysql)
		{
			return E_MYSQLCONNECT;
		}
	}
	else
	{
		MYSQL *pMysql = mysql_real_connect(&m_connection, m_szHost, m_szUser, m_szPassword, "", uint32_t(m_nPort), NULL, 0);
		if (NULL == pMysql)
		{
			return E_MYSQLCONNECT;
		}

	}

	return S_OK;
}

//断开连接
int32_t CMysqlEngine::Disconnect()
{
	mysql_close(&m_connection);

	return S_OK;
}

//初始化日志前缀
int32_t CMysqlEngine::InitializeLogPrefix(const char* szLogPrefix)
{
	strcpy(m_szLogPrefix, szLogPrefix);

	return S_OK;
}

//设置日志等级
void CMysqlEngine::SetLogLevel(LogLevel logLevel)
{
	m_logLevel = logLevel;
}

//执行插入操作
int32_t CMysqlEngine::ExecuteInsert(const char* szSql, uint64_t& nAffectedRows)
{
	int ret = mysql_real_query(&m_connection, szSql, (unsigned long)strlen(szSql));
	if (0 != ret)
	{
		WriteMysqlLog(enmLogLevel_Error, "Execute insert sql result:%d  sql:%s Error:%s\n", ret, szSql, mysql_error(&m_connection));
		return E_MYSQLINSERT;
	}

	nAffectedRows = mysql_affected_rows(&m_connection);

	WriteMysqlLog(enmLogLevel_Notice, "Execute insert(affected rows:%d) sql result:%d  sql:%s\n", nAffectedRows, ret, szSql);

	return S_OK;
}

//执行插入操作
int32_t CMysqlEngine::ExecuteInsert(const char* szSql, uint64_t& nAffectedRows, uint64_t& nNewID)
{
	int ret = mysql_real_query(&m_connection, szSql, (unsigned long)strlen(szSql));
	if (0 != ret)
	{
		WriteMysqlLog(enmLogLevel_Error, "Execute insert sql result:%d  sql:%s Error:%s\n", ret, szSql, mysql_error(&m_connection));
		return E_MYSQLINSERT;
	}

	nAffectedRows = mysql_affected_rows(&m_connection);
	nNewID = mysql_insert_id(&m_connection);
	
	WriteMysqlLog(enmLogLevel_Notice, "Execute insert(affected rows:%d, newid:%d) sql result:%d  sql:%s\n",nAffectedRows, nNewID, ret, szSql);

	return S_OK;
}

//执行更新操作
int32_t CMysqlEngine::ExecuteUpdate(const char* szSql, uint64_t& nAffectedRows)
{
	int ret = mysql_real_query(&m_connection, szSql, (unsigned long)strlen(szSql));
	if (0 != ret)
	{
		WriteMysqlLog(enmLogLevel_Error, "Execute update sql result:%d  sql:%s Error:%s\n", ret, szSql, mysql_error(&m_connection));
		return E_MYSQLUPDATE;
	}

	nAffectedRows = mysql_affected_rows(&m_connection);
	WriteMysqlLog(enmLogLevel_Notice, "Execute update(affected rows:%d) sql result:%d  sql:%s\n",nAffectedRows, ret, szSql);

	return S_OK;
}

//执行删除操作
int32_t CMysqlEngine::ExecuteDelete(const char* szSql, uint64_t& nAffectedRows)
{
	int ret = mysql_real_query(&m_connection, szSql, (unsigned long)strlen(szSql));
	if (0 != ret)
	{
		WriteMysqlLog(enmLogLevel_Error, "Execute delete sql result:%d  sql:%s Error:%s\n", ret, szSql, mysql_error(&m_connection));
		return E_MYSQLDELETE;
	}

	nAffectedRows = mysql_affected_rows(&m_connection);

	WriteMysqlLog(enmLogLevel_Notice, "Execute delete(affected rows:%d) sql result:%d  sql:%s\n",nAffectedRows, ret, szSql);

	return S_OK;
}

//执行查询操作
int32_t CMysqlEngine::ExecuteQuery(const char* szSql, char arrRecordSet[][enmMaxFieldValueLength], uint32_t nRecordSetSize, uint32_t& nFieldCount, uint32_t& nRowCount)
{
	int ret = mysql_real_query(&m_connection, szSql, (unsigned long)strlen(szSql));
	if (0 != ret)
	{
		WriteMysqlLog(enmLogLevel_Error, "Execute query sql result:%d  sql:%s Error:%s\n", ret, szSql, mysql_error(&m_connection));
		return E_MYSQLQUERY;
	}
	WriteMysqlLog(enmLogLevel_Debug, "Execute query sql result:%d  sql:%s\n", ret, szSql);

	MYSQL_RES *res = NULL;
	MYSQL_ROW row;

	res = mysql_store_result(&m_connection);
	if (NULL == res)
	{
		return E_MYSQLSTORERESULT;
	}

	nFieldCount = (uint32_t)mysql_num_fields(res);
	nRowCount = (uint32_t)mysql_num_rows(res);
	for (uint32_t i = 0; i < nRowCount; ++i)
	{
		row = mysql_fetch_row(res);
		for (uint32_t j = 0; j < nFieldCount; ++j)
		{
			if (NULL != row[j])
			{
				strcpy(arrRecordSet[nFieldCount * i + j], row[j]);
			}
		}
	}
	mysql_free_result(res);

	return S_OK;
}

//执行查询操作
int32_t CMysqlEngine::ExecuteQuerySafe(const char* szSql, char arrRecordSet[][enmMaxFieldValueLength], uint32_t nRecordSetSize, uint32_t& nFieldCount, uint32_t& nRowCount)
{
	int ret = mysql_real_query(&m_connection, szSql, (unsigned long)strlen(szSql));
	if (0 != ret)
	{
		WriteMysqlLog(enmLogLevel_Error, "Execute query sql result:%d  sql:%s Error:%s\n", ret, szSql, mysql_error(&m_connection));
		return E_MYSQLQUERY;
	}
	WriteMysqlLog(enmLogLevel_Debug, "Execute query sql result:%d  sql:%s\n", ret, szSql);

	MYSQL_RES *res = NULL;
	MYSQL_ROW row;

	res = mysql_store_result(&m_connection);
	if (NULL == res)
	{
		return E_MYSQLSTORERESULT;
	}

	nFieldCount = (uint32_t)mysql_num_fields(res);
	nRowCount = (uint32_t)mysql_num_rows(res);
	for (uint32_t i = 0; i < nRowCount; ++i)
	{
		row = mysql_fetch_row(res);
		for (uint32_t j = 0; j < nFieldCount; ++j)
		{
			if (NULL != row[j])
			{
				if (strlen(row[j]) >= enmMaxFieldValueLength)
				{
					//在return之前一定要释放资源
					mysql_free_result(res);
					return E_SQL_QUERYFLOW;
				}

				strcpy(arrRecordSet[nFieldCount * i + j], row[j]);
			}
		}
	}
	mysql_free_result(res);

	return S_OK;
}

//执行查询行数操作
int32_t CMysqlEngine::ExecuteQueryRow(const char* szSql, uint32_t& nRowCount)
{
	int ret = mysql_real_query(&m_connection, szSql, (unsigned long)strlen(szSql));
	if (0 != ret)
	{
		WriteMysqlLog(enmLogLevel_Error, "Execute query sql result:%d  sql:%s Error:%s\n", ret, szSql, mysql_error(&m_connection));
		return E_MYSQLQUERY;
	}
	WriteMysqlLog(enmLogLevel_Debug, "Execute query sql result:%d  sql:%s\n", ret, szSql);

	MYSQL_RES *res = NULL;

	res = mysql_store_result(&m_connection);
	if (NULL == res)
	{
		return E_MYSQLSTORERESULT;
	}

	nRowCount = (uint32_t)mysql_num_rows(res);

	mysql_free_result(res);

	return S_OK;
}

//执行开始查询操作
int32_t CMysqlEngine::ExecuteQueryBegin(const char* szSql, uint32_t& nFieldCount, uint32_t& nRowCount)
{
	int ret = mysql_real_query(&m_connection, szSql, (unsigned long)strlen(szSql));
	if (0 != ret)
	{
		return E_MYSQLQUERY;
	}
	WriteMysqlLog(enmLogLevel_Debug, "Execute query sql result:%d  sql:%s\n", ret, szSql);


	m_res = mysql_store_result(&m_connection);
	if (NULL == m_res)
	{
		return E_MYSQLSTORERESULT;
	}

	m_nFieldCount = (uint32_t)mysql_num_fields(m_res);
	m_nRowCount = (uint32_t)mysql_num_rows(m_res);
	m_nFetchedCount = 0;

	nFieldCount = m_nFieldCount;
	nRowCount = m_nRowCount;

	return S_OK;
}

//读取下一行
int32_t CMysqlEngine::FetchRow(char arrRecordSet[][enmMaxFieldValueLength], uint32_t nRecordSetSize)
{
	MYSQL_ROW row = mysql_fetch_row(m_res);
	if (NULL == row)
	{
		return E_NULLPOINTER;
	}
	for (uint32_t i = 0; i < m_nFieldCount; ++i)
	{
		if (NULL != row[i])
		{
			strcpy(arrRecordSet[i], row[i]);
		}
	}
	++m_nFetchedCount;

	return S_OK;
}

//数据是否读取完毕
bool CMysqlEngine::IsFetchEnd()
{
	return (m_nFetchedCount >= m_nRowCount);
}

//执行结束查询操作
int32_t CMysqlEngine::ExecuteQueryEnd()
{
	mysql_free_result(m_res);

	return S_OK;
}

int32_t CMysqlEngine::GetLastMysqlError()
{
	return mysql_errno(&m_connection);
}

bool CMysqlEngine::LastMysqlErrorIsServerError()
{
	uint32_t err = mysql_errno(&m_connection);
	if (err >= 1000 && err < 2000)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CMysqlEngine::LastMysqlErrorIsClinetError()
{
	return !LastMysqlErrorIsServerError();
}

//写sql日志
void CMysqlEngine::WriteMysqlLog(LogLevel logLevel, const char* szFormat, ...)
{
	if (m_szLogPrefix[0] == '\0')
	{
		return;
	}

	if (logLevel < m_logLevel)
	{
		return;
	}

	//生成文件名
	char szFileName[enmMaxFileNameLength] = { 0 };

#ifndef WIN32
	CDateTime dt = CDateTime::CurrentDateTime();
	sprintf(szFileName, "%s_%04d%02d%02d.sql", m_szLogPrefix, dt.Year(), dt.Month(), dt.Day());
#else
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	sprintf(szFileName, "%s_%04d%02d%02d.sql", m_szLogPrefix, st.wYear, st.wMonth, st.wDay);
#endif

	FILE *pf = fopen(szFileName, "a+");
	if (NULL == pf)
	{
		return;
	}

#ifndef WIN32
	fprintf(pf, "%04d-%02d-%02d %02d:%02d:%02d ", dt.Year(), dt.Month(), dt.Day(), dt.Hour(), dt.Minute(), dt.Second());
#else
	fprintf(pf, "%04d-%02d-%02d %02d:%02d:%02d ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
#endif

	va_list ap;
	va_start(ap, szFormat);
	vfprintf(pf, szFormat, ap);
	va_end(ap);

	fclose(pf);
}


//#endif
