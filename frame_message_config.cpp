/*
 * frame_message_config.cpp
 *
 *  Created on: 2012-12-19
 *      Author: jimm
 */

#include "frame_message_config.h"


FRAME_NAMESPACE_BEGIN

#define ATTRIBUTE_MSG			"msg"
#define ATTRIBUTE_ID			"id"
#define ATTRIBUTE_TYPE			"type"
#define ATTRIBUTE_SOURCETYPE	"source_type"
#define ATTRIBUTE_DESTTYPE		"dest_type"
#define ATTRIBUTE_TRANSTYPE		"trans_type"
#define ATTRIBUTE_TIMEOUT		"timeout"
#define ATTRIBUTE_RESP			"resp"
#define ATTRIBUTE_NOTI			"noti"

CFrameMessageConfig::CFrameMessageConfig()
{
}

CFrameMessageConfig::~CFrameMessageConfig()
{
}

int32_t CFrameMessageConfig::Initialize(const char* szFileName/* = NULL*/, const int32_t type/*=0*/)
{
	strncpy(m_szConfigFile, "./config/message.xml", strlen("./config/message.xml"));
	if(strlen(m_szConfigFile) <= 0)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "Load %s file failed!\n", m_szConfigFile);
		return E_LIBF_LOADCONFIGFILE;
	}

	if(access(m_szConfigFile, 0) < 0)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Info, "it's not found config %s!\n", m_szConfigFile);
		return S_OK;
	}

	TiXmlDocument doc(m_szConfigFile);
	if (!doc.LoadFile(TIXML_ENCODING_UTF8))
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "Load %s file failed!\n", m_szConfigFile);
		return E_LIBF_LOADCONFIGFILE;
	}
	g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "Load %s file success!\n", m_szConfigFile);

	//获取根节点
	TiXmlElement *pRoot = doc.RootElement();
	if (NULL == pRoot)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "Root node cannot be found in %s file!\n", m_szConfigFile);
		return E_LIBF_LOADCONFIGFILE;
	}
	g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "Get root node success!\n");

	int32_t ret = LoadMessageConfig(pRoot);
	if (0 > ret)
	{
		return ret;
	}

	return S_OK;
}

int32_t CFrameMessageConfig::Reload(const char* szFileName/* = NULL*/, const int32_t type/*=0*/)
{
	return Initialize();
}

int32_t CFrameMessageConfig::Uninitialize()
{
	return S_OK;
}

