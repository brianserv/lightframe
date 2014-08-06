/*
 * frame_connection_mgt.h
 *
 *  Created on: 2012-12-11
 *      Author: jimm
 */

#ifndef FRAME_CONNECTION_MGT_H_
#define FRAME_CONNECTION_MGT_H_

#include "common/common_object.h"
#include "common/common_pool.h"
#include "common/common_hashlist.h"
#include "common/common_singleton.h"
#include "common/common_mutex.h"
#include "frame_namespace.h"
#include "frame_typedef.h"
#include "frame_connection.h"
#include "common/common_tcpsocket.h"

#include "../public/public_typedef.h"

FRAME_NAMESPACE_BEGIN

typedef CPool<CFrameConnection, MaxOnlinePlayerCount>	ConnectionPool;
typedef CHashList<SocketFD, MaxOnlinePlayerCount, MaxOnlinePlayerCount * 256>	ConnectionList;

class CFrameConnectionMgt : public CObject
{
protected:
	enum
	{
		enmAdditionalIndex_HashListIndex			= 0,
	};
public:
	CFrameConnectionMgt();

	~CFrameConnectionMgt();

	int32_t Initialize();

	int32_t Resume();

	int32_t Uninitialize();

	CFrameConnection *CreateConnection(SocketFD nSocketFD, ConnInfo *pConnInfo);

	int32_t DestroyConnection(SocketFD nSocketFD);

	CFrameConnection *GetConnection(SocketFD nSocketFD);

private:
	ConnectionList::Key MakeKey(const SocketFD nSocketFD) const;

private:
	ConnectionList	m_stConnectionList;
	ConnectionPool	m_stConnectionPool;

	CriticalSection	m_stConnectionLock;
};

#define	CREATE_FRAMECONNECITONINFOMGT_INSTANCE			CSingleton<CFrameConnectionMgt>::CreateInstance
#define	g_FrameConnectionMgt							CSingleton<CFrameConnectionMgt>::GetInstance()
#define	DESTROY_FRAMECONNECITONINFOMGT_INSTANCE			CSingleton<CFrameConnectionMgt>::DestroyInstance

FRAME_NAMESPACE_END

#endif /* FRAME_CONNECTION_MGT_H_ */
