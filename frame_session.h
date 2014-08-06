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

	//��ʼ���Ự����
	virtual int32_t Initialize()
	{
		Reset();
		return S_OK;
	}

	//�ָ��Ự����
	virtual int32_t Resume()
	{
		return S_OK;
	}

	//ע���Ự����
	virtual int32_t Uninitialize()
	{
		m_sessionType = enmSessionType_None;
		//memset(m_arrData, 0, sizeof(m_arrData));
		m_pSessionData = 0;
		return S_OK;
	}

	//���ûỰID
	void SetSessionIndex(SessionIndex sessionIndex)
	{
		m_nSessionIndex = sessionIndex;
	}

	//��ȡ�ỰID
	SessionIndex GetSessionIndex()
	{
		return m_nSessionIndex;
	}

	//���ûỰ���
	void SetSessionSeq(int32_t nSeq)
	{
		m_nSessionSeq = nSeq;
	}

	//��ȡ�Ự���
	uint32_t GetSessionSeq()
	{
		return m_nSessionSeq;
	}

	//���ûỰ�¼��ӿ�
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

	//��ȡ�Ự�¼��ӿ�
	ISessionEvent* GetSessionEventHandler()
	{
		return m_pSessionEvent;
	}

	//���ûỰ�ȴ���MsgID
	void SetSessionWaitMsg(uint32_t nMsgID)
	{
		m_nWaitMsgID = nMsgID;
	}

	//��ȡ�Ự�ȴ���MsgID
	int32_t GetSessionWaitMsg()
	{
		return m_nWaitMsgID;
	}

	//���ûỰ��ʱʱ��
	void SetTimeoutValue(int64_t timeout)
	{
		m_nTimeoutVal = timeout;
	}

	//��ȡ�Ự��ʱʱ��
	int64_t GetTimeoutVal()
	{
		return m_nTimeoutVal;
	}

	//���ûỰ�󶨵�TimerID
	void SetTimerIndex(TimerIndex timerindex)
	{
		m_nTimerIndex = timerindex;
	}

	//��ȡ�Ự�󶨵�TimerID
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
	//���ûỰ����
	void SetSessionType(SessionType sessionType)
	{
		m_sessionType = sessionType;
	}

	//��ȡ�Ự����
	SessionType GetSessionType()
	{
		return m_sessionType;
	}

	//��ȡ�Ự����ָ��
	uint8_t* GetSessionData()
	{
		//return m_arrData;
		return m_pSessionData;
	}

	//��ȡ�Ự���ݴ�С
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
	SessionType			m_sessionType;			//�Ự����
//	uint8_t				m_arrData[MAX_SESSION_SIZE];	//�Ự����
	uint8_t				*m_pSessionData;				//�Ự����

	uint32_t			m_nSessionSeq;		//�Ự����ʱ�����
	SessionIndex		m_nSessionIndex;		//�ỰID
	TimerIndex			m_nTimerIndex;		//�Ự�󶨵�TimerID

	int64_t				m_nTimeoutVal;		//�Ự��ʱʱ��
	ISessionEvent*		m_pSessionEvent;		//�Ự�ص��ӿ�(����)
	int32_t				m_nWaitMsgID;		//�Ự�󶨵���Ϣ

	SessionEventCallBack m_pSessionCallBack; //�Ự�ص��ӿ�(����)
	SessionTimeoutCallBack m_pTimeoutCallBack; //�Ự��ʱ�ӿ�(����)
};


FRAME_NAMESPACE_END


CCOMPLEXTYPE_SPECIFICALLY(FRAME_NAMESPACE::CFrameSession)


#endif /* FRAME_SESSION_H_ */
