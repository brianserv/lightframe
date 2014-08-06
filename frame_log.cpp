/*
 * frame_log.cpp
 *
 *  Created on: 2013-1-2
 *      Author: jimm
 */

#include "frame_log.h"


FRAME_NAMESPACE_BEGIN

CFrameLog::CFrameLog()
{
}

CFrameLog::~CFrameLog()
{

}

//��ʼ����־����
int32_t CFrameLog::Initialize()
{
	int32_t nRet = m_stUserLogQueue.Initialize();
	if(0 > nRet)
	{
		return nRet;
	}

	return S_OK;
}

//�ָ���־����
int32_t CFrameLog::Resume()
{
	int32_t nRet = m_stUserLogQueue.Resume();
	if(0 > nRet)
	{
		return nRet;
	}

	return S_OK;
}

//ע����־����
int32_t CFrameLog::Uninitialize()
{
	m_stUserLogQueue.Uninitialize();

	return S_OK;
}

//�Ӷ���β��׷��һ����־
int32_t CFrameLog::Push(const char *szLog, const char *szName, const int32_t nID1, const int32_t nID2, const char* szDate)
{
	return m_stUserLogQueue.Push(szLog, szName, nID1, nID2, szDate);
}

//�Ӷ���ͷ����ȡһ����־
int32_t CFrameLog::Pop(char* szLog, uint32_t nLogSize, char *szName, int32_t& nID1, int32_t& nID2, char* szDate)
{
	return m_stUserLogQueue.Pop(szLog, nLogSize, szName, nID1, nID2, szDate);
}

//�ж϶����Ƿ�����
bool CFrameLog::IsFull()
{
	return m_stUserLogQueue.IsFull();
}

//�ж������Ƿ�Ϊ��
bool CFrameLog::IsEmpty()
{
	return m_stUserLogQueue.IsEmpty();
}

FRAME_NAMESPACE_END

