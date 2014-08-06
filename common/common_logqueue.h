/*
 * common_logqueue.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_LOGQUEUE_H_
#define COMMON_LOGQUEUE_H_

#include <stdio.h>
#include <memory.h>
#include <string.h>

#include "common_typedef.h"
#include "common_def.h"
#include "common_object.h"
#include "common_datetime.h"
#include "common_codequeue.h"



template<uint32_t CAPACITY>
class CLogQueue : public CObject
{
public:
	CLogQueue()
	{
	}
	virtual ~CLogQueue()
	{
	}
	enum
	{
		enmLogParameterLength = 17,//17 = sizeof(uint8_t) + 2*sizeof(uint32_t) + strlen("20090406")
		enmLogNameLength = 30,
		enmLogOrherParameterLength = 46,//46 = enmLogNameLength + 2*sizeof(uint32_t) + strlen("20090406")
	};

public:
	//��ʼ������
	virtual int32_t Initialize()
	{
		return m_codeQueue.Initialize();
	}
	//�ָ�����
	virtual int32_t Resume()
	{
		return m_codeQueue.Resume();
	}
	//ע������
	virtual int32_t Uninitialize()
	{
		return m_codeQueue.Uninitialize();
	}

public:

	//�Ӷ���β��׷��һ����־
	int32_t Push(const char *szLogMsg, uint8_t type, int32_t id1 , int32_t id2 , const char* szDate)
	{
		if (NULL == szLogMsg)
		{
			return E_INVALIDARGUMENT;
		}
		int logLen = (int)strlen(szLogMsg);
		if (logLen > (enmMaxLogInfoLength - enmLogParameterLength))
		{
			return E_UNKNOWN;
		}


		//��������֯��һ����������
		char szLogInfo[enmMaxLogInfoLength];// = { 0 };
		strcpy(szLogInfo, szLogMsg);
		szLogInfo[logLen] = type;
		memcpy(szLogInfo + logLen + 1, &id1, sizeof(int32_t));
		memcpy(szLogInfo + logLen + 1 + sizeof(int32_t), &id2, sizeof(int32_t));
		if (NULL == szDate)
		{
			CDateTime dt = CDateTime::CurrentDateTime();
			//���㵱ǰ����
			sprintf(szLogInfo + logLen + 1 + 2*sizeof(int32_t), "%04d%02d%02d", dt.Year(), dt.Month(), dt.Day());
		}
		else
		{
			strcpy(szLogInfo + logLen + 1 + 2*sizeof(int32_t), szDate);
		}

		return m_codeQueue.Push((const uint8_t*)szLogInfo, (uint32_t)(logLen + 17));
	}

	//�Ӷ���β��׷��һ����־
	int32_t Push(const char *szLogMsg, const char *szName, int32_t id1 , int32_t id2 , const char* szDate)
	{
		if (NULL == szLogMsg)
		{
			return E_INVALIDARGUMENT;
		}
		int logLen = (int)strlen(szLogMsg);
		if (logLen > (enmMaxLogInfoLength - enmLogOrherParameterLength))
		{
			return E_UNKNOWN;
		}


		//��������֯��һ����������
		char szLogInfo[enmMaxLogInfoLength];// = { 0 };
		strcpy(szLogInfo, szLogMsg);

		int32_t offset = logLen ;
		int32_t len = strlen(szName);
		if(len > enmLogNameLength)
		{
			return E_UNKNOWN;
		}

		strncpy(szLogInfo + offset, szName, enmLogNameLength);
		offset += enmLogNameLength;

		memcpy(szLogInfo + offset, &id1, sizeof(int32_t));
		offset += sizeof(int32_t);

		memcpy(szLogInfo + offset, &id2, sizeof(int32_t));
		offset += sizeof(int32_t);

		if (NULL == szDate)
		{
			CDateTime dt = CDateTime::CurrentDateTime();
			//���㵱ǰ����
			sprintf(szLogInfo + offset, "%04d%02d%02d", dt.Year(), dt.Month(), dt.Day());
		}
		else
		{
			strcpy(szLogInfo + offset, szDate);
		}

		return m_codeQueue.Push((const uint8_t*)szLogInfo, (uint32_t)(offset + 8));
	}

	//�Ӷ���β��׷��һ����־
// 	int32_t Push(const char* szFileName, const char *szLog)
// 	{
// 		if (NULL == szFileName)
// 		{
// 			return E_INVALIDARGUMENT;
// 		}
// 		if (NULL == szLog)
// 		{
// 			return E_INVALIDARGUMENT;
// 		}
// 		//if (strlen(szFileName) + strlen(szLog) + 2 > enmMaxLogInfoLength)
// 		//{
// 		//	return E_UNKNOWN;
// 		//}
//
// 		//�������ַ���ƴ����һ��������
// 		char szLogInfo[enmMaxLogInfoLength] = { 0 };
// 		strcpy(szLogInfo, szFileName);
// 		strcpy(szLogInfo + strlen(szLogInfo) + 1, szLog);
//
// 		return m_codeQueue.Push((const uint8_t*)szLogInfo, (uint32_t)(strlen(szFileName) + strlen(szLog) + 2));
// 	}
		//�Ӷ���ͷ����ȡһ����־
	int32_t Pop(char* szLogMsg, uint32_t nLogSize, uint8_t& type, int32_t& id1, int32_t& id2, char* szDate)
	{
		if (NULL == szLogMsg)
		{
			return E_INVALIDARGUMENT;
		}
		if (NULL == szDate)
		{
			return E_INVALIDARGUMENT;
		}

		//��ȡһ����־��¼
		uint32_t nLength = 0;
		char szLogInfo[enmMaxLogInfoLength] ;//= { 0 };
		int32_t ret = m_codeQueue.Pop((uint8_t*)szLogInfo, enmMaxLogInfoLength, nLength);
		if (0 > ret)
		{
			return ret;
		}

		////�ж������������С�Ƿ��㹻
		//if ((strlen(szLogInfo) + 1 > nFileNameSize) || (nLength - strlen(szLogInfo) - 1) > nLogSize)
		//{
		//	return E_UNKNOWN;
		//}

		//���ַ����зֽ���־��Ϣ�͸�������
		uint32_t msgLen = nLength - enmLogParameterLength;
		if (msgLen > nLogSize)
		{
			return E_UNKNOWN;
		}
		memcpy(szLogMsg, szLogInfo,msgLen);
		szLogMsg[msgLen] = '\0';
		type = (uint8_t)szLogInfo[msgLen];
		id1 = (int32_t)szLogInfo[msgLen + 1];
		id2 = (int32_t)szLogInfo[msgLen + 1 + 4];
		memcpy(szDate,szLogInfo + msgLen + 9,enmLogParameterLength - 9);
		return S_OK;
	}

	int32_t Pop(char* szLogMsg, uint32_t nLogSize, char *szName, int32_t& id1, int32_t& id2, char* szDate)
	{
		if (NULL == szLogMsg)
		{
			return E_INVALIDARGUMENT;
		}
		if (NULL == szDate)
		{
			return E_INVALIDARGUMENT;
		}

		//��ȡһ����־��¼
		uint32_t nLength = 0;
		char szLogInfo[enmMaxLogInfoLength] ;//= { 0 };
		int32_t ret = m_codeQueue.Pop((uint8_t*)szLogInfo, enmMaxLogInfoLength, nLength);
		if (0 > ret)
		{
			return ret;
		}

		////�ж������������С�Ƿ��㹻
		//if ((strlen(szLogInfo) + 1 > nFileNameSize) || (nLength - strlen(szLogInfo) - 1) > nLogSize)
		//{
		//	return E_UNKNOWN;
		//}

		//���ַ����зֽ���־��Ϣ�͸�������
		uint32_t msgLen = nLength - enmLogOrherParameterLength;
		if (msgLen > nLogSize)
		{
			return E_UNKNOWN;
		}
		memcpy(szLogMsg, szLogInfo,msgLen);
		szLogMsg[msgLen] = '\0';

		int32_t offset = msgLen;

		memcpy(szName, szLogInfo+offset, enmLogNameLength);
		szName[enmLogNameLength] = '\0';
		offset += enmLogNameLength;

		id1 = (int32_t)szLogInfo[offset];
		offset += sizeof(int32_t);

		id2 = (int32_t)szLogInfo[offset];
		offset += sizeof(int32_t);

		memcpy(szDate,szLogInfo + offset,nLength - offset);
		return S_OK;
	}

// 	int32_t Pop(char* szFileName, uint32_t nFileNameSize, char* szLog, uint32_t nLogSize)
// 	{
// 		if (NULL == szFileName)
// 		{
// 			return E_INVALIDARGUMENT;
// 		}
// 		if (NULL == szLog)
// 		{
// 			return E_INVALIDARGUMENT;
// 		}
//
// 		//��ȡһ����־��¼
// 		uint32_t nLength = 0;
// 		char szLogInfo[enmMaxLogInfoLength] = { 0 };
// 		int32_t ret = m_codeQueue.Pop((uint8_t*)szLogInfo, enmMaxLogInfoLength, nLength);
// 		if (0 > ret)
// 		{
// 			return ret;
// 		}
//
// 		////�ж������������С�Ƿ��㹻
// 		//if ((strlen(szLogInfo) + 1 > nFileNameSize) || (nLength - strlen(szLogInfo) - 1) > nLogSize)
// 		//{
// 		//	return E_UNKNOWN;
// 		//}
//
// 		//���ַ����зֽ���ļ�������־��Ϣ
// 		strcpy(szFileName, szLogInfo);
// 		strcpy(szLog, szLogInfo + strlen(szFileName) + 1);
//
// 		return S_OK;
// 	}

	//�ж϶����Ƿ�����
	bool IsFull()
	{
		return m_codeQueue.IsFull();
	}
	//�ж������Ƿ�Ϊ��
	bool IsEmpty()
	{
		return m_codeQueue.IsEmpty();
	}

protected:
	CCodeQueue<CAPACITY>	m_codeQueue;		//����������

};

#endif /* COMMON_LOGQUEUE_H_ */
