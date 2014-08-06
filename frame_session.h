/*
 * frame_session.h
 *
 *  Created on: 2011-8-23
 *      Author: jimm
 */

#ifndef FRAME_SESSION_H_
#define FRAME_SESSION_H_

#include <memory.h>
#include "common/common_object.h"
#include "common/common_complextype.h"
#include "common/common_def.h"
#include "frame_namespace.h"
#include "frame_typedef.h"
#include "lightframe_impl.h"


FRAME_NAMESPACE_BEGIN


#ifndef MAX_SESSION_SIZE
#define MAX_SESSION_SIZE			1024 * 8
#endif

class CFrameSession : public CObject
{
public:
	CFrameSession()
	{
		Reset();
	}

	virtual ~CFrameSession()
	{
	}

	//初始化会话对象
	virtual int32_t Initialize()
	{
		Reset();
		return S_OK;
	}

	//恢复会话对象
	virtual int32_t Resume()
	{
		return S_OK;
	}

	//注销会话对象
	virtual int32_t Uninitialize()
	{
		m_sessionType = enmSessionType_None;
		//memset(m_arrData, 0, sizeof(m_arrData));
		m_pSessionData = 0;
		return S_OK;
	}

	//设置会话ID
	void SetSessionIndex(SessionIndex sessionIndex)
	{
		m_nSessionIndex = sessionIndex;
	}

	//获取会话ID
	SessionIndex GetSessionIndex()
	{
		return m_nSessionIndex;
	}

	//设置会话序号
	void SetSessionSeq(int32_t nSeq)
	{
		m_nSessionSeq = nSeq;
	}

	//获取会话序号
	uint32_t GetSessionSeq()
	{
		return m_nSessionSeq;
	}

	//设置会话事件接口
	void SetSessionEventHandler(ISessionEvent *pEventHandler)
	{
		m_pSessionEvent = pEventHandler;
	}

	void SetSessionCallBackProc(SessionEventCallBack SessionEvent, SessionTimeoutCallBack SessionTimeout)
	{
		m_pSessionCallBack = SessionEvent;
		m_pTimeoutCallBack = SessionTimeout;
	}

	void GetSessionCallBackProc(SessionEventCallBack &SessionEvent, SessionTimeoutCallBack &SessionTimeout)
	{
		SessionEvent = m_pSessionCallBack;
		SessionTimeout = m_pTimeoutCallBack;
	}

	//获取会话事件接口
	ISessionEvent* GetSessionEventHandler()
	{
		return m_pSessionEvent;
	}

	//设置会话等待的MsgID
	void SetSessionWaitMsg(uint32_t nMsgID)
	{
		m_nWaitMsgID = nMsgID;
	}

	//获取会话等待的MsgID
	int32_t GetSessionWaitMsg()
	{
		return m_nWaitMsgID;
	}

	//设置会话超时时间
	void SetTimeoutValue(int64_t timeout)
	{
		m_nTimeoutVal = timeout;
	}

	//获取会话超时时间
	int64_t GetTimeoutVal()
	{
		return m_nTimeoutVal;
	}

	//设置会话绑定的TimerID
	void SetTimerIndex(TimerIndex timerindex)
	{
		m_nTimerIndex = timerindex;
	}

	//获取会话绑定的TimerID
	TimerIndex GetTimerIndex()
	{
		return m_nTimerIndex;
	}

public:
	void Reset()
	{
		m_sessionType = 0;
		m_pSessionData = NULL;

		m_nSessionSeq = 0;
		m_nSessionIndex = enmInvalidSessionIndex;
		m_nTimerIndex = enmInvalidTimerIndex;

		m_nTimeoutVal = 0;
		m_pSessionEvent = NULL;
		m_nWaitMsgID = 0;

		m_pSessionCallBack = NULL;
		m_pTimeoutCallBack = NULL;
	}
	//设置会话类型
	void SetSessionType(SessionType sessionType)
	{
		m_sessionType = sessionType;
	}

	//获取会话类型
	SessionType GetSessionType()
	{
		return m_sessionType;
	}

	//获取会话数据指针
	uint8_t* GetSessionData()
	{
		//return m_arrData;
		return m_pSessionData;
	}

	//获取会话数据大小
	int32_t GetSessionDataSize()
	{
		uint16_t nBlockSize = 0;
		//return MAX_SESSION_SIZE;
		if(m_pSessionData != NULL)
		{
			nBlockSize = *(uint16_t *)(m_pSessionData - sizeof(uint16_t));
		}

		return nBlockSize;
	}

	void SetSessionMem(uint8_t *pMem)
	{
		m_pSessionData = pMem;
	}

protected:
	SessionType			m_sessionType;			//会话类型
//	uint8_t				m_arrData[MAX_SESSION_SIZE];	//会话数据
	uint8_t				*m_pSessionData;				//会话数据

	uint32_t			m_nSessionSeq;		//会话生成时的序号
	SessionIndex		m_nSessionIndex;		//会话ID
	TimerIndex			m_nTimerIndex;		//会话绑定的TimerID

	int64_t				m_nTimeoutVal;		//会话超时时间
	ISessionEvent*		m_pSessionEvent;		//会话回调接口(对象)
	int32_t				m_nWaitMsgID;		//会话绑定的消息

	SessionEventCallBack m_pSessionCallBack; //会话回调接口(过程)
	SessionTimeoutCallBack m_pTimeoutCallBack; //会话超时接口(过程)
};


FRAME_NAMESPACE_END


CCOMPLEXTYPE_SPECIFICALLY(FRAME_NAMESPACE::CFrameSession)


#endif /* FRAME_SESSION_H_ */
