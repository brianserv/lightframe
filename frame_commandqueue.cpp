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

//��ʼ��CS����
int32_t CFrameCommandQueue::Initialize()
{
	return S_OK;
}

//�ָ�CS����
int32_t CFrameCommandQueue::Resume()
{
	return S_OK;
}

//ע��CS����
int32_t CFrameCommandQueue::Uninitialize()
{
	return S_OK;
}

//�ӽ��ն���β��׷��һ����Ϣ
int32_t CFrameCommandQueue::Push(NetPacket *pNetPacket)
{
	m_stCommandQueue.push_back(pNetPacket);

	return S_OK;
}

//�ӽ��ն���ͷ����ȡһ����Ϣ
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

//�жϽ��ն����Ƿ�����
bool CFrameCommandQueue::IsFull()
{
	return false;
}

//�ж����ն����Ƿ�Ϊ��
bool CFrameCommandQueue::IsEmpty()
{
	return m_stCommandQueue.empty();
}

FRAME_NAMESPACE_END

