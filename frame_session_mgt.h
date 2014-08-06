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

//定义会话对象池和索引类型
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
	//初始化会话管理器
	virtual int32_t Initialize()
	{
		MUTEX_GUARD(Lock, m_stSessionLock);
		m_sessionPool.Initialize();
		return S_OK;
	}
	//恢复会话管理器
	virtual int32_t Resume()
	{
		MUTEX_GUARD(Lock, m_stSessionLock);
		m_sessionPool.Resume();
		return S_OK;
	}
	//注销会话管理器
	virtual int32_t Uninitialize()
	{
		MUTEX_GUARD(Lock, m_stSessionLock);
		m_sessionPool.Uninitialize();
		return S_OK;
	}

public:
	//创建会话(兼容老接口)
	int32_t CreateSession(const uint32_t nMsgID, const int32_t nSessionType, ISessionEvent* pHandler, int64_t nTimeOut, CFrameSession*& pSession);

	//创建会话
	int32_t CreateSession(const uint32_t nMsgID, int64_t nTimeOut, ISessionData *pSessionData, CFrameSession*& pSession);

	//删除会话
	int32_t RemoveSession(const SessionIndex sessionindex);

	//会话绑定消息
	int32_t SessionBindMsg(const SessionIndex sessionindex, const uint32_t MsgID);

	//会话是否有绑定了这个消息
	bool IsSessionBindingMsg(const SessionIndex sessionindex, const uint32_t MsgID);

	//会话被激活
	int32_t OnFrameSessionEvent(SessionIndex sessionindex, MessageHeadSS* msgHead, const uint8_t* buf, const uint32_t size);

	//会话超时
	int32_t OnTimerEvent(CFrameTimer *pFrameTimer);

	//获取对象池容量
	uint32_t GetCapacity() const
	{
		return m_sessionPool.Capacity();
	}
	//获取对象池中对象数量
	uint32_t GetSessionCount() const
	{
		//MUTEX_GUARD(Lock, m_stSessionLock);
		return m_sessionPool.ObjectCount();
	}
	//对象池是否为空
	bool IsEmpty() const
	{
		//MUTEX_GUARD(Lock, m_stSessionLock);
		return m_sessionPool.IsEmpty();
	}
	//对象池是否已满
	bool IsFull() const
	{
		//MUTEX_GUARD(Lock, m_stSessionLock);
		return m_sessionPool.IsFull();
	}

	//清空会话管理器
	int32_t Clear()
	{
		MUTEX_GUARD(Lock, m_stSessionLock);
		m_sessionPool.Clear();
		return S_OK;
	}

//protected:
	//创建会话对象
	int32_t CreateSession(SessionPool::CIndex*& pIndex)
	{
		MUTEX_GUARD(Lock, m_stSessionLock);
		//创建对象
		pIndex = m_sessionPool.CreateObject();
		if (NULL == pIndex)
		{
			return E_LIBF_CREATESESSION;
		}

		return S_OK;
	}
	//创建会话对象
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

		//设置会话生成序号(此处对于多线程会有问题，因为++不是线程安全的)
		pSession->SetSessionSeq(++m_nSessionSeq);
		return S_OK;
	}

	//获取会话对象
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
	//获取会话对象
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

	//销毁会话对象
	int32_t DestroySession(SessionPool::CIndex* pIndex)
	{
		if (NULL == pIndex)
		{
			return E_INVALIDARGUMENT;
		}

		MUTEX_GUARD(Lock, m_stSessionLock);

		//将会话从对象池及索引表中删除
		m_sessionPool.DestroyObject(pIndex);

		return S_OK;
	}

	//销毁会话对象
	int32_t DestroySessionByIndex(const SessionIndex sessionIndex);

	//使用回调接口
	int32_t CallBackSessionEvent(const uint32_t nMsgID, CFrameSession *pSession, MessageHeadSS *pMsgHeadSS, const uint8_t* buf, const uint32_t size);

protected:
	int32_t					m_nSessionSeq;			//会话生成序号
	SessionPool				m_sessionPool;			//会话对象池

	CriticalSection			m_stSessionLock;
};

#define	CREATE_FRAMESESSIONMGT_INSTANCE		CSingleton<CFrameSessionMgt>::CreateInstance
#define	g_FrameSessionMgt					CSingleton<CFrameSessionMgt>::GetInstance()
#define	DESTROY_FRAMESESSIONMGT_INSTANCE	CSingleton<CFrameSessionMgt>::DestroyInstance


FRAME_NAMESPACE_END
#endif /* FRAME_SESSION_MGT_H_ */
