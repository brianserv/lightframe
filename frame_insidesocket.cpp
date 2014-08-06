/*
 * frame_serversocket.cpp
 *
 *  Created on: 2012-12-3
 *      Author: jimm
 */

#include "frame_netqueue.h"
#include "frame_insidesocket.h"
#include "frame_eventid.h"
#include "frame_configmgt.h"

#include "../public/public_typedef.h"

FRAME_NAMESPACE_BEGIN

CInsideSocket::CInsideSocket()
{
	Reset();
}

CInsideSocket::~CInsideSocket()
{

}

void CInsideSocket::Reset()
{
	m_nSocketAttr = enmSocketAttr_Inside;

	Clear();
}

void CInsideSocket::Clear()
{
	CSocket::Clear();

	if(m_pPacket != NULL)
	{
		FREE((uint8_t *)m_pPacket);
	}
	m_pPacket = NULL;
	m_nCurPacketSize = 0;
	m_nPacketOffset = 0;
}

int32_t CInsideSocket::OnRead(int32_t nErrorCode)
{
	uint8_t arrBuf[enmMaxMessageSize];

	int32_t nCloseCode = 0;
	int32_t nRecvBytes = 0;
	int32_t nRet = Recv(arrBuf, sizeof(arrBuf), nRecvBytes);
	if(nRet != S_OK)
	{
		nCloseCode = nRet;
	}

	m_stRecvBuffer.Write(arrBuf, nRecvBytes);
	//��ȡ��Ϣ��
	MakeMessage();

	if(nCloseCode != 0)
	{
		CloseSocket(nCloseCode);
		return E_SOCKETERROR;
	}

	return S_OK;
}

int32_t CInsideSocket::OnWrite(int32_t nErrorCode)
{
	if(nErrorCode != 0)
	{
		CloseSocket(SYS_EVENT_CONN_ERROR);
		return E_SOCKETERROR;
	}

	int32_t nRet = SendRestData();
	if(nRet < 0)
	{
		CloseSocket(SYS_EVENT_CONN_ERROR);
		return E_SOCKETERROR;
	}

	return S_OK;
}

int32_t CInsideSocket::OnError(int32_t nErrorCode)
{
	CloseSocket(SYS_EVENT_CONN_ERROR);
	return S_OK;
}

int32_t CInsideSocket::OnConnected()
{
	m_pEpoll->RegistEvent(this, EPOLLIN);

	//����ע����Ϣ
	if(SendRegisteNotify(m_nPeerType,m_nPeerID) < 0)
	{
		CloseSocket(SYS_EVENT_CONN_REGISTFAILED);
		return S_OK;
	}

	//�ɹ�����֪ͨ����Ϊ����״̬
	SetSocketStatus(enmSocketStatus_Available);

	//���ͻ�����ʣ������
	int32_t nRet = SendRestData();
	if(nRet < 0)
	{
		CloseSocket(SYS_EVENT_CONN_ERROR);
		return E_SOCKETERROR;
	}

	//��ȡ���ջ����е���Ϣ
	MakeMessage();

	return S_OK;
}

//�������ӻص�
int32_t CInsideSocket::OnAccept(SocketFD nAcceptFD, struct sockaddr_in stPeerAddress,
		socklen_t nPeerAddressLen)
{
	CSocket *pSocket = g_FrameSocketMgt.CreateSocket(m_nListenPort);//g_FrameSocketMgt.CreateInsideSocket();
	if(pSocket == NULL)
	{
		g_FrameLogEngine.WriteNetioLog(enmLogLevel_Error, "[%s:%d]create socket object error!\n", 
			__FILE__, __LINE__);
		return -1;
	}

	if(m_pEpoll == NULL)
	{
		g_FrameLogEngine.WriteNetioLog(enmLogLevel_Error, "m_pEpoll is null!\n");
		return -1;
	}

	pSocket->Clear();
	pSocket->SetSocketFD(nAcceptFD);
	pSocket->SetSocketStatus(enmSocketStatus_Connected);
	pSocket->SetPeerAddress((uint32_t)stPeerAddress.sin_addr.s_addr);
	pSocket->SetPeerPort(stPeerAddress.sin_port);
	pSocket->SetCreateTime(time(NULL));
	pSocket->SetEpollObj(m_pEpoll);

	m_pEpoll->RegistEvent(pSocket, EPOLLIN | EPOLLOUT);

	set_non_block(nAcceptFD);

	return S_OK;
}

