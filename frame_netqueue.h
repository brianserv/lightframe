/*
 * frame_netqueue.h
 *
 *  Created on: 2012-11-1
 *      Author: jimm
 */

#ifndef FRAME_NETQUEUE_H_
#define FRAME_NETQUEUE_H_

#include "common/common_object.h"
#include "common/common_messagequeue.h"
#include "common/common_memory.h"
#include "common/common_singleton.h"
#include "common/common_mutex.h"
#include "frame_namespace.h"
#include "frame_def.h"
#include "frame_protocolhead.h"

#include <list>

using namespace std;

FRAME_NAMESPACE_BEGIN

//typedef CCodeQueue<enmMaxLogicNetQueueSize>		LogicSSCodeQueue;

//typedef CCodeQueue<enmMaxLogicNetQueueSize>		LogicCSCodeQueue;

typedef list<NetPacket *>	 NetQueue;

class CFrameNetQueue
{
public:
	CFrameNetQueue();
	virtual ~CFrameNetQueue();

	virtual size_t GetSize();
	virtual const char* GetName();

public:
	//��ʼ��CS����
	virtual int32_t Initialize();
	//�ָ�CS����
	virtual int32_t Resume();
	//ע��CS����
	virtual int32_t Uninitialize();

public:
	//�ӽ��ն���β��׷��һ����Ϣ
	int32_t PushRecvSSQueue(NetPacket *pNetPacket);
	//�ӽ��ն���ͷ����ȡһ����Ϣ
	int32_t PopRecvSSQueue(NetPacket *&pNetPacket);
	//�жϽ��ն����Ƿ�����
	bool IsRecvSSQueueFull();
	//�ж����ն����Ƿ�Ϊ��
	bool IsRecvSSQueueEmpty();
	//�ӷ��Ͷ���β��׷��һ����Ϣ
	int32_t PushSendSSQueue(NetPacket *pNetPacket);
	//�ӷ��Ͷ���ͷ����ȡһ����Ϣ
	int32_t PopSendSSQueue(NetPacket *&pNetPacket);
	//�жϷ��Ͷ����Ƿ�����
	bool IsSendSSQueueFull();
	//�ж����Ͷ����Ƿ�Ϊ��
	bool IsSendSSQueueEmpty();

	//�ӽ��ն���β��׷��һ����Ϣ
	int32_t PushRecvCSQueue(NetPacket *pNetPacket);
	//�ӽ��ն���ͷ����ȡһ����Ϣ
	int32_t PopRecvCSQueue(NetPacket *&pNetPacket);
	//�жϽ��ն����Ƿ�����
	bool IsRecvCSQueueFull();
	//�ж����ն����Ƿ�Ϊ��
	bool IsRecvCSQueueEmpty();
	//�ӷ��Ͷ���β��׷��һ����Ϣ
	int32_t PushSendCSQueue(NetPacket *pNetPacket);
	//�ӷ��Ͷ���ͷ����ȡһ����Ϣ
	int32_t PopSendCSQueue(NetPacket *&pNetPacket);
	//�жϷ��Ͷ����Ƿ�����
	bool IsSendCSQueueFull();
	//�ж����Ͷ����Ƿ�Ϊ��
	bool IsSendCSQueueEmpty();

protected:
	NetQueue				m_stRecvSSQueue;
	CriticalSection			m_stRecvSSLock;
	NetQueue				m_stSendSSQueue;
	CriticalSection			m_stSendSSLock;

	NetQueue				m_stRecvCSQueue;
	CriticalSection			m_stRecvCSLock;
	NetQueue				m_stSendCSQueue;
	CriticalSection			m_stSendCSLock;
};


#define	CREATE_FRAMENETQUEUE_INSTANCE	CSingleton<CFrameNetQueue>::CreateInstance
#define	g_FrameNetQueue					CSingleton<CFrameNetQueue>::GetInstance()
#define	DESTROY_FRAMENETQUEUE_INSTANCE	CSingleton<CFrameNetQueue>::DestroyInstance

FRAME_NAMESPACE_END

#endif /* FRAME_NETQUEUE_H_ */
