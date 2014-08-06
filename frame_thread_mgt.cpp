/*
 * frame_thread_mgt.cpp
 *
 *  Created on: 2013-2-27
 *      Author: jimm
 */

#include "common/common_api.h"
#include "frame_thread_mgt.h"

FRAME_NAMESPACE_BEGIN

CFrameThreadMgt::CFrameThreadMgt()
{
	m_nThreadCount = 0;
	memset(m_arrThreadID, 0, sizeof(m_arrThreadID));
	memset(m_arrThread, 0, sizeof(m_arrThread));
}

CFrameThreadMgt::~CFrameThreadMgt()
{

}

void CFrameThreadMgt::RegistThread(int32_t nThreadID, CThread *pThread)
{
	if(nThreadID <= 0 || pThread == NULL)
	{
		return;
	}

//	MUTEX_GUARD(Lock, m_stThreadMgtLock);

	for(int32_t i = 0; i < m_nThreadCount; ++i)
	{
		if(m_arrThreadID[i] == nThreadID)
		{
			m_arrThread[i] = pThread;
			return;
		}
	}

	m_arrThreadID[m_nThreadCount] = nThreadID;
	m_arrThread[m_nThreadCount] = pThread;
	++m_nThreadCount;
}

CThread *CFrameThreadMgt::GetCurThread()
{
	return GetThread((int32_t)gettid());
}

CThread *CFrameThreadMgt::GetThread(int32_t nThreadID)
{
	if(nThreadID <= 0)
	{
		return NULL;
	}

	for(int32_t i = 0; i < m_nThreadCount; ++i)
	{
		if(m_arrThreadID[i] == nThreadID)
		{
			return m_arrThread[i];
		}
	}

	return NULL;
}

FRAME_NAMESPACE_END


