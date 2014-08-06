/*
 * frame_commandqueue.h
 *
 *  Created on: 2012-11-6
 *      Author: jimm
 */

#ifndef FRAME_COMMANDQUEUE_H_
#define FRAME_COMMANDQUEUE_H_

#include "common/common_object.h"
#include "common/common_messagequeue.h"
#include "common/common_memory.h"
#include "common/common_singleton.h"
#include "frame_namespace.h"
#include "frame_def.h"
#include "lightframe_impl.h"
#include "frame_protocolhead.h"

#include <list>

using namespace std;

FRAME_NAMESPACE_BEGIN

//typedef CCodeQueue<enmMaxCmdQueueSize>		CommandCodeQueue;

typedef list<NetPacket *>		CommandQueue;

class CFrameCommandQueue
{
public:
	CFrameCommandQueue();
	virtual ~CFrameCommandQueue();

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
	int32_t Push(NetPacket *pNetPacket);
	//�ӽ��ն���ͷ����ȡһ����Ϣ
	int32_t Pop(NetPacket *&pNetPacket);
	//�жϽ��ն����Ƿ�����
	bool IsFull();
	//�ж����ն����Ƿ�Ϊ��
	bool IsEmpty();

protected:
	CommandQueue			m_stCommandQueue;
};

#define	CREATE_COMMANDQUEUE_INSTANCE	CSingleton<CFrameCommandQueue>::CreateInstance
#define	g_FrameCommandQueue				CSingleton<CFrameCommandQueue>::GetInstance()
#define	DESTROY_COMMANDQUEUE_INSTANCE	CSingleton<CFrameCommandQueue>::DestroyInstance

FRAME_NAMESPACE_END

#endif /* FRAME_COMMANDQUEUE_H_ */
