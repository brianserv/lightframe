/*
 * frame_timer_mgt.cpp
 *
 *  Created on: 2011-11-22
 *      Author: jimm
 */


#include "common/common_datetime.h"
#include "frame_errordef.h"
#include "frame_timer_mgt.h"
#include "frame_mem_mgt.h"


FRAME_NAMESPACE_BEGIN

CFrameTimerMgt::CFrameTimerMgt()
{
	m_nTimerSeq = 0;
}

CFrameTimerMgt::~CFrameTimerMgt()
{

}

//��ʼ����������
int32_t CFrameTimerMgt::Initialize()
{
	MUTEX_GUARD(TimerLock, m_stTimerLock);
	m_timerPool.Initialize();
	m_timerMap.Initialize();
	return S_OK;
}

//�ָ���������
int32_t CFrameTimerMgt::Resume()
{
	MUTEX_GUARD(TimerLock, m_stTimerLock);
	m_timerPool.Resume();
	m_timerMap.Resume();
	return S_OK;
}

//ע����ʱ��������
int32_t CFrameTimerMgt::Uninitialize()
{
	MUTEX_GUARD(TimerLock, m_stTimerLock);
	m_timerPool.Uninitialize();
	m_timerMap.Uninitialize();
	return S_OK;
}

int32_t CFrameTimerMgt::CreateTimer(int32_t nTimerID, ITimerEvent *pHandle, ITimerData *pTimerData, int64_t nCycleTime, bool bLoop, TimerIndex& timerIndex)
{
	uint8_t *pObj = NULL;
	if(pTimerData != NULL)
	{
		int32_t nTimerDataSize = pTimerData->GetSize();
		pObj = MALLOC(nTimerDataSize);//g_FrameMemMgt.AllocBlock(nTimerDataSize);
		if(pObj == NULL)
		{
			return E_LIBF_ALLOCBLOCKFAILED;
		}

		memcpy(pObj, pTimerData, nTimerDataSize);
	}

	//��ʼ����ʱ������
	FrameTimer timer;
	timer.timerType = 0;//enmFrameTimerType_Timer;
	timer.nTimerID = nTimerID;
	timer.pData = pObj;
	timer.nStartTime = CTimeValue::CurrentTime().Microseconds();
	timer.nCycleTime = nCycleTime;
	timer.nEndTime = timer.nStartTime + timer.nCycleTime;
	timer.bLoop = bLoop;
	timer.pEventHandle = pHandle;
	timer.pTimerProc = NULL;
	timer.nFiredCount = 0;

	MUTEX_GUARD(TimerLock, m_stTimerLock);
	//����Timer����ʱ��Seq
	timer.nTimerSeq = m_nTimerSeq;
	//����TimerSeq;
	++m_nTimerSeq;

	//������ʱ������
	TimerPool::CIndex* pIndex = m_timerPool.CreateObject();
	if (NULL == pIndex)
	{
		if(pObj != NULL)
		{
			FREE(pObj);
		}
		return E_LIBF_CREATETIMER;
	}

	//������ʱ������
	TimerMap::CIndex* pMapIndex = m_timerMap.Insert(timer.nEndTime, pIndex->Index());
	if (NULL == pMapIndex)
	{
		if(pObj != NULL)
		{
			FREE(pObj);
		}
		m_timerPool.DestroyObject(pIndex);
		pIndex = NULL;
		return E_LIBF_CREATETIMERINDEX;
	}

	//���������浽�������ݱ���
	int32_t ret = pIndex->SetAdditionalData(enmAdditionalIndex_RBTreeIndex, (uint64_t)pMapIndex);
	if (0 > ret)
	{
		if(pObj != NULL)
		{
			FREE(pObj);
		}
		m_timerMap.Erase(pMapIndex);
		m_timerPool.DestroyObject(pIndex);
		pIndex = NULL;
		return ret;
	}

	//���ö�ʱ������
	pIndex->ObjectPtr()->SetTimer(timer);
	timerIndex = pIndex->Index();

	return S_OK;
}