int32_t CInsideSocket::OnDisconnect(int32_t nCloseCode)
{
	m_pEpoll->DeleteEvent(this);

	//�������ӶϿ�֪ͨ
	SendDisconnectNotify(m_nPeerType, m_nPeerID);

	//���peer�����socket�����ӳ��
	g_FrameSocketMgt.DelSocket(m_nPeerType, m_nPeerID);
	//���մ�socket����
	g_FrameSocketMgt.DestroySocketObject(this);

	g_FrameLogEngine.WriteNetioLog(enmLogLevel_Warning, "connection is close!{nCloseCode=0x%08x,"
			"nPeerType=%d, nPeerID=%d}\n", nCloseCode, m_nPeerType, m_nPeerPort);
	return S_OK;
}

//�������ӹر��¼�
int32_t CInsideSocket::MakeConnCloseEvent(uint16_t nEventID)
{
	int32_t nPacketLen = sizeof(NetPacket) + sizeof(ConnInfo);
	uint8_t *pMem = MALLOC(nPacketLen);//g_FrameMemMgt.AllocBlock(nPacketLen);
	if(pMem == NULL)
	{
		return E_NULLPOINTER;
	}

	NetPacket *pNetPacket = new(pMem)NetPacket();
	pNetPacket->m_nNetPacketLen = nPacketLen;

	ConnInfo *pConnInfo = new(&pNetPacket->m_pNetPacket[0])ConnInfo;
	pConnInfo->nTunnelIndex = (uint16_t)m_nSocketFD;
//	pConnInfo->nRoleID = m_nRoleID;
	pConnInfo->nCreateTime = m_nCreateTime;
	pConnInfo->nAddress = m_nPeerAddress;
	pConnInfo->nPort = m_nPeerPort;
	pConnInfo->nNetType = enmNetType_Tel;
	pConnInfo->nErrorCode = nEventID;

	g_FrameNetQueue.PushRecvSSQueue(pNetPacket);

	return S_OK;
}


