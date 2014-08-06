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

	//接收连接回调
	virtual int32_t OnAccept(SocketFD nAcceptFD, struct sockaddr_in stPeerAddress,
			socklen_t nPeerAddressLen);

protected:
	//从接收缓存中提取一个消息包
	virtual int32_t MakeMessage();

	//处理peer端的注册消息
	void ProcessRegistMessage(MessageHeadSS *pMsgHead);

private:
	//发送缓存中剩余的数据
	//int32_t SendRestData();

	//生成链接关闭事件
	int32_t MakeConnCloseEvent(uint16_t nEventID);

	//发送注册消息
	int32_t SendRegisteNotify(EntityType nDestType, ServerID nDestID);

	//发送连接断开消息
	int32_t SendDisconnectNotify(EntityType nDestType, ServerID nDestID);

private:
	int32_t			m_nCurPacketSize;			//当前包的长度
	int32_t			m_nPacketOffset;			//已经有多少数据了
	NetPacket		*m_pPacket;					//封装给应用层的网络包
};

FRAME_NAMESPACE_END

#endif /* FRAME_INSIDESOCKET_H_ */
