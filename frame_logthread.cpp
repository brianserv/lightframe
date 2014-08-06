/*
 * frame_logthread.cpp
 *
 *  Created on: 2011-11-22
 *      Author: jimm
 */


#include <memory.h>
#include "frame_logthread.h"
#include "common/common_api.h"
#include "common/common_datetime.h"
#include "frame_logengine.h"
#include "frame_configmgt.h"

FRAME_NAMESPACE_BEGIN


const char* g_arrPrefix[] = {
	"_netio",
	"_main",
	"_dbaccess",
};


CFrameLogThread::CFrameLogThread()
{
	memset(m_arrLogFileState, 0, sizeof(m_arrLogFileState));
}

CFrameLogThread::~CFrameLogThread()
{

}

//��ʼ����־�߳�
int32_t CFrameLogThread::Initialize()
{
	return S_OK;
}

//�ָ���־�߳�
int32_t CFrameLogThread::Resume()
{
	return S_OK;
}

//ע����־�߳�
int32_t CFrameLogThread::Uninitialize()
{
	return S_OK;
}

//�߳���ں���
void CFrameLogThread::Execute()
{
	int32_t ret = S_OK;

	LogSource logSource = 0;
	int32_t id1 = 0;
	int32_t id2 = 0;
	char szDate[enmMaxDateLength] = { 0 };
	char szLog[enmMaxLogInfoLength];
	szLog[0] = '\0';
	bool bIdle = true;
	char szName[32] = { 0 };
	szName[0] = '_';

	while ((!GetTerminated()) || (!bIdle))
	{
		bIdle = true;

		//�Ӷ�����ȡ����־��Ϣ
		ret = g_FrameLogEngine.ReadLog(enmThreadType_NetioThread, szLog, enmMaxLogInfoLength, logSource, id1, id2, szDate);
		if (0 <= ret)
		{
			WriteToLogFile(logSource, id1, id2, szDate, szLog);
			bIdle = false;
		}
		ret = g_FrameLogEngine.ReadLog(enmThreadType_MainThread, szLog, enmMaxLogInfoLength, logSource, id1, id2, szDate);
		if (0 <= ret)
		{
			WriteToLogFile(logSource, id1, id2, szDate, szLog);
			bIdle = false;
		}
		ret = g_FrameLogEngine.ReadLog(enmThreadType_DBAccessThread, szLog, enmMaxLogInfoLength, logSource, id1, id2, szDate);
		if (0 <= ret)
		{
			WriteToLogFile(logSource, id1, id2, szDate, szLog);
			bIdle = false;
		}

		char szPrefixName[32][enmMaxFileNameLength];
		int32_t nLogObjCount = 0;
		g_FrameLogEngine.GetAllUserLogObj(szPrefixName, nLogObjCount);
		for(int32_t i = 0; i < nLogObjCount; ++i)
		{
			ret = g_FrameLogEngine.ReadUserLog(szPrefixName[i], szLog, enmMaxLogInfoLength, id1, id2, szDate);
			if(0 <= ret)
			{
				WriteToLogFile(szPrefixName[i], id1, id2, szDate, szLog);
				bIdle = false;
			}
		}

		//���ж����ж�û����־
		if (bIdle)
		{
			int64_t now_time = CTimeValue::CurrentTime().Milliseconds();
			for (uint32_t i = 0; i < enmMaxLogFileNameCount; ++i)
			{
				if (NULL == m_arrLogFileState[i].pFile)
				{
					continue;
				}

				//����ˢ��
				if (now_time > m_arrLogFileState[i].nLastFlushTime + enmFlushPeriod)
				{
					fflush(m_arrLogFileState[i].pFile);
					m_arrLogFileState[i].nLastFlushTime = now_time;
					MoveLogFile(g_FrameConfigMgt.GetFrameBaseConfig().GetLogFileSize(), m_arrLogFileState[i].szFileName, m_arrLogFileState[i].pFile);
				}

				//��־�ļ��Ƿ���Ҫ�ر�
				if (now_time > m_arrLogFileState[i].nActiveTime + enmWriteLogPeriod)
				{
					fclose(m_arrLogFileState[i].pFile);
					m_arrLogFileState[i].pFile = NULL;
					MoveLogFile(g_FrameConfigMgt.GetFrameBaseConfig().GetLogFileSize(), m_arrLogFileState[i].szFileName, m_arrLogFileState[i].pFile);
					memset(&m_arrLogFileState[i], 0, sizeof(LogFileState));
				}
			}
			Delay(enmThreadExecutePeriod);
		}
	}

	//�ر�������־�ļ�
	for (uint32_t i = 0; i < enmMaxLogFileNameCount; ++i)
	{
		if (NULL == m_arrLogFileState[i].pFile)
		{
			continue;
		}
		fclose(m_arrLogFileState[i].pFile);
	}
}

