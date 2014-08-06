/*
 * frame_netthread.cpp
 *
 *  Created on: 2012-11-1
 *      Author: jimm
 */

#include "frame_protocolhead.h"
#include "common/common_api.h"
#include "common/common_datetime.h"
#include "common/common_message.h"
#include "common/common_socket.h"
#include "common/common_tcpsocket.h"
#include "common/common_codeengine.h"
#include "frame_netthread.h"
#include "frame_netqueue.h"
#include "frame_configmgt.h"
#include "frame_socket_mgt.h"
#include "frame_eventid.h"
#include "frame_sockettimer.h"

#ifndef WIN32
#include<cstring>
#include <errno.h>
#endif

FRAME_NAMESPACE_BEGIN

CFrameNetThread::CFrameNetThread()
{
	m_nReconnectTime = 10;
	m_nLastConnectTime = 0;
}

CFrameNetThread::~CFrameNetThread()
{

}

int32_t CFrameNetThread::Initialize()
{
	m_nNetConfigCount = g_FrameConfigMgt.GetFrameBaseConfig().GetNetConfig(m_arrNetConfig);

	int32_t nRet = m_stEpoll.Create(0xffff);
	if(nRet < 0)
	{
		return -1;
	}

	for(int32_t i = 0; i < m_nNetConfigCount; ++i)
	{
		NetConfig *pNetConfig = &m_arrNetConfig[i];
		if(pNetConfig->m_nListenPort == 0)
		{
			return -1;
		}

		CSocket *pSocket = g_FrameSocketMgt.CreateSocket(pNetConfig->m_nListenPort);//CreateOutsideSocket();
		if(pSocket == NULL)
		{
			g_FrameLogEngine.WriteNetioLog(enmLogLevel_Error, "[%s:%d]create socket object error!\n",
				__FILE__, __LINE__);
			return -1;
		}

		pSocket->SetEpollObj(&m_stEpoll);
		pSocket->OpenSocketAsServer(pNetConfig->m_szListenAddress, pNetConfig->m_nListenPort);
		pSocket->SetSocketStatus(enmSocketStatus_Connected);
		pSocket->SetCreateTime(time(NULL));

//		m_stEpoll.Control(pSocket->GetSocketFD(), pSocket, EPOLL_CTL_ADD, EPOLLIN | EPOLLET);
		m_stEpoll.RegistEvent(pSocket, EPOLLIN | EPOLLET);

		g_FrameSocketMgt.AddListenSocket(pSocket);
	}

	return S_OK;
}

int32_t CFrameNetThread::Resume()
{
	return S_OK;
}

int32_t CFrameNetThread::Uninitialize()
{
	return S_OK;
}

void CFrameNetThread::Execute()
{
	while(!GetTerminated())
	{
		//连接所有结点
		ConnectAllPeer();
		//消息泵
		MessagePump();
		//处理socket超时事件
		HandleTimeOutEvent();
	}
}

CEpoll *CFrameNetThread::GetEpollObj()
{
	return &m_stEpoll;
}

int32_t CFrameNetThread::GetZoneID(uint8_t nRouterType)
{
	return 0;
}

uint16_t CFrameNetThread::GetRouterIndex(uint32_t nRouterAddress, uint16_t nRouterPort)
{
	return 0;
}

int32_t CFrameNetThread::MessagePump()
{
	int32_t nServerMessageResult = SendMessagesToServer();
	int32_t nClientMessageResult = SendMessagesToClient();

	int32_t nWaitTimeout = 0;
	if((nClientMessageResult == S_FALSE) && (nServerMessageResult == S_FALSE))
	{
		nWaitTimeout = 50;
	}
	int32_t nEventCount = m_stEpoll.HandleMessage(nWaitTimeout);

	return nEventCount;
}

