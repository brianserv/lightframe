/*
 * frame_connection_mgt.cpp
 *
 *  Created on: 2012-12-12
 *      Author: jimm
 */

#include "frame_connection_mgt.h"

FRAME_NAMESPACE_BEGIN

CFrameConnectionMgt::CFrameConnectionMgt()
{

}

CFrameConnectionMgt::~CFrameConnectionMgt()
{

}

int32_t CFrameConnectionMgt::Initialize()
{
	MUTEX_GUARD(Lock, m_stConnectionLock);

	m_stConnectionList.Initialize();
	m_stConnectionPool.Initialize();

	return S_OK;
}

int32_t CFrameConnectionMgt::Resume()
{
	MUTEX_GUARD(Lock, m_stConnectionLock);

	m_stConnectionList.Resume();
	m_stConnectionPool.Resume();

	return S_OK;
}

int32_t CFrameConnectionMgt::Uninitialize()
{
	MUTEX_GUARD(Lock, m_stConnectionLock);

	m_stConnectionList.Uninitialize();
	m_stConnectionPool.Uninitialize();

	return S_OK;
}

ConnectionList::Key CFrameConnectionMgt::MakeKey(const SocketFD nSocketFD) const
{
	ConnectionList::Key key = { 0 };
	key.nKeyLength = (uint32_t)sizeof(SocketFD);
	*(SocketFD*)key.arrKey = nSocketFD;

	return key;
}

CFrameConnection *CFrameConnectionMgt::CreateConnection(SocketFD nSocketFD, ConnInfo *pConnInfo)
{
	if(nSocketFD <= 0 || pConnInfo == NULL)
	{
		return NULL;
	}

	MUTEX_GUARD(Lock, m_stConnectionLock);

	ConnectionPool::CIndex* pIndex = NULL;
	pIndex = m_stConnectionPool.CreateObject();
	if(NULL == pIndex)
	{
		return NULL;
	}

	ConnectionList::Key key = MakeKey(nSocketFD);
	ConnectionList::CIndex *pListIndex = m_stConnectionList.Insert(key, pIndex->Index());
	if (NULL == pListIndex)
	{
		m_stConnectionPool.DestroyObject(pIndex);
		pIndex = NULL;
		return NULL;
	}

	//将索引保存到附加数据表中
	int32_t ret = pIndex->SetAdditionalData(enmAdditionalIndex_HashListIndex, (uint32_t)pListIndex->Index());
	if (0 > ret)
	{
		m_stConnectionList.Erase(pListIndex);
		m_stConnectionPool.DestroyObject(pIndex);
		pIndex = NULL;
		return NULL;
	}

	CFrameConnection *pConnection = pIndex->ObjectPtr();
	if(pConnection != NULL)
	{
		pConnection->m_stConnInfo = *pConnInfo;
	}

	return pConnection;
}

int32_t CFrameConnectionMgt::DestroyConnection(SocketFD nSocketFD)
{
	MUTEX_GUARD(Lock, m_stConnectionLock);
	//根据索引查找
	ConnectionList::Key key = MakeKey(nSocketFD);
	ConnectionList::CIndex* pListIndex = m_stConnectionList.Find(key);
	if (NULL == pListIndex)
	{
		return E_UNKNOWN;
	}

	int32_t nIndex = pListIndex->GetObject();

	//从对象池及索引表中删除
	m_stConnectionList.Erase(pListIndex);
	m_stConnectionPool.DestroyObject(nIndex);

	return S_OK;
}

CFrameConnection *CFrameConnectionMgt::GetConnection(SocketFD nSocketFD)
{
	MUTEX_GUARD(Lock, m_stConnectionLock);

	//根据索引查找
	ConnectionList::Key key = MakeKey(nSocketFD);
	ConnectionList::CIndex* pListIndex = m_stConnectionList.Find(key);
	if (NULL == pListIndex)
	{
		return NULL;
	}

	ConnectionPool::CIndex *pIndex = m_stConnectionPool.GetIndex(pListIndex->GetObject());
	if (NULL == pIndex)
	{
		return NULL;
	}

	return pIndex->ObjectPtr();
}


FRAME_NAMESPACE_END
