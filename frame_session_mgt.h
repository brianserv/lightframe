/*
 * frame_session_mgt.h
 *
 *  Created on: 2011-11-22
 *      Author: jimm
 */

#ifndef FRAME_SESSION_MGT_H_
#define FRAME_SESSION_MGT_H_

#include "common/common_object.h"
#include "common/common_pool.h"
#include "common/common_hashlist.h"
#include "common/common_singleton.h"
#include "common/common_mutex.h"
#include "frame_namespace.h"
#include "frame_def.h"
#include "frame_session.h"
#include "frame_errordef.h"
#include "frame_msgevent_mgt.h"
#include "frame_timer_mgt.h"
#include "frame_mem_mgt.h"


FRAME_NAMESPACE_BEGIN

//����Ự����غ���������
typedef CPool<CFrameSession, MAX_FRAMESESSION_COUNT>	SessionPool;


class CFrameSessionMgt : public CObject, public ITimerEvent
{
public:

public:
	CFrameSessionMgt()
	{
		m_nSessionSeq = 0;
	}
	virtual ~CFrameSessionMgt()
	{

	}

public:
	//��ʼ���Ự������
	virtual int32_t Initialize()
	{
		MUTEX_GUARD(Lock, m_stSessionLock);
		m_sessionPool.Initialize();
		return S_OK;
	}
	//�ָ��Ự������
	virtual int32_t Resume()
	{
		MUTEX_GUARD(Lock, m_stSessionLock);
		m_sessionPool.Resume();
		return S_OK;
	}
	//ע���Ự������
	virtual int32_t Uninitialize()
	{
		MUTEX_GUARD(Lock, m_stSessionLock);
		m_sessionPool.Uninitialize();
		return S_OK;
	}

public:
	//�����Ự(�����Ͻӿ�)
	int32_t CreateSession(const uint32_t nMsgID, const int32_t nSessionType, ISessionEvent* pHandler, int64_t nTimeOut, CFrameSession*& pSession);

	//�����Ự
	int32_t CreateSession(const uint32_t nMsgID, int64_t nTimeOut, ISessionData *pSessionData, CFrameSession*& pSession);

	//ɾ���Ự
	int32_t RemoveSession(const SessionIndex sessionindex);

	//�Ự����Ϣ
	int32_t SessionBindMsg(const SessionIndex sessionindex, const uint32_t MsgID);

	//�Ự�Ƿ��а��������Ϣ
	bool IsSessionBindingMsg(const SessionIndex sessionindex, const uint32_t MsgID);

	//�Ự������
	int32_t OnFrameSessionEvent(SessionIndex sessionindex, MessageHeadSS* msgHead, const uint8_t* buf, const uint32_t size);

	//�Ự��ʱ
	int32_t OnTimerEvent(CFrameTimer *pFrameTimer);

	//��ȡ���������
	uint32_t GetCapacity() const
	{
		return m_sessionPool.Capacity();
	}
	//��ȡ������ж�������
	uint32_t GetSessionCount() const
	{
		//MUTEX_GUARD(Lock, m_stSessionLock);
		return m_sessionPool.ObjectCount();
	}
	//������Ƿ�Ϊ��
	bool IsEmpty() const
	{
		//MUTEX_GUARD(Lock, m_stSessionLock);
		return m_sessionPool.IsEmpty();
	}
	//������Ƿ�����
	bool IsFull() const
	{
		//MUTEX_GUARD(Lock, m_stSessionLock);
		return m_sessionPool.IsFull();
	}

	//��ջỰ������
	int32_t Clear()
	{
		MUTEX_GUARD(Lock, m_stSessionLock);
		m_sessionPool.Clear();
		return S_OK;
	}

//protected:
	//�����Ự����
	int32_t CreateSession(SessionPool::CIndex*& pIndex)
	{
		MUTEX_GUARD(Lock, m_stSessionLock);
		//��������
		pIndex = m_sessionPool.CreateObject();
		if (NULL == pIndex)
		{
			return E_LIBF_CREATESESSION;
		}

		return S_OK;
	}
	//�����Ự����
	int32_t CreateSession(CFrameSession*& pSession, SessionIndex& sessionIndex)
	{
		SessionPool::CIndex* pIndex = NULL;

		int32_t ret = CreateSession(pIndex);
		if (0 > ret)
		{
			return ret;
		}

		pSession = pIndex->ObjectPtr();
		sessionIndex = pIndex->Index();

		//���ûỰ�������(�˴����ڶ��̻߳������⣬��Ϊ++�����̰߳�ȫ��)
		pSession->SetSessionSeq(++m_nSessionSeq);
		return S_OK;
	}

	//��ȡ�Ự����
	int32_t GetSessionByIndex(const SessionIndex sessionIndex, SessionPool::CIndex*& pIndex)
	{
		MUTEX_GUARD(Lock, m_stSessionLock);

		pIndex = m_sessionPool.GetIndex(sessionIndex);
		if (NULL == pIndex)
		{
			return E_LIBF_SESSIONNOTEXIST;
		}

		return S_OK;
	}
	//��ȡ�Ự����
	int32_t GetSessionByIndex(const SessionIndex sessionIndex, CFrameSession*& pSession)
	{
		SessionPool::CIndex* pIndex = NULL;

		int32_t ret = GetSessionByIndex(sessionIndex, pIndex);
		if (0 > ret)
		{
			return ret;
		}

		pSession = pIndex->ObjectPtr();

		return S_OK;
	}

	//���ٻỰ����
	int32_t DestroySession(SessionPool::CIndex* pIndex)
	{
		if (NULL == pIndex)
		{
			return E_INVALIDARGUMENT;
		}

		MUTEX_GUARD(Lock, m_stSessionLock);

		//���Ự�Ӷ���ؼ���������ɾ��
		m_sessionPool.DestroyObject(pIndex);

		return S_OK;
	}

	//���ٻỰ����
	int32_t DestroySessionByIndex(const SessionIndex sessionIndex);

	//ʹ�ûص��ӿ�
	int32_t CallBackSessionEvent(const uint32_t nMsgID, CFrameSession *pSession, MessageHeadSS *pMsgHeadSS, const uint8_t* buf, const uint32_t size);

protected:
	int32_t					m_nSessionSeq;			//�Ự�������
	SessionPool				m_sessionPool;			//�Ự�����

	CriticalSection			m_stSessionLock;
};

#define	CREATE_FRAMESESSIONMGT_INSTANCE		CSingleton<CFrameSessionMgt>::CreateInstance
#define	g_FrameSessionMgt					CSingleton<CFrameSessionMgt>::GetInstance()
#define	DESTROY_FRAMESESSIONMGT_INSTANCE	CSingleton<CFrameSessionMgt>::DestroyInstance


FRAME_NAMESPACE_END
#endif /* FRAME_SESSION_MGT_H_ */