int32_t CFrameNetThread::SendMessagesToServer()
{
	int32_t ret = S_OK;
	NetPacket *pNetPacket = NULL;
	//从队列中取出数据
	ret = g_FrameNetQueue.PopSendSSQueue(pNetPacket);
	if ((0 > ret) || (pNetPacket == NULL))
	{
		return S_FALSE;
	}

	MessageHeadSS stMessageHead;
	uint32_t nOffset = 0;
	ret = stMessageHead.MessageDecode(&pNetPacket->m_pNetPacket[0], pNetPacket->m_nNetPacketLen, nOffset);
	if(ret < 0)
	{
		FREE((uint8_t *)pNetPacket);//g_FrameMemMgt.RecycleBlock((uint8_t *)pNetPacket);
		g_FrameLogEngine.WriteNetioLog(enmLogLevel_Error, "decode messagehead failed!\n");
		return ret;
	}

	int32_t nSocketCount = 0;
	CSocket *arrSocket[enmMaxConnectCount];

	GetSocket(stMessageHead.nTransType, stMessageHead.nDestType, stMessageHead.nDestID,
			arrSocket, enmMaxConnectCount, nSocketCount);

	int32_t nSendBytes = 0;
	for(int32_t i = 0; i < nSocketCount; ++i)
	{
		ret = arrSocket[i]->Send(&pNetPacket->m_pNetPacket[0], pNetPacket->m_nNetPacketLen, nSendBytes);
		if(ret < 0)
		{
			g_FrameLogEngine.WriteNetioLog(enmLogLevel_Error, "send data failed!{nSendBytes=%d, "
					"len=%d, MessageID=0x%08x, PeerAddress=%s, PeerPort=%d, errno=%d, desc=%s}\n",
					nSendBytes, pNetPacket->m_nNetPacketLen, stMessageHead.nMessageID,
					arrSocket[i]->GetPeerAddressStr(), arrSocket[i]->GetPeerPort(), errno, strerror(errno));

			if(ret < 0)
			{
				//关闭连接
				arrSocket[i]->CloseSocket(SYS_EVENT_CONN_ERROR);
			}

			//设置重连间隔时间为0
			m_nReconnectTime = 0;
		}
		else
		{
			if(nSendBytes < pNetPacket->m_nNetPacketLen)
			{
				if(errno != EAGAIN)
				{
					arrSocket[i]->CloseSocket(SYS_EVENT_CONN_ERROR);
				}
			}
		}
	}

	FREE((uint8_t *)pNetPacket);//g_FrameMemMgt.RecycleBlock((uint8_t *)pNetPacket);

	return S_OK;
}

int32_t CFrameNetThread::SendMessagesToClient()
{
	int32_t ret = S_OK;
	NetPacket *pNetPacket = NULL;
	//从队列中取出数据
	ret = g_FrameNetQueue.PopSendCSQueue(pNetPacket);
	if ((0 > ret) || (pNetPacket == NULL))
	{
		return S_FALSE;
	}

	ConnInfo *pConnInfo = (ConnInfo *)&pNetPacket->m_pNetPacket[0];

	int32_t nSocketCount = 0;
	CSocket *arrSocket[enmMaxConnectCount];

	GetSocket(enmTransType_P2P, enmEntityType_Client, pConnInfo->nTunnelIndex,
			arrSocket, enmMaxConnectCount, nSocketCount);

	if(pConnInfo->nErrorCode > 0)
	{
		for(int32_t i = 0 ;i < nSocketCount; i++)
		{
			arrSocket[i]->CloseSocket(pConnInfo->nErrorCode);
		}
		FREE((uint8_t *)pNetPacket);
		return S_OK;
	}

	uint8_t *pBufCS = &pNetPacket->m_pNetPacket[sizeof(ConnInfo)];
	uint32_t nBufSizeCS = pNetPacket->m_nNetPacketLen - sizeof(ConnInfo);

	//web消息包的编码方式不同，此次不进行解码
//	uint32_t nOffset = 0;
//	MessageHeadCS stMessageHead;
//	ret = stMessageHead.MessageDecode(pBufCS, nBufSizeCS, nOffset);
//	if(ret < 0)
//	{
//		FREE((uint8_t *)pNetPacket);//g_FrameMemMgt.RecycleBlock((uint8_t *)pNetPacket);
//		g_FrameLogEngine.WriteNetioLog(enmLogLevel_Error, "decode messagehead failed!\n");
//		return ret;
//	}

	int32_t nSendBytes = 0;
	for(int32_t i = 0; i < nSocketCount; ++i)
	{
		ret = arrSocket[i]->Send(pBufCS, nBufSizeCS, nSendBytes);
		if(ret < 0)
		{
			g_FrameLogEngine.WriteNetioLog(enmLogLevel_Error, "send data failed!{nSendBytes=%d, "
					"len=%d, PeerAddress=%s, PeerPort=%d, errno=%d, desc=%s}\n",
					nSendBytes, pNetPacket->m_nNetPacketLen, /*stMessageHead.nMessageID,*/
					arrSocket[i]->GetPeerAddressStr(), arrSocket[i]->GetPeerPort(), errno, strerror(errno));

			if(ret < 0)
			{
				//关闭连接
				arrSocket[i]->CloseSocket(SYS_EVENT_CONN_ERROR);
			}
		}
		else
		{
			if((uint32_t)nSendBytes < nBufSizeCS)
			{
				if(errno != EAGAIN)
				{
					arrSocket[i]->CloseSocket(SYS_EVENT_CONN_ERROR);
				}
			}
		}
	}

	FREE((uint8_t *)pNetPacket);//g_FrameMemMgt.RecycleBlock((uint8_t *)pNetPacket);

	return S_OK;
}

