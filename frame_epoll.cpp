/*
 * frame_epoll.cpp
 *
 *  Created on: 2012-12-3
 *      Author: jimm
 */

#include "frame_epoll.h"
#include "frame_socket.h"
#include "frame_logengine.h"

FRAME_NAMESPACE_BEGIN

CEpoll::CEpoll()
{
	m_nEpollEventTimeout = 50;
}

CEpoll::~CEpoll()
{

}

int32_t CEpoll::Create(int32_t nMaxConn)
{
	m_nMaxFD = nMaxConn;
	m_nEpollFD = epoll_create(nMaxConn);
	if(m_nEpollFD < 0)
	{
		return -1;
	}

	m_pEpollEvent = new epoll_event[nMaxConn];
	if(m_pEpollEvent == NULL)
	{
		return -1;
	}

	memset(m_pEpollEvent, 0, sizeof(struct epoll_event) * nMaxConn);

	return 0;
}

int32_t CEpoll::GetEpollFD()
{
	return m_nEpollFD;
}

//int32_t CEpoll::Control(int32_t nSocketFD, void *pObject, uint32_t nOperation, uint32_t nEvents)
//{
//	struct epoll_event event;
////	event.data.fd = nSocketFD;
//	event.data.ptr = pObject;
//	event.events = nEvents;
//	if (epoll_ctl(m_nEpollFD, nOperation, nSocketFD, &event) < 0)
//	{
//		return -1;
//	}
//
//	return 0;
//}

#include <stdio.h>
int32_t CEpoll::RegistEvent(CSocket *pSocket, uint32_t nEvents)
{
	uint32_t nIOEvents = pSocket->GetIOEvents();
	if(nIOEvents == 0)
	{
		nIOEvents |= (EPOLLHUP | EPOLLERR);
	}

	nIOEvents |= nEvents;

	struct epoll_event event;
	event.data.ptr = (void *)pSocket;
	event.events = nIOEvents;
	//�������ǵ�һ�����
	if(pSocket->GetIOEvents() > 0)
	{
		if (epoll_ctl(m_nEpollFD, EPOLL_CTL_MOD, pSocket->GetSocketFD(), &event) < 0)
		{
			printf("1111nsocketfd=%d, errno=%d, desc=%s\n", pSocket->GetSocketFD(), errno, strerror(errno));
			return -1;
		}
	}
	else
	{
		if (epoll_ctl(m_nEpollFD, EPOLL_CTL_ADD, pSocket->GetSocketFD(), &event) < 0)
		{
			printf("2222nsocketfd=%d, errno=%d, desc=%s\n", pSocket->GetSocketFD(), errno, strerror(errno));
			return -1;
		}
	}

	pSocket->SetIOEvents(nIOEvents);

	return 0;
}

int32_t CEpoll::RemoveEvent(CSocket *pSocket, uint32_t nEvents)
{
	uint32_t nIOEvents = pSocket->GetIOEvents();
	nIOEvents &= (~nEvents);

	struct epoll_event event;
	event.data.ptr = (void *)pSocket;
	event.events = nIOEvents;
	if (epoll_ctl(m_nEpollFD, EPOLL_CTL_MOD, pSocket->GetSocketFD(), &event) < 0)
	{
		printf("3333nsocketfd=%d, errno=%d, desc=%s\n", pSocket->GetSocketFD(), errno, strerror(errno));
		return -1;
	}

	pSocket->SetIOEvents(nIOEvents);

	return 0;
}

int32_t CEpoll::DeleteEvent(CSocket *pSocket)
{
	uint32_t nIOEvents = pSocket->GetIOEvents();

	struct epoll_event event;
	event.data.ptr = pSocket;
	event.events = nIOEvents;
	if (epoll_ctl(m_nEpollFD, EPOLL_CTL_DEL, pSocket->GetSocketFD(), &event) < 0)
	{
		printf("4444nsocketfd=%d, errno=%d, desc=%s\n", pSocket->GetSocketFD(), errno, strerror(errno));
		return -1;
	}

	pSocket->SetIOEvents(0);

	return 0;
}

int32_t CEpoll::HandleMessage(int32_t nWaitTimeout)
{
	if(m_nEpollFD < 0)
	{
		return -1;
	}

	int32_t nEventCount = epoll_wait(m_nEpollFD, m_pEpollEvent, m_nMaxFD, nWaitTimeout);
	if(nEventCount < 0)
	{
		return -1;
	}

	for(int32_t i = 0; i < nEventCount; ++i)
	{
		CSocket *pSocket = (CSocket *)m_pEpollEvent[i].data.ptr;
		if(pSocket == NULL)
		{
			continue;
		}

		if((m_pEpollEvent[i].events & EPOLLIN) != 0)
		{
			pSocket->ReadEvent();

			SocketStatus nSocketStatus = pSocket->GetStatus();
			if((nSocketStatus == enmSocketStatus_Closed) || (nSocketStatus == enmSocketStatus_Error))
			{
				continue;
			}
		}
		if((m_pEpollEvent[i].events & EPOLLOUT) != 0)
		{
			pSocket->WriteEvent();

			SocketStatus nSocketStatus = pSocket->GetStatus();
			if((nSocketStatus == enmSocketStatus_Closed) || (nSocketStatus == enmSocketStatus_Error))
			{
				continue;
			}
		}
		if((m_pEpollEvent[i].events & EPOLLERR) != 0)
		{
			pSocket->ErrorEvent();

			SocketStatus nSocketStatus = pSocket->GetStatus();
			if((nSocketStatus == enmSocketStatus_Closed) || (nSocketStatus == enmSocketStatus_Error))
			{
				continue;
			}
		}
		if((m_pEpollEvent[i].events & EPOLLHUP) != 0)
		{
			pSocket->ErrorEvent();

			SocketStatus nSocketStatus = pSocket->GetStatus();
			if((nSocketStatus == enmSocketStatus_Closed) || (nSocketStatus == enmSocketStatus_Error))
			{
				continue;
			}
		}
		if(!(m_pEpollEvent[i].events & EPOLLIN) &&
				!(m_pEpollEvent[i].events & EPOLLOUT) &&
				!(m_pEpollEvent[i].events & EPOLLERR) &&
				!(m_pEpollEvent[i].events & EPOLLHUP))
		{
			g_FrameLogEngine.WriteNetioLog(enmLogLevel_Error, "it's not found ioevent = 0x%08x\n", m_pEpollEvent[i].events);
		}
	}

	return nEventCount;
}

FRAME_NAMESPACE_END
