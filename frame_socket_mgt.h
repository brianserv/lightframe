/*
 * frame_socket_mgt.h
 *
 *  Created on: 2012-12-2
 *      Author: jimm
 */

#ifndef FRAME_SOCKET_MGT_H_
#define FRAME_SOCKET_MGT_H_

#include "common/common_object.h"
#include "common/common_singleton.h"
#include "frame_namespace.h"
#include "frame_typedef.h"
#include "frame_socket.h"
#include "frame_baseconfig.h"
#include <map>
#include <list>

using namespace std;

FRAME_NAMESPACE_BEGIN

//最大的服务器类型个数
#define MAX_PEERTYPE_COUNT 64

typedef map<int32_t, CSocket *> CSocketMap;
typedef map<EntityType, CSocketMap> CServerMap;

typedef list<CSocket *> CSocketList;
typedef map<uint16_t, CSocketList> CSocketPortMap;
typedef map<EntityType, CSocketList> CSocketTypeMap;

class CFrameSocketMgt : public CObject
{
public:
	int32_t Initialize();

	int32_t Resume();

	int32_t Uninitialize();

	int32_t RegistSocketHandler(CSocket *pSocket, uint16_t nListenPort);

	void UnregistSocketHandler(uint16_t nListenPort);

	int32_t RegistSocketHandler(CSocket *pSocket, EntityType nServerType);

	void UnregistSocketHandler(EntityType nServerType);

	//注销已经在使用的Peer套接字
	void UnregistUsedSocketHandler(EntityType nServerType);

	CSocket *CreateSocket(uint16_t nListenPort);

	CSocket *CreateSocket(EntityType nServerType);

	int32_t DestroySocketObject(CSocket *pSocket);

	int32_t AddSocket(EntityType nType, int32_t nID, CSocket *pSocket);

	int32_t DelSocket(EntityType nType, int32_t nID);

	CSocket *GetSocket(EntityType nType, int32_t nID);

	int32_t GetSocket(EntityType nType, CSocket *arrSocket[], uint32_t nSize, int32_t &nSocketCount);

	int32_t GetAllInsideSocket(CSocket *arrSocket[], uint32_t nSize, int32_t &nSocketCount);

	void AddListenSocket(CSocket *pSocket);

	void DelListenSocket(CSocket *pSocket);

	CSocket *GetListenSocket(uint16_t nListenPort);
protected:
	CServerMap			m_stServerMap;

	CSocketList			m_stListenList;

	CSocketPortMap		m_stSocketPortMap;
	CSocketTypeMap		m_stSocketTypeMap;
	CSocketTypeMap		m_stUsedSocketTypeMap;
};

#define	CREATE_FRAMESOCKETMGT_INSTANCE		CSingleton<CFrameSocketMgt>::CreateInstance
#define	g_FrameSocketMgt					CSingleton<CFrameSocketMgt>::GetInstance()
#define	DESTROY_FRAMESOCKETMGT_INSTANCE		CSingleton<CFrameSocketMgt>::DestroyInstance

FRAME_NAMESPACE_END

#endif /* FRAME_SOCKET_MGT_H_ */