void CFrameNetThread::ConnectAllPeer()
{
	int32_t nNowTime = CDateTime::CurrentDateTime().Seconds();
	if(m_nLastConnectTime + m_nReconnectTime > nNowTime)
	{
		return;
	}

	m_nReconnectTime = RECONNECT_TIME;
	m_nLastConnectTime = nNowTime;

	int32_t nPeerNodeConfigCount = 0;
	PeerNodeConfig	arrPeerNodeConfig[MAX_PEERNODE_COUNT];
	g_FrameConfigMgt.GetFrameBaseConfig().GetPeerNodeConfig(arrPeerNodeConfig, nPeerNodeConfigCount);

	for(int32_t i = 0; i < nPeerNodeConfigCount; ++i)
	{
		CSocket *pSocket = g_FrameSocketMgt.GetSocket(arrPeerNodeConfig[i].m_nServerType, arrPeerNodeConfig[i].m_nServerID);
		if(pSocket != NULL)
		{
			if((pSocket->GetStatus() != enmSocketStatus_Closed) && (pSocket->GetStatus() != enmSocketStatus_Error))
			{
				continue;
			}
		}
		else
		{
			pSocket = g_FrameSocketMgt.CreateSocket(arrPeerNodeConfig[i].m_nServerType);//CreateInsideSocket();
		}

		if(pSocket == NULL)
		{
			g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "[%s:%d]create socket object error!\n", 
				__FILE__, __LINE__);
			continue;
		}

		pSocket->SetCreateTime(time(NULL));

		ConnectPeerNode(pSocket, arrPeerNodeConfig[i]);
	}
}

