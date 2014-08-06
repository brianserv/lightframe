/*
 * frame_netthread.h
 *
 *  Created on: 2012-11-1
 *      Author: jimm
 */

#ifndef FRAME_NETTHREAD_H_
#define FRAME_NETTHREAD_H_

#include "common/common_singleton.h"
#include "common/common_thread.h"
#include "common/common_message.h"
#include "frame_namespace.h"
#include "frame_typedef.h"
#include "frame_errordef.h"
#include "frame_logengine.h"
#include "frame_socket.h"
#include "frame_baseconfig.h"
#include "frame_epoll.h"

#include "../public/public_typedef.h"


FRAME_NAMESPACE_BEGIN

//enum enmSocketType
//{
//	enmSocketType_invalid = 0,
//	enmSocketType_listen =	1,			//监听socket
//	enmSocketType_common =  2,			//普通socket
//};

//最大的服务器类型个数
//#define MAX_PEERTYPE_COUNT 64

#define RECONNECT_TIME		10//(s)


template<typename FromType, typename ToType>
void GetMemberFuncAddr(FromType f, ToType &addr)
{
	union
	{
		FromType _f;
		ToType _t;
	}ut;

	ut._f = f;
	addr = ut._t;
}

class CFrameNetThread : public CThread
{
public:
	CFrameNetThread();
	virtual ~CFrameNetThread();

	int32_t Initialize();

	int32_t Resume();

	int32_t Uninitialize();

	void Execute();

	CEpoll *GetEpollObj();

	int32_t GetZoneID(uint8_t nRouterType);

	uint16_t GetRouterIndex(uint32_t nRouterAddress, uint16_t nRouterPort);

	void ConnectPeerNode(CSocket *pSocket, PeerNodeConfig &stPeerNode);

protected:
	int32_t MessagePump();

	int32_t SendMessagesToServer();

	int32_t SendMessagesToClient();

	void ConnectAllPeer();

	int32_t GetSocket(TransType nTransType, EntityType nDestType, int32_t nDestID,
			CSocket *arrSocket[], uint32_t nSize, int32_t &nSocketCount);

	int32_t HandleTimeOutEvent();

protected:
	CEpoll		m_stEpoll;

	int32_t		m_nNetConfigCount;
	NetConfig	m_arrNetConfig[MAX_NETCONFIG_COUNT];

	int64_t		m_nReconnectTime;
	int64_t		m_nLastConnectTime;
};


#define	CREATE_FRAMENETTHREAD_INSTANCE		CSingleton<CFrameNetThread>::CreateInstance
#define	g_FrameNetThread					CSingleton<CFrameNetThread>::GetInstance()
#define	DESTROY_FRAMENETTHREAD_INSTANCE		CSingleton<CFrameNetThread>::DestroyInstance

FRAME_NAMESPACE_END

#endif /* FRAME_NETTHREAD_H_ */
