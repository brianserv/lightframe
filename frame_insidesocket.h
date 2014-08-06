/*
 * frame_insidesocket.h
 *
 *  Created on: 2012-12-3
 *      Author: jimm
 */

#ifndef FRAME_INSIDESOCKET_H_
#define FRAME_INSIDESOCKET_H_

#include "common/common_codeengine.h"
#include "frame_namespace.h"
#include "frame_socket.h"
#include "frame_socket_mgt.h"

FRAME_NAMESPACE_BEGIN

class CInsideSocket : public CSocket
{
public:
	CInsideSocket();

	virtual ~CInsideSocket();

	virtual void Reset();

	virtual void Clear();

	virtual int32_t OnRead(int32_t nErrorCode);

	virtual int32_t OnWrite(int32_t nErrorCode);

	virtual int32_t OnError(int32_t nErrorCode);

	virtual int32_t OnConnected();

	virtual int32_t OnDisconnect(int32_t nErrorCode);

	//�������ӻص�
	virtual int32_t OnAccept(SocketFD nAcceptFD, struct sockaddr_in stPeerAddress,
			socklen_t nPeerAddressLen);

protected:
	//�ӽ��ջ�������ȡһ����Ϣ��
	virtual int32_t MakeMessage();

	//����peer�˵�ע����Ϣ
	void ProcessRegistMessage(MessageHeadSS *pMsgHead);

private:
	//���ͻ�����ʣ�������
	//int32_t SendRestData();

	//�������ӹر��¼�
	int32_t MakeConnCloseEvent(uint16_t nEventID);

	//����ע����Ϣ
	int32_t SendRegisteNotify(EntityType nDestType, ServerID nDestID);

	//�������ӶϿ���Ϣ
	int32_t SendDisconnectNotify(EntityType nDestType, ServerID nDestID);

private:
	int32_t			m_nCurPacketSize;			//��ǰ���ĳ���
	int32_t			m_nPacketOffset;			//�Ѿ��ж���������
	NetPacket		*m_pPacket;					//��װ��Ӧ�ò�������
};

FRAME_NAMESPACE_END

#endif /* FRAME_INSIDESOCKET_H_ */