//����־д�뵽�ļ�
void CFrameLogThread::WriteToLogFile(LogSource logSource, int32_t id1, int32_t id2, const char* szDate, const char* szLog)
{
	char szFileName[enmMaxFullPathLength] = { 0 };
	sprintf(szFileName, "./log/%s%s_%d_%d_%s.log", g_FrameConfigMgt.GetFrameBaseConfig().GetServerName() ,g_arrPrefix[logSource], id1, id2, szDate);

	//����־�ļ�״̬�б��в����ļ�
	bool bWritten = false;
	uint32_t nIndex = enmInvalidLogFileIndex;
	int64_t now_time = CTimeValue::CurrentTime().Milliseconds();
	for (uint32_t i = 0; i < enmMaxLogFileNameCount; ++i)
	{
		if (NULL == m_arrLogFileState[i].pFile)
		{
			//�ҵ���һ����λ���
			if (enmInvalidLogFileIndex == nIndex)
			{
				nIndex = i;
			}
			continue;
		}
		//��־�ļ��Ƿ��Ѿ���
		if (0 == strcmp(szFileName, m_arrLogFileState[i].szFileName))
		{
			fwrite(szLog, 1, strlen(szLog), m_arrLogFileState[i].pFile);
			bWritten = true;
			//д���ļ����޸Ļ�Ծʱ��
			m_arrLogFileState[i].nActiveTime = now_time;
		}
		else
		{
			//��־�ļ��Ƿ���Ҫ�ر�
			if (now_time > m_arrLogFileState[i].nActiveTime + enmWriteLogPeriod)
			{
				fclose(m_arrLogFileState[i].pFile);
				m_arrLogFileState[i].pFile = NULL;
				MoveLogFile(g_FrameConfigMgt.GetFrameBaseConfig().GetLogFileSize(), m_arrLogFileState[i].szFileName, m_arrLogFileState[i].pFile);
				memset(&m_arrLogFileState[i], 0, sizeof(LogFileState));
			}
		}


	}
	//��־�ļ�δ�������־�ļ�
	if ((!bWritten) && (enmInvalidLogFileIndex != nIndex))
	{
		m_arrLogFileState[nIndex].pFile = fopen(szFileName, "a+");
		if (NULL != m_arrLogFileState[nIndex].pFile)
		{
			m_arrLogFileState[nIndex].nActiveTime = now_time;
			m_arrLogFileState[nIndex].nLastFlushTime = now_time;
			uint32_t len = (uint32_t)strlen(szFileName);
			if(enmMaxFullPathLength < len )
			{
				len = enmMaxFullPathLength;
			}
			strncpy(m_arrLogFileState[nIndex].szFileName, szFileName, len);
			fwrite(szLog, 1, strlen(szLog), m_arrLogFileState[nIndex].pFile);
		}
	}

}

//����־д�뵽�ļ�
void CFrameLogThread::WriteToLogFile(const char *szName, int32_t id1, int32_t id2, const char* szDate, const char* szLog)
{
	char strName[32] = { 0 };
	if(szName == NULL)
	{
		strcpy(strName, "default");
	}
	else
	{
		strncpy(strName, szName, 31);
	}

	char szFileName[enmMaxFullPathLength] = { 0 };
	sprintf(szFileName, "./log/%s_%s_%d_%d_%s.log", g_FrameConfigMgt.GetFrameBaseConfig().GetServerName() ,strName, id1, id2, szDate);

	//����־�ļ�״̬�б��в����ļ�
	bool bWritten = false;
	uint32_t nIndex = enmInvalidLogFileIndex;
	int64_t now_time = CTimeValue::CurrentTime().Milliseconds();
	for (uint32_t i = 0; i < enmMaxLogFileNameCount; ++i)
	{
		if (NULL == m_arrLogFileState[i].pFile)
		{
			//�ҵ���һ����λ���
			if (enmInvalidLogFileIndex == nIndex)
			{
				nIndex = i;
			}
			continue;
		}
		//��־�ļ��Ƿ��Ѿ���
		if (0 == strcmp(szFileName, m_arrLogFileState[i].szFileName))
		{
			fwrite(szLog, 1, strlen(szLog), m_arrLogFileState[i].pFile);
			bWritten = true;
			//д���ļ����޸Ļ�Ծʱ��
			m_arrLogFileState[i].nActiveTime = now_time;
		}
		else
		{
			//��־�ļ��Ƿ���Ҫ�ر�
			if (now_time > m_arrLogFileState[i].nActiveTime + enmWriteLogPeriod)
			{
				fclose(m_arrLogFileState[i].pFile);
				m_arrLogFileState[i].pFile = NULL;
				MoveLogFile(g_FrameConfigMgt.GetFrameBaseConfig().GetLogFileSize(), m_arrLogFileState[i].szFileName, m_arrLogFileState[i].pFile);
				memset(&m_arrLogFileState[i], 0, sizeof(LogFileState));
			}
		}


	}
	//��־�ļ�δ�������־�ļ�
	if ((!bWritten) && (enmInvalidLogFileIndex != nIndex))
	{
		m_arrLogFileState[nIndex].pFile = fopen(szFileName, "a+");
		if (NULL != m_arrLogFileState[nIndex].pFile)
		{
			m_arrLogFileState[nIndex].nActiveTime = now_time;
			m_arrLogFileState[nIndex].nLastFlushTime = now_time;
			uint32_t len = (uint32_t)strlen(szFileName);
			if(enmMaxFullPathLength < len )
			{
				len = enmMaxFullPathLength;
			}
			strncpy(m_arrLogFileState[nIndex].szFileName, szFileName, len);
			fwrite(szLog, 1, strlen(szLog), m_arrLogFileState[nIndex].pFile);
		}
	}

}


FRAME_NAMESPACE_END