int32_t CInsideSocket::MakeMessage()
{
	//��ʼ���
	while(m_stRecvBuffer.Size() > 0)
	{
		//�����ظ���������
		static MessageHeadSS stMsgHead;
		if(m_nCurPacketSize <= 0)
		{
			//��ȡ����С�ֶ���ռ���ֽ���
			int32_t nPacketLengthSize = sizeof(stMsgHead.nTotalSize);
			if(m_stRecvBuffer.Size() < nPacketLengthSize)
			{
				break;
			}

			//ȡ����ͷǰ��İ���С�ֶΣ�Ȼ��������
			uint8_t arrPacketSize[nPacketLengthSize];
			m_stRecvBuffer.PeekRead(arrPacketSize, nPacketLengthSize);

			uint32_t nOffset = 0;
			CCodeEngine::Decode(arrPacketSize, nPacketLengthSize, nOffset, m_nCurPacketSize);
		}

		int32_t nWantDataSize = m_nCurPacketSize - m_nPacketOffset;
		int32_t nRealGetDataSize = (m_stRecvBuffer.Size() >= nWantDataSize ? nWantDataSize : m_stRecvBuffer.Size());
		if(nRealGetDataSize <= 0)
		{
			break;
		}

		if(m_pPacket == NULL)
		{
			int32_t nPacketLen = m_nCurPacketSize;
			uint8_t *pMem = MALLOC(sizeof(NetPacket) + nPacketLen);//g_FrameMemMgt.AllocBlock(sizeof(NetPacket) + nPacketLen);
			if(pMem == NULL)
			{
				break;
			}

			m_pPacket = new(pMem)NetPacket();
			m_pPacket->m_nNetPacketLen = nPacketLen;
		}

		if(m_stRecvBuffer.Read(&m_pPacket->m_pNetPacket[m_nPacketOffset], nRealGetDataSize) <= 0)
		{
			break;
		}

		m_nPacketOffset += nRealGetDataSize;
		if(m_nPacketOffset >= m_nCurPacketSize)
		{
			uint32_t nOffset = 0;
			int32_t nRet = stMsgHead.MessageDecode(&m_pPacket->m_pNetPacket[0], m_nPacketOffset, nOffset);
			if(nRet < 0)
			{
				FREE((uint8_t *)m_pPacket);//g_FrameMemMgt.RecycleBlock((uint8_t *)m_pPacket);
				m_pPacket = NULL;
				m_nPacketOffset = 0;
				m_nCurPacketSize = 0;
				return nRet;
			}


			//��ʾ��ע���
			if(stMsgHead.nTransType == enmTransType_Regist)
			{
				ProcessRegistMessage(&stMsgHead);
			}
			//������ѹ�����
			g_FrameNetQueue.PushRecvSSQueue(m_pPacket);

			m_pPacket = NULL;
			m_nPacketOffset = 0;
			m_nCurPacketSize = 0;
		}
	}

	return S_OK;
}

//����peer�˵�ע����Ϣ
void CInsideSocket::ProcessRegistMessage(MessageHeadSS *pMsgHead)
{
	if(pMsgHead == NULL)
	{
		return;
	}

	//���peer�˺ʹ�socket�����ӳ��
	g_FrameSocketMgt.AddSocket(pMsgHead->nSourceType, pMsgHead->nSourceID, this);

	//����socket���ӵ�servertype��serverID
	SetPeerType(pMsgHead->nSourceType);
	SetPeerID(pMsgHead->nSourceID);

	SetSocketStatus(enmSocketStatus_Available);
}

int32_t CInsideSocket::SendRegisteNotify(EntityType nDestType, ServerID nDestID)
{
	int32_t nPacketLen = sizeof(MessageHeadSS) + sizeof(ConnInfo);
	uint8_t *pMem = MALLOC(sizeof(NetPacket) + nPacketLen);//g_FrameMemMgt.AllocBlock(sizeof(NetPacket) + nPacketLen);
	if(pMem == NULL)
	{
		return E_NULLPOINTER;
	}

	NetPacket *pPacket = new(pMem)NetPacket();

	MessageHeadSS stMessageHeadSS;
	stMessageHeadSS.nTotalSize = 0;
	stMessageHeadSS.nMessageID = 0;
	stMessageHeadSS.nHeadSize = 0;
	stMessageHeadSS.nMessageType = enmMessageType_Notify;
	stMessageHeadSS.nSourceType = g_FrameConfigMgt.GetFrameBaseConfig().GetServerType();
	stMessageHeadSS.nDestType = nDestType;
	stMessageHeadSS.nSourceID = g_FrameConfigMgt.GetFrameBaseConfig().GetServerID();
	stMessageHeadSS.nDestID = nDestID;
	stMessageHeadSS.nRoleID = enmInvalidRoleID;
	stMessageHeadSS.nTransType = enmTransType_Regist;
	stMessageHeadSS.nSessionIndex = enmInvalidSessionIndex;

	//����Ϣ���б���
	uint32_t offset = 0;
	int32_t ret = stMessageHeadSS.MessageEncode(&pPacket->m_pNetPacket[0], nPacketLen, offset);
	if (0 > ret)
	{
		FREE(pMem);

		g_FrameLogEngine.WriteNetioLog(enmLogLevel_Notice,"Encode MessageHeadSS ErrorCode: 0x%08x source type:%d, source id: %d, desttype: %d!\n", ret,
				stMessageHeadSS.nSourceType,stMessageHeadSS.nSourceID,stMessageHeadSS.nDestType);
		return ret;
	}

	uint32_t nTotalSize = offset;
	uint32_t nTotalSizeOffset = offsetof(MessageHeadSS, nTotalSize);
	ret = CCodeEngine::Encode(&pPacket->m_pNetPacket[0], enmMaxMessageSize, nTotalSizeOffset, nTotalSize);

	uint16_t nHeadSize = offset;
	uint32_t nHeadSizeOffset = offsetof(MessageHeadSS, nHeadSize);
	ret = CCodeEngine::Encode(&pPacket->m_pNetPacket[0], enmMaxMessageSize, nHeadSizeOffset, nHeadSize);

	pPacket->m_nNetPacketLen = nTotalSize;

	return g_FrameNetQueue.PushSendSSQueue(pPacket);
}

