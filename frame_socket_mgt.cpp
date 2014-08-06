/*
 * frame_socket_mgt.cpp
 *
 *  Created on: 2012-12-2
 *      Author: jimm
 */

#include "frame_outsidesocket.h"
#include "frame_insidesocket.h"
#include "frame_socket_mgt.h"
#include "frame_configmgt.h"

FRAME_NAMESPACE_BEGIN

int32_t CFrameSocketMgt::Initialize()
{
	NetConfig arrNetConfig[MAX_NETCONFIG_COUNT];
	int32_t nNetConfigCount = g_FrameConfigMgt.GetFrameBaseConfig().GetNetConfig(arrNetConfig);

	for(int32_t i = 0; i < nNetConfigCount; ++i)
	{
		NetConfig *pNetConfig = &arrNetConfig[i];
		CSocket *pSocket = NULL;
		for(int32_t j = 0; j < pNetConfig->m_nMaxConnectNum; ++j)
		{
			if(pNetConfig->m_nSocketAttr == enmSocketAttr_Inside)
			{
				pSocket = new CInsideSocket();
			}
			else if(pNetConfig->m_nSocketAttr == enmSocketAttr_Outside)
			{
				pSocket = new COutsideSocket();
			}
			else
			{
				return -1;
			}

			if(pSocket == NULL)
			{
				return -1;
			}

			RegistSocketHandler(pSocket, pNetConfig->m_nListenPort);
		}
	}

	int32_t nPeerNodeCount = 0;
	PeerNodeConfig arrPeerNodeConfig[MAX_PEERNODE_COUNT];
	g_FrameConfigMgt.GetFrameBaseConfig().GetPeerNodeConfig(arrPeerNodeConfig, nPeerNodeCount);
	for(int32_t i = 0; i < nPeerNodeCount; ++i)
	{
		CSocket *pSocket = new CInsideSocket();
		if(pSocket == NULL)
		{
			return -1;
		}

		RegistSocketHandler(pSocket, arrPeerNodeConfig[i].m_nServerType);
	}

	return S_OK;
}

int32_t CFrameSocketMgt::Resume()
{
	return S_OK;
}

int32_t CFrameSocketMgt::Uninitialize()
{
	return S_OK;
}

int32_t CFrameSocketMgt::RegistSocketHandler(CSocket *pSocket, uint16_t nListenPort)
{
	if(pSocket == NULL)
	{
		return -1;
	}

	m_stSocketPortMap[nListenPort].push_back(pSocket);
	pSocket->SetBelongPort(nListenPort);

	return 0;
}

void CFrameSocketMgt::UnregistSocketHandler(uint16_t nListenPort)
{
	CSocket *pSocket = GetListenSocket(nListenPort);
	if(pSocket != NULL)
	{
		CEpoll *pEpoll = pSocket->GetEpollObj();
		//pEpoll->Control(pSocket->GetSocketFD(), pSocket, EPOLL_CTL_DEL, EPOLLIN /*| EPOLLET*/);
		//pEpoll->DeleteEvent(pSocket);
		pSocket->CloseSocket();
		DelListenSocket(pSocket);
	}

	int32_t nCount = m_stSocketPortMap[nListenPort].size();
	for(int32_t i = 0; i < nCount; ++i)
	{
		CSocket *pSocket = m_stSocketPortMap[nListenPort].front();
		m_stSocketPortMap[nListenPort].pop_front();
		delete pSocket;
	}
}

int32_t CFrameSocketMgt::RegistSocketHandler(CSocket *pSocket, EntityType nServerType)
{
	if(pSocket == NULL)
	{
		return -1;
	}

	m_stSocketTypeMap[nServerType].push_back(pSocket);
	pSocket->SetPeerType(nServerType);

	return 0;
}

void CFrameSocketMgt::UnregistSocketHandler(EntityType nServerType)
{
	int32_t nCount = m_stSocketTypeMap[nServerType].size();
	for(int32_t i = 0; i < nCount; ++i)
	{
		CSocket *pSocket = m_stSocketTypeMap[nServerType].front();
		m_stSocketTypeMap[nServerType].pop_front();
		delete pSocket;
	}
}

//注销已经在使用的Peer套接字
void CFrameSocketMgt::UnregistUsedSocketHandler(EntityType nServerType)
{
	int32_t nCount = m_stUsedSocketTypeMap[nServerType].size();
	for(int32_t i = 0; i < nCount; ++i)
	{
		CSocket *pSocket = m_stUsedSocketTypeMap[nServerType].front();
		m_stUsedSocketTypeMap[nServerType].pop_front();
		if(pSocket)
		{
			DelSocket(pSocket->GetPeerType(), pSocket->GetPeerID());
			pSocket->CloseSocket();
			DestroySocketObject(pSocket);
		}
	}
}

CSocket *CFrameSocketMgt::CreateSocket(uint16_t nListenPort)
{
	CSocket *pSocket = NULL;
	if(m_stSocketPortMap.count(nListenPort) > 0)
	{
		if(m_stSocketPortMap[nListenPort].size() > 0)
		{
			pSocket = m_stSocketPortMap[nListenPort].front();
			m_stSocketPortMap[nListenPort].pop_front();

			pSocket->SetBelongPort(nListenPort);
		}
	}

	return pSocket;
}