int32_t CFrameTimerMgt::CreateTimer(TimerProc Proc, ITimerEvent *pTimer, ITimerData *pTimerData, int64_t nCycleTime, bool bLoop, TimerIndex& timerIndex)
{
	uint8_t *pObj = NULL;
	if(pTimerData != NULL)
	{
		int32_t nTimerDataSize = pTimerData->GetSize();
		pObj = MALLOC(nTimerDataSize);//g_FrameMemMgt.AllocBlock(nTimerDataSize);
		if(pObj == NULL)
		{
			return E_LIBF_ALLOCBLOCKFAILED;
		}

		memcpy(pObj, pTimerData, nTimerDataSize);
	}

	//��ʼ����ʱ������
	FrameTimer timer;
	timer.timerType = 0;//enmFrameTimerType_Timer;
	timer.nTimerID = 0;
	timer.pData = pObj;
	timer.nStartTime = CTimeValue::CurrentTime().Microseconds();
	timer.nCycleTime = nCycleTime;
	timer.nEndTime = timer.nStartTime + timer.nCycleTime;
	timer.bLoop = bLoop;
	timer.pEventHandle = pTimer;
	timer.pTimerProc = Proc;
	timer.nFiredCount = 0;

	MUTEX_GUARD(TimerLock, m_stTimerLock);
	//����Timer����ʱ��Seq
	timer.nTimerSeq = m_nTimerSeq;
	//����TimerSeq;
	++m_nTimerSeq;

	//������ʱ������
	TimerPool::CIndex* pIndex = m_timerPool.CreateObject();
	if (NULL == pIndex)
	{
		if(pObj != NULL)
		{
			FREE(pObj);
		}
		return E_LIBF_CREATETIMER;
	}

	//������ʱ������
	TimerMap::CIndex* pMapIndex = m_timerMap.Insert(timer.nEndTime, pIndex->Index());
	if (NULL == pMapIndex)
	{
		if(pObj != NULL)
		{
			FREE(pObj);
		}
		m_timerPool.DestroyObject(pIndex);
		pIndex = NULL;
		return E_LIBF_CREATETIMERINDEX;
	}

	//���������浽�������ݱ���
	int32_t ret = pIndex->SetAdditionalData(enmAdditionalIndex_RBTreeIndex, (uint64_t)pMapIndex);
	if (0 > ret)
	{
		if(pObj != NULL)
		{
			FREE(pObj);
		}
		m_timerMap.Erase(pMapIndex);
		m_timerPool.DestroyObject(pIndex);
		pIndex = NULL;
		return ret;
	}

	//���ö�ʱ������
	pIndex->ObjectPtr()->SetTimer(timer);
	timerIndex = pIndex->Index();

	return S_OK;
}

//ɾ����ʱ��
int32_t CFrameTimerMgt::RemoveTimer(TimerIndex timerIndex)
{
	MUTEX_GUARD(TimerLock, m_stTimerLock);
	TimerPool::CIndex* pIndex = m_timerPool.GetIndex(timerIndex);
	if (NULL == pIndex)
	{
		return E_LIBF_TIMERNOTEXIST;
	}

	return RemoveTimer(pIndex);
}

//��ն�ʱ��������
int32_t CFrameTimerMgt::Clear()
{
	MUTEX_GUARD(TimerLock, m_stTimerLock);
	m_timerPool.Clear();
	m_timerMap.Clear();
	return S_OK;
}

//��ʱ���Ѵ���
int32_t CFrameTimerMgt::TimerFired(TimerIndex timerIndex, uint32_t timerSeq)
{
	CFrameTimer *pTimer = NULL;
	int32_t nRet = GetTimer(timerIndex,pTimer);
	if( nRet == S_OK && pTimer->GetTimerSeq() == timerSeq)
	{
		return TimerFired(timerIndex);
	}

	return S_FALSE;
}


//��ȡ��ʱ��
int32_t CFrameTimerMgt::GetTimer(TimerIndex timerIndex, CFrameTimer*& pTimer)
{
	MUTEX_GUARD(TimerLock, m_stTimerLock);
	TimerPool::CIndex* pIndex = m_timerPool.GetIndex(timerIndex);
	if (NULL == pIndex)
	{
		return E_LIBF_TIMERNOTFOUND;
	}

	pTimer = pIndex->ObjectPtr();

	return S_OK;
}

