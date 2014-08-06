/*
 * frame_baseconfig.h
 *
 *  Created on: 2011-11-22
 *      Author: jimm
 */

#ifndef FRAME_BASECONFIG_H_
#define FRAME_BASECONFIG_H_

#include <memory.h>
#include "tinyxml/tinyxml.h"
#include "common/common_singleton.h"
#include "common/common_tcpsocket.h"
#include "common/common_def.h"
#include "frame_namespace.h"
#include "frame_errordef.h"
#include "frame_logengine.h"
#include "frame_def.h"
#include "lightframe_impl.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define  enmMaxDBCount 10

FRAME_NAMESPACE_BEGIN

#ifndef MAX_PEERNODE_COUNT
#define MAX_PEERNODE_COUNT 32
#endif

#ifndef MAX_NETCONFIG_COUNT
#define MAX_NETCONFIG_COUNT 32
#endif

class NetConfig
{
public:
	NetConfig()
	{
		memset(m_szListenAddress, 0, sizeof(m_szListenAddress));
		m_nListenIP = 0;
		m_nListenPort = 0;
		m_nMaxConnectNum = 0;
		m_nSocketAttr = enmSocketAttr_Unknown;
		memset(m_szType, 0, sizeof(m_szType));
		memset(m_szDesc, 0, sizeof(m_szDesc));
	}

	char			m_szListenAddress[enmMaxIPAddressLength];
	uint32_t		m_nListenIP;
	uint16_t		m_nListenPort;
	int32_t			m_nMaxConnectNum;
	SocketAttr		m_nSocketAttr;
	char			m_szType[enmMaxDescriptionLength];
	char			m_szDesc[enmMaxDescriptionLength];
};

class PeerNodeConfig
{
public:
	PeerNodeConfig()
	{
		memset(m_szPeerAddress, 0 , sizeof(m_szPeerAddress));
		m_nPeerIP = 0;
		m_nPeerPort = 0;
		m_nServerType = 0;
		m_nServerID = enmInvalidServerID;
	}

	char			m_szPeerAddress[enmMaxIPAddressLength];
	uint32_t		m_nPeerIP;
	uint16_t		m_nPeerPort;
	EntityType		m_nServerType;
	ServerID		m_nServerID;
};

class CFrameBaseConfig : public IServerConfig_Impl , public IConfigCenter
{
public:
	CFrameBaseConfig();
	virtual ~CFrameBaseConfig();

	void SetFrameBaseConfigFilePath(const char* szServerName)
	{
		strcpy(m_szServerName, szServerName);

		memset(m_szConfigFile, 0, sizeof(m_szConfigFile));
		sprintf(m_szConfigFile, "./config/%s.xml", szServerName);
	}

	char *GetFrameBaseConfigFilePath()
	{
		return m_szConfigFile;
	}

	virtual int32_t Initialize(const char* szFileName = NULL, const int32_t type=0);
	virtual int32_t Reload(const char* szFileName = NULL, const int32_t type=0);
	virtual int32_t Uninitialize();

	virtual int32_t LoadCommon(TiXmlElement *pRoot);
	virtual int32_t LoadLogFileSize(TiXmlElement * pRoot);
	virtual int32_t LoadNetConfig(TiXmlElement * pRoot);
	virtual int32_t LoadPeerNodeConfig(TiXmlElement * pRoot);
public:
	const char* GetServerName()
	{
		return m_szServerName;
	}

	//获取Server配置
	int32_t GetZoneID()
	{
		return m_nZoneID;
	}

	//Server类型
	EntityType GetServerType()
	{
		return m_nEntityType;
	}

	//ServerID
	ServerID GetServerID()
	{
		return m_nServerID;
	}

	NetType GetNetType()
	{
		return m_nNetType;
	}

	//日志等级
	LogLevel GetLogLevel()
	{
		return m_logLevel;
	}

	//日志文件大小限制
	int32_t GetLogFileSize()
	{
		return m_nLogFileSize;
	}

	int32_t GetPeerNodeCount()
	{
		return m_nPeerNodeConfigCount;
	}

	int32_t GetPeerNodeConf(PeerNodeConfig*& pPeerConfig, int32_t &nPeerNodeCount)
	{
		pPeerConfig = m_arrPeerNodeConfig;
		nPeerNodeCount = m_nPeerNodeConfigCount;
		return S_OK;
	}

	int32_t GetPeerNodeConfig(PeerNodeConfig arrPeerNodeConfig[], int32_t &nPeerNodeCount)
	{
		memcpy(arrPeerNodeConfig, m_arrPeerNodeConfig, sizeof(m_arrPeerNodeConfig));
		nPeerNodeCount = m_nPeerNodeConfigCount;

		return m_nPeerNodeConfigCount;
	}

	int32_t GetNetConfig(NetConfig arrNetConfig[])
	{
		memcpy(arrNetConfig, m_arrNetConfig, sizeof(m_arrNetConfig));
		return m_nNetConfigCount;
	}

	int32_t GetNetConfigByDesc(const char *szDesc, NetConfig &stNetConfig) const
	{
		for(int32_t i = 0; i < m_nNetConfigCount; ++i)
		{
			if(strncmp(szDesc, m_arrNetConfig[i].m_szDesc, strlen(m_arrNetConfig[i].m_szDesc)) == 0)
			{
				stNetConfig = m_arrNetConfig[i];
				return 0;
			}
		}

		return -1;
	}

	int32_t GetAppThreadCount()
	{
		return m_nAppThreadCount;
	}
protected:
	char					m_szConfigFile[enmMaxFileNameLength];
	char					m_szServerName[enmMaxFileNameLength];
	LogLevel				m_logLevel;
	int32_t					m_nZoneID;
	EntityType				m_nEntityType;
	ServerID				m_nServerID;
	NetType					m_nNetType;
	int32_t					m_nLogFileSize;
	int32_t					m_nAppThreadCount;

	int32_t					m_nNetConfigCount;
	NetConfig				m_arrNetConfig[MAX_NETCONFIG_COUNT];

	int32_t					m_nPeerNodeConfigCount;
	PeerNodeConfig			m_arrPeerNodeConfig[MAX_PEERNODE_COUNT];
};

FRAME_NAMESPACE_END


#endif /* FRAME_BASECONFIG_H_ */
