/*
 * frame_commandqueue.cpp
 *
 *  Created on: 2012-11-6
 *      Author: jimm
 */

#include "frame_errordef.h"
#include "frame_commandqueue.h"


FRAME_NAMESPACE_BEGIN

CFrameCommandQueue::CFrameCommandQueue()
{
}

CFrameCommandQueue::~CFrameCommandQueue()
{

}

size_t CFrameCommandQueue::GetSize()
{
	return m_stCommandQueue.size();
}

const char* CFrameCommandQueue::GetName()
{
	return COMMANDQUEUE_DATACENTER;
}

//初始化CS队列
int32_t CFrameCommandQueue::Initialize()
{
	return S_OK;
}

//恢复CS队列
int32_t CFrameCommandQueue::Resume()
{
	return S_OK;
}

//注销CS队列
int32_t CFrameCommandQueue::Uninitialize()
{
	return S_OK;
}

//从接收队列尾部追加一条消息
int32_t CFrameCommandQueue::Push(NetPacket *pNetPacket)
{
	m_stCommandQueue.push_back(pNetPacket);

	return S_OK;
}

//从接收队列头部读取一条消息
int32_t CFrameCommandQueue::Pop(NetPacket *&pNetPacket)
{
	pNetPacket = NULL;

	if(m_stCommandQueue.size() > 0)
	{
		pNetPacket = m_stCommandQueue.front();
		m_stCommandQueue.pop_front();
	}

	return S_OK;
}

//判断接收队列是否已满
bool CFrameCommandQueue::IsFull()
{
	return false;
}

//判读接收队列是否为空
bool CFrameCommandQueue::IsEmpty()
{
	return m_stCommandQueue.empty();
}

FRAME_NAMESPACE_END

