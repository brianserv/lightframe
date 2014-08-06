/*
 * frame_timer_mgt.h
 *
 *  Created on: 2011-11-22
 *      Author: jimm
 */

#ifndef FRAME_TIMER_MGT_H_
#define FRAME_TIMER_MGT_H_

#include "common/common_object.h"
#include "common/common_pool.h"
#include "common/common_rbtree.h"
#include "common/common_singleton.h"
#include "common/common_mutex.h"
#include "frame_namespace.h"
#include "frame_def.h"
#include "frame_timer.h"


FRAME_NAMESPACE_BEGIN

//���嶨ʱ������غ���������
typedef CPool<CFrameTimer, MAX_FRAMETIMER_COUNT>	TimerPool;
typedef CRBTree<int64_t, TimerIndex, MAX_FRAMETIMER_COUNT>	TimerMap;

class CFrameTimerMgt : public CObject
{
protected:
	enum
	{
		enmAdditionalIndex_RBTreeIndex			= 0,
	};
public:
	CFrameTimerMgt();
	virtual ~CFrameTimerMgt();

public:
	//��ʼ����ʱ��������
	virtual int32_t Initialize();
	//�ָ���ʱ��������
	virtual int32_t Resume();
	//ע����ʱ��������
	virtual int32_t Uninitialize();

public:

	int32_t CreateTimer(int32_t nTimerID, ITimerEvent *pHandle, ITimerData *pTimerData, int64_t nCycleTime, bool bLoop, TimerIndex& timerIndex);

	//ʹ�÷���ΪCreateTimer(MEMBER_FUNC(timer_class, timer_func), pTimer, pTimerData, nCycleTime, bLoop, timerIndex);
	int32_t CreateTimer(TimerProc Proc, ITimerEvent *pTimer, ITimerData *pTimerData, int64_t nCycleTime, bool bLoop, TimerIndex& timerIndex);

	//ɾ����ʱ��
	int32_t RemoveTimer(TimerIndex timerIndex);
	//��ն�ʱ��������
	int32_t Clear();
	//��ʱ���Ѵ���
	int32_t TimerFired(TimerIndex timerIndex, uint32_t timerSeq);
	//��ȡ����ʱ����С�Ķ�ʱ��
	int32_t GetFirstTimer(CFrameTimer*& pTimer, TimerIndex& timerIndex);
	//��ȡ��ʱ��
	int32_t GetTimer(TimerIndex timerIndex, CFrameTimer*& pTimer);
	//ɾ����ʱ��
	int32_t RemoveTimer(TimerPool::CIndex* pIndex);
	//��ʱ���Ѵ���
	int32_t TimerFired(TimerIndex timerIndex);
	//��ȡ����ʱ����С�Ķ�ʱ��
	int32_t GetFirstTimer(TimerPool::CIndex*& pIndex);

protected:
	uint32_t				m_nTimerSeq;			//Tiemr�������
	CriticalSection			m_stTimerLock;
//	CriticalSection			m_stTimerPoolLock;
	TimerPool				m_timerPool;			//��ʱ�������
//	CriticalSection			m_stTimerMapLock;
	TimerMap				m_timerMap;				//��ʱ������

};

#define	CREATE_FRAMETIMERMGT_INSTANCE	CSingleton<CFrameTimerMgt>::CreateInstance
#define	g_FrameTimerMgt					CSingleton<CFrameTimerMgt>::GetInstance()
#define	DESTROY_FRAMETIMERMGT_INSTANCE	CSingleton<CFrameTimerMgt>::DestroyInstance


FRAME_NAMESPACE_END


#endif /* FRAME_TIMER_MGT_H_ */
