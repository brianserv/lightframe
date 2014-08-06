/*
 * common_messagequeue.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_MESSAGEQUEUE_H_
#define COMMON_MESSAGEQUEUE_H_

#include <stdio.h>
#include <memory.h>

#include "common_typedef.h"
#include "common_object.h"
#include "common_codequeue.h"
#include "common_message.h"


template<uint32_t CAPACITY>
class CMessageQueue : public CObject
{
public:
	CMessageQueue()
	{
	}
	virtual ~CMessageQueue()
	{
	}

public:
	//��ʼ������
	virtual int32_t Initialize()
	{
		return m_codeQueue.Initialize();
	}
	//�ָ�����
	virtual int32_t Resume()
	{
		return m_codeQueue.Resume();
	}
	//ע������
	virtual int32_t Uninitialize()
	{
		return m_codeQueue.Uninitialize();
	}

public:
	//�Ӷ���β��׷��һ�����ݰ�
	int32_t Push(const MessageContent *pMessageContent)
	{
		if (NULL == pMessageContent)
		{
			return E_INVALIDARGUMENT;
		}

		return m_codeQueue.Push(pMessageContent->m_arrMessageBuffer, pMessageContent->m_nMessageSize);
	}
	//�Ӷ���ͷ����ȡһ�����ݰ�
	int32_t Pop(MessageContent *pMessageContent)
	{
		if (NULL == pMessageContent)
		{
			return E_INVALIDARGUMENT;
		}

		return m_codeQueue.Pop(pMessageContent->m_arrMessageBuffer, enmMaxMessageSize, pMessageContent->m_nMessageSize);
	}

	//�ж϶����Ƿ�����
	bool IsFull()
	{
		return m_codeQueue.IsFull();
	}
	//�ж������Ƿ�Ϊ��
	bool IsEmpty()
	{
		return m_codeQueue.IsEmpty();
	}

protected:
	CCodeQueue<CAPACITY>	m_codeQueue;		//����������

};

#endif /* COMMON_MESSAGEQUEUE_H_ */