//ɾ����ʱ��
int32_t CFrameTimerMgt::RemoveTimer(TimerPool::CIndex* pIndex)
{
	if (NULL == pIndex)
	{
		return E_LIBF_TIMERNOTEXIST;
	}

	CFrameTimer* pTimer = pIndex->ObjectPtr();
	if(pTimer == NULL)
	{
		return E_NULLPOINTER;
	}
	//���ն�ʱ����������ڴ���Դ
	FrameTimer timer;
	pTimer->GetTimer(timer);
	if(timer.pData != NULL)
	{
		FREE((uint8_t *)timer.pData);//g_FrameMemMgt.RecycleBlock((uint8_t *)timer.pData);
	}

	uint64_t nAddtionalValue = 0;
	pIndex->GetAdditionalData(enmAdditionalIndex_RBTreeIndex, nAddtionalValue);
	TimerMap::CIndex* pMapIndex = (TimerMap::CIndex*)nAddtionalValue;

	//����ʱ���Ӷ���ؼ���������ɾ��
//	MUTEX_GUARD(TimerLock, m_stTimerLock);
	m_timerMap.Erase(pMapIndex);
	m_timerPool.DestroyObject(pIndex);

	return S_OK;
}

//��ʱ���Ѵ���
int32_t CFrameTimerMgt::TimerFired(TimerIndex timerIndex)
{
	MUTEX_GUARD(TimerLock, m_stTimerLock);
	TimerPool::CIndex* pIndex = m_timerPool.GetIndex(timerIndex);
	if (NULL == pIndex)
	{
		return E_LIBF_TIMERNOTFOUND;
	}

	CFrameTimer* pTimer = pIndex->ObjectPtr();
	if(pTimer == NULL)
	{
		return E_NULLPOINTER;
	}

	if (!pTimer->IsLoop())
	{
		//������ѭ��������ɾ����ʱ��
		return RemoveTimer(pIndex);
	}

	FrameTimer timer;
	pTimer->GetTimer(timer);

	//���¶�ʱ������
	timer.nEndTime = CTimeValue::CurrentTime().Microseconds() + timer.nCycleTime;
	++timer.nFiredCount;
	pTimer->SetTimer(timer);

	//������ʱ������
	uint64_t nAddtionalValue = 0;
	pIndex->GetAdditionalData(enmAdditionalIndex_RBTreeIndex, nAddtionalValue);
	TimerMap::CIndex* pMapIndex = (TimerMap::CIndex*)nAddtionalValue;

	m_timerMap.Erase(pMapIndex);
	pMapIndex = m_timerMap.Insert(timer.nEndTime, pIndex->Index());
	if (NULL == pMapIndex)
	{
		m_timerPool.DestroyObject(pIndex);
		return E_LIBF_CREATETIMERINDEX;
	}

	//���������浽�������ݱ���
	int32_t ret = pIndex->SetAdditionalData(enmAdditionalIndex_RBTreeIndex, (uint64_t)pMapIndex);
	if (0 > ret)
	{
		m_timerMap.Erase(pMapIndex);
		m_timerPool.DestroyObject(pIndex);
		return ret;
	}

	return S_OK;
}

//��ȡ����ʱ����С�Ķ�ʱ��
int32_t CFrameTimerMgt::GetFirstTimer(CFrameTimer*& pTimer, TimerIndex& timerIndex)
{
	MUTEX_GUARD(TimerLock, m_stTimerLock);
	TimerMap::CIndex* pMapIndex = m_timerMap.First();
	if (NULL == pMapIndex)
	{
		return E_LIBF_NOTIMER;
	}

	TimerPool::CIndex* pIndex = m_timerPool.GetIndex( pMapIndex->Object() );
	if (NULL == pIndex)
	{
		//�����б��д�����Ч������ɾ��
		m_timerMap.Erase(pMapIndex);
		return E_LIBF_INVALIDTIMERINDEX;
	}

	pTimer = pIndex->ObjectPtr();
	timerIndex = pIndex->Index();

	return S_OK;
}


FRAME_NAMESPACE_END
