/*
 * frame_netqueue.cpp
 *
 *  Created on: 2012-11-1
 *      Author: jimm
 */

#include "frame_errordef.h"
#include "frame_baseconfig.h"
#include "frame_netqueue.h"


FRAME_NAMESPACE_BEGIN

CFrameNetQueue::CFrameNetQueue()
{
}

CFrameNetQueue::~CFrameNetQueue()
{

}

size_t CFrameNetQueue::GetSize()
{
	return m_stRecvSSQueue.size() + m_stSendSSQueue.size() +
			m_stRecvCSQueue.size() + m_stSendCSQueue.size();
}

const char* CFrameNetQueue::GetName()
{
	return LOGICQUEUE_DATACENTER;
}

//��ʼ��CS����
int32_t CFrameNetQueue::Initialize()
{
	return S_OK;
}

//�ָ�CS����
int32_t CFrameNetQueue::Resume()
{
	return S_OK;
}

//ע��CS����
int32_t CFrameNetQueue::Uninitialize()
{
	return S_OK;
}

//�ӽ��ն���β��׷��һ����Ϣ
int32_t CFrameNetQueue::PushRecvSSQueue(NetPacket *pNetPacket)
{
	if(pNetPacket == NULL)
	{
		return E_NULLPOINTER;
	}

	MUTEX_GUARD(RecvSSLock, m_stRecvSSLock);
	m_stRecvSSQueue.push_back(pNetPacket);

	return S_OK;
}

//�ӽ��ն���ͷ����ȡһ����Ϣ
int32_t CFrameNetQueue::PopRecvSSQueue(NetPacket *&pNetPacket)
{
	MUTEX_GUARD(RecvSSLock, m_stRecvSSLock);
	if(m_stRecvSSQueue.size() > 0)
	{
		pNetPacket = m_stRecvSSQueue.front();
		m_stRecvSSQueue.pop_front();
	}
	else
	{
		pNetPacket = NULL;
	}

	return S_OK;
}

//�жϽ��ն����Ƿ�����
bool CFrameNetQueue::IsRecvSSQueueFull()
{
	//MUTEX_GUARD(SSLock, m_stSSLock);
	return false;
}

//�ж����ն����Ƿ�Ϊ��
bool CFrameNetQueue::IsRecvSSQueueEmpty()
{
	MUTEX_GUARD(RecvSSLock, m_stRecvSSLock);
	return m_stRecvSSQueue.empty();
}

//�ӷ��Ͷ���β��׷��һ����Ϣ
int32_t CFrameNetQueue::PushSendSSQueue(NetPacket *pNetPacket)
{
	if(pNetPacket == NULL)
	{
		return E_NULLPOINTER;
	}

	MUTEX_GUARD(SendSSLock, m_stSendSSLock);
	m_stSendSSQueue.push_back(pNetPacket);

	return S_OK;
}

//�ӷ��Ͷ���ͷ����ȡһ����Ϣ
int32_t CFrameNetQueue::PopSendSSQueue(NetPacket *&pNetPacket)
{
	MUTEX_GUARD(SendSSLock, m_stSendSSLock);
	if(m_stSendSSQueue.size() > 0)
	{
		pNetPacket = m_stSendSSQueue.front();
		m_stSendSSQueue.pop_front();
	}
	else
	{
		pNetPacket = NULL;
	}

	return S_OK;
}

//�жϷ��Ͷ����Ƿ�����
bool CFrameNetQueue::IsSendSSQueueFull()
{
	return false;
}

//�ж����Ͷ����Ƿ�Ϊ��
bool CFrameNetQueue::IsSendSSQueueEmpty()
{
	MUTEX_GUARD(SendSSLock, m_stSendSSLock);
	return m_stSendSSQueue.empty();
}

//�ӽ��ն���β��׷��һ����Ϣ
int32_t CFrameNetQueue::PushRecvCSQueue(NetPacket *pNetPacket)
{
	if(pNetPacket == NULL)
	{
		return E_NULLPOINTER;
	}

	MUTEX_GUARD(RecvCSLock, m_stRecvCSLock);
	m_stRecvCSQueue.push_back(pNetPacket);

	return S_OK;
}

//�ӽ��ն���ͷ����ȡһ����Ϣ
int32_t CFrameNetQueue::PopRecvCSQueue(NetPacket *&pNetPacket)
{
	MUTEX_GUARD(RecvCSLock, m_stRecvCSLock);
	if(m_stRecvCSQueue.size() > 0)
	{
		pNetPacket = m_stRecvCSQueue.front();
		m_stRecvCSQueue.pop_front();
	}
	else
	{
		pNetPacket = NULL;
	}

	return S_OK;
}

//�жϽ��ն����Ƿ�����
bool CFrameNetQueue::IsRecvCSQueueFull()
{
	return false;
}

//�ж����ն����Ƿ�Ϊ��
bool CFrameNetQueue::IsRecvCSQueueEmpty()
{
	MUTEX_GUARD(RecvCSLock, m_stRecvCSLock);
	return m_stRecvCSQueue.empty();
}

//�ӷ��Ͷ���β��׷��һ����Ϣ
int32_t CFrameNetQueue::PushSendCSQueue(NetPacket *pNetPacket)
{
	if(pNetPacket == NULL)
	{
		return E_NULLPOINTER;
	}

	MUTEX_GUARD(SendCSLock, m_stSendCSLock);
	m_stSendCSQueue.push_back(pNetPacket);

	return S_OK;
}

//�ӷ��Ͷ���ͷ����ȡһ����Ϣ
int32_t CFrameNetQueue::PopSendCSQueue(NetPacket *&pNetPacket)
{
	MUTEX_GUARD(SendCSLock, m_stSendCSLock);
	if(m_stSendCSQueue.size() > 0)
	{
		pNetPacket = m_stSendCSQueue.front();
		m_stSendCSQueue.pop_front();
	}
	else
	{
		pNetPacket = NULL;
	}

	return S_OK;
}

//�жϷ��Ͷ����Ƿ�����
bool CFrameNetQueue::IsSendCSQueueFull()
{
	return false;
}

//�ж����Ͷ����Ƿ�Ϊ��
bool CFrameNetQueue::IsSendCSQueueEmpty()
{
	MUTEX_GUARD(SendCSLock, m_stSendCSLock);
	return m_stSendCSQueue.empty();
}


FRAME_NAMESPACE_END