int32_t CFrameMessageConfig::LoadMessageConfig(TiXmlElement *pRoot)
{
	//获取msg节点
	TiXmlElement *pMsg = pRoot->FirstChildElement(ATTRIBUTE_MSG);
	if (NULL == pMsg)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Error, "msg node cannot be found in %s file!\n", m_szConfigFile);
		return E_LIBF_CONFIGNOKEY;
	}
	g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "Get msg node success!\n");

	const char* pszValue = NULL;

	while(pMsg != NULL)
	{
		//获取id
		pszValue = pMsg->Attribute(ATTRIBUTE_ID);
		if (NULL == pszValue)
		{
			g_FrameLogEngine.WriteBaseLog(enmLogLevel_Warning, "id cannot be found in %s file!\n", m_szConfigFile);
			return E_LIBF_CONFIGNOKEY;
		}
		int32_t nMsgID = 0;
		sscanf(pszValue, "%x", (int32_t *)&nMsgID);

		//获取source_type
		pszValue = pMsg->Attribute(ATTRIBUTE_SOURCETYPE);
		if (NULL == pszValue)
		{
			g_FrameLogEngine.WriteBaseLog(enmLogLevel_Warning, "source_type cannot be found in %s file!\n", m_szConfigFile);
			return E_LIBF_CONFIGNOKEY;
		}
		int32_t nSourceType = 0;
		sscanf(pszValue, "%x", (int32_t *)&nSourceType);

		//获取dest_type
		pszValue = pMsg->Attribute(ATTRIBUTE_DESTTYPE);
		if (NULL == pszValue)
		{
			g_FrameLogEngine.WriteBaseLog(enmLogLevel_Warning, "dest_type cannot be found in %s file!\n", m_szConfigFile);
			return E_LIBF_CONFIGNOKEY;
		}
		int32_t nDestType = 0;
		sscanf(pszValue, "%x", (int32_t *)&nDestType);

		//获取trans_type
		pszValue = pMsg->Attribute(ATTRIBUTE_TRANSTYPE);
		if (NULL == pszValue)
		{
			g_FrameLogEngine.WriteBaseLog(enmLogLevel_Warning, "trans_type cannot be found in %s file!\n", m_szConfigFile);
			return E_LIBF_CONFIGNOKEY;
		}

		TransType nTransType = enmTransType_None;
		if(strcmp(pszValue, "p2p") == 0)
		{
			nTransType = enmTransType_P2P;
		}
		else if(strcmp(pszValue, "p2g") == 0)
		{
			nTransType = enmTransType_P2G;
		}
		else if(strcmp(pszValue, "broadcast") == 0)
		{
			nTransType = enmTransType_Broadcast;
		}
		else if(strcmp(pszValue, "bykey") == 0)
		{
			nTransType = enmTransType_ByKey;
		}
		else if(strcmp(pszValue, "regist") == 0)
		{
			nTransType = enmTransType_Regist;
		}
		else if(strcmp(pszValue, "random") == 0)
		{
			nTransType = enmTransType_Ramdom;
		}
		else
		{
			g_FrameLogEngine.WriteBaseLog(enmLogLevel_Warning, "type is unknow!{type=%s}\n", pszValue);
			return E_LIBF_CONFIGNOKEY;
		}

		//获取type
		pszValue = pMsg->Attribute(ATTRIBUTE_TYPE);
		if (NULL == pszValue)
		{
			g_FrameLogEngine.WriteBaseLog(enmLogLevel_Warning, "type cannot be found in %s file!\n", m_szConfigFile);
			return E_LIBF_CONFIGNOKEY;
		}

		int32_t nTimeout = 0;
		int32_t nRespID = 0;
		int32_t nNotiID = 0;
		if(strcmp(pszValue, "req") == 0)
		{
			//获取id
			pszValue = pMsg->Attribute(ATTRIBUTE_TIMEOUT, &nTimeout);
			if (NULL == pszValue)
			{
				g_FrameLogEngine.WriteBaseLog(enmLogLevel_Warning, "timeout cannot be found in %s file!\n", m_szConfigFile);
				return E_LIBF_CONFIGNOKEY;
			}

			//获取resp
			pszValue = pMsg->Attribute(ATTRIBUTE_RESP);
			if (NULL == pszValue)
			{
				g_FrameLogEngine.WriteBaseLog(enmLogLevel_Warning, "resp cannot be found in %s file!\n", m_szConfigFile);
				return E_LIBF_CONFIGNOKEY;
			}
			sscanf(pszValue, "%x", (int32_t *)&nRespID);

			//获取noti
			pszValue = pMsg->Attribute(ATTRIBUTE_NOTI);
			if (NULL == pszValue)
			{
				g_FrameLogEngine.WriteBaseLog(enmLogLevel_Warning, "noti cannot be found in %s file!\n", m_szConfigFile);
				return E_LIBF_CONFIGNOKEY;
			}
			sscanf(pszValue, "%x", (int32_t *)&nNotiID);
		}
		else if(strcmp(pszValue, "resp") == 0)
		{

		}
		else if(strcmp(pszValue, "noti") == 0)
		{

		}
		else
		{
			g_FrameLogEngine.WriteBaseLog(enmLogLevel_Warning, "type is unknow!{type=%s}\n", pszValue);
			return E_LIBF_CONFIGNOKEY;
		}

		CMessageConfig stMessageConfig;
		stMessageConfig.nSourceType = nSourceType;
		stMessageConfig.nDestType = nDestType;
		stMessageConfig.nTransType = nTransType;
		stMessageConfig.nTimeoutValue = nTimeout;
		stMessageConfig.nRespMsgID = nRespID;
		stMessageConfig.nNotiMsgID = nNotiID;

		m_stMessageConfigMap[nMsgID] = stMessageConfig;

		pMsg = pMsg->NextSiblingElement();
	}

	return S_OK;
}

EntityType CFrameMessageConfig::GetSourceType(uint32_t nMsgID)
{
	if(m_stMessageConfigMap.count(nMsgID) > 0)
	{
		return m_stMessageConfigMap[nMsgID].nSourceType;
	}

	return 0;
}

EntityType CFrameMessageConfig::GetDestType(uint32_t nMsgID)
{
	if(m_stMessageConfigMap.count(nMsgID) > 0)
	{
		return m_stMessageConfigMap[nMsgID].nDestType;
	}

	return 0;
}

TransType CFrameMessageConfig::GetTransType(uint32_t nMsgID)
{
	if(m_stMessageConfigMap.count(nMsgID) > 0)
	{
		return m_stMessageConfigMap[nMsgID].nTransType;
	}

	return 0;
}

int64_t CFrameMessageConfig::GetTimeoutValue(uint32_t nMsgID)
{
	if(m_stMessageConfigMap.count(nMsgID) > 0)
	{
		return m_stMessageConfigMap[nMsgID].nTimeoutValue;
	}

	return 0;
}

uint32_t CFrameMessageConfig::GetRespMsgID(uint32_t nMsgID)
{
	if(m_stMessageConfigMap.count(nMsgID) > 0)
	{
		return m_stMessageConfigMap[nMsgID].nRespMsgID;
	}

	return 0;
}

uint32_t CFrameMessageConfig::GetNotiMsgID(uint32_t nMsgID)
{
	if(m_stMessageConfigMap.count(nMsgID) > 0)
	{
		return m_stMessageConfigMap[nMsgID].nNotiMsgID;
	}

	return 0;
}

FRAME_NAMESPACE_END