void CFrameNetThread::ConnectPeerNode(CSocket *pSocket, PeerNodeConfig &stPeerNode)
{
	if(pSocket == NULL)
	{
		return;
	}

	if((pSocket->GetStatus() == enmSocketStatus_Connecting) ||
			(pSocket->GetStatus() == enmSocketStatus_Connected) ||
			(pSocket->GetStatus() == enmSocketStatus_Available))
	{
		return;
	}

	if((pSocket->GetStatus() != enmSocketStatus_Closed) && (pSocket->GetStatus() != enmSocketStatus_Error))
	{
		g_FrameLogEngine.WriteNetioLog(enmLogLevel_Error, "socket status is wrong!{SocketStatus=%d, "
				"PeerAddress=%s, PeerPort=%d}\n", pSocket->GetStatus(), pSocket->GetPeerAddressStr(),
				pSocket->GetPeerPort());

		pSocket->CloseSocket(SYS_EVENT_CONN_CONFLICT);
		return;
	}

	int32_t ret = pSocket->OpenSocketAsClient();
	if(0 > ret)
	{
		g_FrameLogEngine.WriteNetioLog(enmLogLevel_Error, "open socket failed!{ret=0x%08x, PeerAddress=%s, "
				"PeerPort=%d}\n", ret, pSocket->GetPeerAddressStr(), pSocket->GetPeerPort());

		pSocket->CloseSocket(ret);
		return;
	}

	m_stEpoll.RegistEvent(pSocket, EPOLLIN | EPOLLOUT);

	pSocket->SetPeerID(stPeerNode.m_nServerID);
	pSocket->SetPeerType(stPeerNode.m_nServerType);
	pSocket->SetEpollObj(&m_stEpoll);

	ret = pSocket->Connect(stPeerNode.m_szPeerAddress, stPeerNode.m_nPeerPort);
	if(0 > ret && ret != (int32_t)E_SOCKET_CONNECTING)
	{
		g_FrameLogEngine.WriteNetioLog(enmLogLevel_Error, "connect is failed!{ret=0x%08x, PeerAddress=%s, "
				"PeerPort=%d}\n", ret, pSocket->GetPeerAddressStr(), pSocket->GetPeerPort());

		pSocket->CloseSocket(ret);
		return;
	}

	if(ret == (int32_t)E_SOCKET_CONNECTING)
	{
		g_FrameLogEngine.WriteNetioLog(enmLogLevel_Notice, "[%s:%d] connecting in process!{PeerAddress=%s, PeerPort=%d}\n",
				__FILE__, __LINE__, pSocket->GetPeerAddressStr(), pSocket->GetPeerPort());
		return;
	}

	g_FrameLogEngine.WriteNetioLog(enmLogLevel_Notice, "[%s:%d] connect peer success! peerip=%s, peerport=%d\n", 
		__FILE__, __LINE__, stPeerNode.m_szPeerAddress, stPeerNode.m_nPeerPort);

	return;
}

int32_t CFrameNetThread::GetSocket(TransType nTransType, EntityType nDestType, int32_t nDestID,
		CSocket *arrSocket[], uint32_t nSize, int32_t &nSocketCount)
{
	nSocketCount = 0;

	if(nTransType == enmTransType_P2P || nTransType == enmTransType_Regist)
	{
		arrSocket[nSocketCount] = g_FrameSocketMgt.GetSocket(nDestType, nDestID);
		if(arrSocket[nSocketCount] != NULL)
		{
			++nSocketCount;
		}
	}
	else if(nTransType == enmTransType_Broadcast)
	{
		if(nDestType == enmEntityType_None)
		{
			g_FrameSocketMgt.GetAllInsideSocket(arrSocket, nSize, nSocketCount);
		}
		else
		{
			g_FrameSocketMgt.GetSocket(nDestType, arrSocket, nSize, nSocketCount);
		}
	}
	else if((nTransType == enmTransType_ByKey) || (nTransType == enmTransType_Ramdom))
	{
		g_FrameSocketMgt.GetSocket(nDestType, arrSocket, nSize, nSocketCount);
		if(nSocketCount > 0)
		{
			int32_t nIndex = Random(nSocketCount);
			arrSocket[0] = arrSocket[nIndex];
			nSocketCount = 1;
		}
	}
	else
	{
		g_FrameLogEngine.WriteNetioLog(enmLogLevel_Error, "transtype not correct use!"
				"{nTransType=%d, nDestType=%d, nDestID=%d}\n", nTransType, nDestType, nDestID);
	}

	if(nSocketCount <= 0)
	{
		//用router服务器转发
		g_FrameSocketMgt.GetSocket(enmEntityType_Router, arrSocket, nSize, nSocketCount);
		if(nSocketCount > 0)
		{
			int32_t nIndex = Random(nSocketCount);
			arrSocket[0] = arrSocket[nIndex];
			nSocketCount = 1;
		}
	}

	return nSocketCount;
}

int32_t CFrameNetThread::HandleTimeOutEvent()
{
	int32_t nTimerCount = g_FrameSocketTimerMgt.GetTimerCount();

	for(int32_t i = 0; i < nTimerCount; ++i)
	{
		CFrameSocketTimer *pTimer = g_FrameSocketTimerMgt.GetFirstSocketTimer();
		if(pTimer->m_nEndTime > CDateTime::CurrentDateTime().Seconds())
		{
			break;
		}

		CSocket *pSocket = pTimer->pSocket;
		SocketTimerProc pTimerProc = pTimer->pTimerProc;

		(pSocket->*pTimerProc)(pTimer);
	}

	return S_OK;
}

FRAME_NAMESPACE_END

