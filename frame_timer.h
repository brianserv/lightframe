/*
 * frame_timer.h
 *
 *  Created on: 2011-11-22
 *      Author: jimm
 */

#ifndef FRAME_TIMER_H_
#define FRAME_TIMER_H_

#include "common/common_object.h"
#include "common/common_complextype.h"
#include "frame_namespace.h"
#include "frame_typedef.h"
#include "frame_errordef.h"
#include "lightframe_impl.h"

FRAME_NAMESPACE_BEGIN

typedef struct tagFrameTimer : public Timer
{
	uint32_t					nTimerSeq;
	ITimerEvent*				pEventHandle;			//Timer�ӿ�ָ��
	TimerProc					pTimerProc;		//Timer������
} FrameTimer;


class CFrameTimer : public CObject
{
public:
	CFrameTimer()
	{
	}
	virtual ~CFrameTimer()
	{
	}

public:
	//��ʼ����ʱ������
	virtual int32_t Initialize()
	{
		memset(&m_stTimer, 0, sizeof(m_stTimer));
		return S_OK;
	}

	//�ָ���ʱ������
	virtual int32_t Resume()
	{
		return S_OK;
	}

	//ע����ʱ������
	virtual int32_t Uninitialize()
	{
		memset(&m_stTimer, 0, sizeof(m_stTimer));
		return S_OK;
	}


public:
	//���ö�ʱ����Ϣ
	void SetTimer(const FrameTimer& stTimer)
	{
		memcpy(&m_stTimer, &stTimer, sizeof(m_stTimer));
	}

	//��ȡ��ʱ����Ϣ
	void GetTimer(FrameTimer& stTimer)
	{
		memcpy(&stTimer, &m_stTimer, sizeof(m_stTimer));
	}

	//��ȡ��ʱ������
	TimerType GetTimerType()
	{
		return m_stTimer.timerType;
	}

	//��ȡ��ʱ����ʶ
	int32_t GetTimerID()
	{
		return m_stTimer.nTimerID;
	}

	//��ȡ��ʱ������
	void *GetTimerData()
	{
		return m_stTimer.pData;
	}

	//��ȡ��ʱ������ʱ��
	int64_t GetEndTime()
	{
		return m_stTimer.nEndTime;
	}

	uint32_t GetTimerSeq()
	{
		return m_stTimer.nTimerSeq;
	}

	//��ʱ���Ƿ�ѭ������
	bool IsLoop()
	{
		return m_stTimer.bLoop;
	}

	//��ȡ�Ѵ�������
	uint32_t GetFiredCount()
	{
		return m_stTimer.nFiredCount;
	}

	//��ȡ�����ص�
	TimerProc GetEventProc()
	{
		return m_stTimer.pTimerProc;
	}

	ITimerEvent* GetEventHandler()
	{
		return m_stTimer.pEventHandle;
	}

protected:
	FrameTimer					m_stTimer;
};


FRAME_NAMESPACE_END


CCOMPLEXTYPE_SPECIFICALLY(FRAME_NAMESPACE::CFrameTimer)

#endif /* FRAME_TIMER_H_ */
