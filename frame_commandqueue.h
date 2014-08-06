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
	//初始化CS队列
	virtual int32_t Initialize();
	//恢复CS队列
	virtual int32_t Resume();
	//注销CS队列
	virtual int32_t Uninitialize();

public:
	//从接收队列尾部追加一条消息
	int32_t Push(NetPacket *pNetPacket);
	//从接收队列头部读取一条消息
	int32_t Pop(NetPacket *&pNetPacket);
	//判断接收队列是否已满
	bool IsFull();
	//判读接收队列是否为空
	bool IsEmpty();

protected:
	CommandQueue			m_stCommandQueue;
};

#define	CREATE_COMMANDQUEUE_INSTANCE	CSingleton<CFrameCommandQueue>::CreateInstance
#define	g_FrameCommandQueue				CSingleton<CFrameCommandQueue>::GetInstance()
#define	DESTROY_COMMANDQUEUE_INSTANCE	CSingleton<CFrameCommandQueue>::DestroyInstance

FRAME_NAMESPACE_END

#endif /* FRAME_COMMANDQUEUE_H_ */