int32_t CInsideSocket::SendDisconnectNotify(EntityType nDestType, ServerID nDestID)
{
	int32_t nPacketLen = sizeof(MessageHeadSS) + sizeof(ConnInfo);
	uint8_t *pMem = MALLOC(sizeof(NetPacket) + nPacketLen);//g_FrameMemMgt.AllocBlock(sizeof(NetPacket) + nPacketLen);
	if(pMem == NULL)
	{
		return E_NULLPOINTER;
	}

	NetPacket *pPacket = new(pMem)NetPacket();

	MessageHeadSS stMessageHeadSS;
	stMessageHeadSS.nTotalSize = 0;
	stMessageHeadSS.nMessageID = 0;
	stMessageHeadSS.nHeadSize = 0;
	stMessageHeadSS.nMessageType = enmMessageType_Notify;
	stMessageHeadSS.nSourceType = nDestType;
	stMessageHeadSS.nDestType = g_FrameConfigMgt.GetFrameBaseConfig().GetServerType();
	stMessageHeadSS.nSourceID = nDestID;
	stMessageHeadSS.nDestID = g_FrameConfigMgt.GetFrameBaseConfig().GetServerID();
	stMessageHeadSS.nRoleID = enmInvalidRoleID;
	stMessageHeadSS.nTransType = enmTransType_Disconnect;
	stMessageHeadSS.nSessionIndex = enmInvalidSessionIndex;

	//����Ϣ���б���
	uint32_t offset = 0;
	int32_t ret = stMessageHeadSS.MessageEncode(&pPacket->m_pNetPacket[0], nPacketLen, offset);
	if (0 > ret)
	{
		FREE(pMem);

		g_FrameLogEngine.WriteNetioLog(enmLogLevel_Notice,"Encode MessageHeadSS ErrorCode: 0x%08x source type:%d, source id: %d, desttype: %d!\n", ret,
				stMessageHeadSS.nSourceType,stMessageHeadSS.nSourceID,stMessageHeadSS.nDestType);
		return ret;
	}

	uint32_t nTotalSize = offset;
	uint32_t nTotalSizeOffset = offsetof(MessageHeadSS, nTotalSize);
	ret = CCodeEngine::Encode(&pPacket->m_pNetPacket[0], enmMaxMessageSize, nTotalSizeOffset, nTotalSize);

	uint16_t nHeadSize = offset;
	uint32_t nHeadSizeOffset = offsetof(MessageHeadSS, nHeadSize);
	ret = CCodeEngine::Encode(&pPacket->m_pNetPacket[0], enmMaxMessageSize, nHeadSizeOffset, nHeadSize);

	pPacket->m_nNetPacketLen = nTotalSize;

	return g_FrameNetQueue.PushRecvSSQueue(pPacket);
}
FRAME_NAMESPACE_END