CSocket *CFrameSocketMgt::CreateSocket(EntityType nServerType)
{
	CSocket *pSocket = NULL;
	if(m_stSocketTypeMap.count(nServerType) > 0)
	{
		if(m_stSocketTypeMap[nServerType].size() > 0)
		{
			pSocket = m_stSocketTypeMap[nServerType].front();
			m_stSocketTypeMap[nServerType].pop_front();

			pSocket->SetPeerType(nServerType);

			m_stUsedSocketTypeMap[nServerType].push_back(pSocket);
		}
	}

	return pSocket;
}

int32_t CFrameSocketMgt::DestroySocketObject(CSocket *pSocket)
{
	if(pSocket == NULL)
	{
		return E_NULLPOINTER;
	}

	uint16_t nBelongPort = pSocket->GetBelongPort();
	if(m_stSocketPortMap.count(nBelongPort) > 0)
	{
		m_stSocketPortMap[nBelongPort].push_back(pSocket);
	}
	else if(m_stSocketTypeMap.count(pSocket->GetPeerType()) > 0)
	{
		m_stSocketTypeMap[pSocket->GetPeerType()].push_back(pSocket);
	}

	return S_OK;
}

/*
* 第二个参数nID, 如果是客户端的链接，则是socketfd， 如果是内部server，则是serverid
*/
int32_t CFrameSocketMgt::AddSocket(EntityType nType, int32_t nID, CSocket *pSocket)
{
	if(0 > nID)
	{
		g_FrameLogEngine.WriteNetioLog(enmLogLevel_Error, "[%s:%d]invalid socket fd or serverid in add socket! nID=%d\n",
			__FILE__, __LINE__, nID);
		return E_INVALID_ARGUMENTS;
	}


	if(pSocket == NULL)
	{
		g_FrameLogEngine.WriteNetioLog(enmLogLevel_Error, "[%s:%d]input psocket is null!\n", 
			__FILE__, __LINE__);
		return E_NULLPOINTER;
	}


	if((nID < 0) || (pSocket == NULL))
	{
		return S_OK;
	}

	CSocket *pOldSocket = GetSocket(nType, nID);
	if(pOldSocket != NULL)
	{
		DelSocket(nType, nID);

		if(pOldSocket != pSocket)
		{
			DestroySocketObject(pOldSocket);
		}
	}

	pair< CSocketMap::iterator, bool > ret = m_stServerMap[nType].insert(make_pair(nID, pSocket));
	if(!ret.second)
	{
		return E_UNKNOWN;
	}

	return S_OK;
}

int32_t CFrameSocketMgt::DelSocket(EntityType nType, int32_t nID)
{
	if(m_stServerMap.count(nType) <= 0)
	{
		return S_OK;
	}

	CSocketMap::iterator it = m_stServerMap[nType].find(nID);
	if(it != m_stServerMap[nType].end())
	{
		m_stServerMap[nType].erase(it);
	}

	return S_OK;
}

CSocket *CFrameSocketMgt::GetSocket(EntityType nType, int32_t nID)
{
	CSocket *pSocket = NULL;
	CSocketMap::iterator it = m_stServerMap[nType].find(nID);
	if(it != m_stServerMap[nType].end())
	{
		pSocket = it->second;
	}

	return pSocket;
}

int32_t CFrameSocketMgt::GetSocket(EntityType nType, CSocket *arrSocket[], uint32_t nSize,
		int32_t &nSocketCount)
{
	nSocketCount = 0;
//	uint32_t nCount = (m_stServerMap[nType].size() > nSize) ? nSize : m_stServerMap[nType].size();

	CSocketMap::iterator it;
	for(it = m_stServerMap[nType].begin(); it != m_stServerMap[nType].end(); ++it)
	{
		arrSocket[nSocketCount] = it->second;
		++nSocketCount;
		if(nSocketCount >= (int32_t)nSize)
		{
			break;
		}
	}

//	nSocketCount = nCount;
//	return nCount;
	return nSocketCount;
}

int32_t CFrameSocketMgt::GetAllInsideSocket(CSocket *arrSocket[], uint32_t nSize, int32_t &nSocketCount)
{
	nSocketCount = 0;

	CServerMap::iterator server_it;
	for(server_it = m_stServerMap.begin(); server_it != m_stServerMap.end(); ++server_it)
	{
		EntityType nType = server_it->first;
		//只获取服务器内部套接字对象
		if(nType > enmEntityType_Count)
		{
			continue;
		}

		CSocketMap::iterator socket_it;
		for(socket_it = m_stServerMap[nType].begin(); socket_it != m_stServerMap[nType].end(); ++socket_it)
		{
			arrSocket[nSocketCount] = socket_it->second;
			++nSocketCount;
			if(nSocketCount >= (int32_t)nSize)
			{
				break;
			}
		}
	}

	return nSocketCount;
}

void CFrameSocketMgt::AddListenSocket(CSocket *pSocket)
{
	m_stListenList.push_back(pSocket);
}

void CFrameSocketMgt::DelListenSocket(CSocket *pSocket)
{
	for(CSocketList::iterator it = m_stListenList.begin(); it != m_stListenList.end(); it++)
	{
		if(pSocket == *it)
		{
			m_stListenList.erase(it);
			break;
		}
	}
}

CSocket *CFrameSocketMgt::GetListenSocket(uint16_t nListenPort)
{
	CSocket *pSocket = NULL;
	for(CSocketList::iterator it = m_stListenList.begin(); it != m_stListenList.end(); it++)
	{
		if(((*it)->GetSocketType() == enmSocketType_Listen) &&((*it)->GetListenPort() == nListenPort))
		{
			pSocket = *it;
			break;
		}
	}

	return pSocket;
}

FRAME_NAMESPACE_END
