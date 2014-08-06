/*
 * frame_message_config.h
 *
 *  Created on: 2012-12-19
 *      Author: jimm
 */

#ifndef FRAME_MESSAGE_CONFIG_H_
#define FRAME_MESSAGE_CONFIG_H_

#include "tinyxml/tinyxml.h"
#include "common/common_singleton.h"
#include "common/common_object.h"
#include "common/common_def.h"
#include "frame_namespace.h"
#include "frame_errordef.h"
#include "frame_logengine.h"
#include "frame_def.h"

#include <map>

using namespace std;

FRAME_NAMESPACE_BEGIN

//最多的消息数量
#define MAX_MESSAGE_COUNT		1024


class CMessageConfig : public CObject
{
public:
	CMessageConfig()
	{
		nSourceType = 0;
		nDestType = 0;
		nTransType = 0;
		nTimeoutValue = 0;
		nRespMsgID = 0;
		nNotiMsgID = 0;
	}

	EntityType		nSourceType;
	EntityType		nDestType;
	TransType		nTransType;
	int64_t			nTimeoutValue;
	uint32_t		nRespMsgID;
	uint32_t		nNotiMsgID;
};

class CFrameMessageConfig : public IConfigCenter
{
public:
	CFrameMessageConfig();

	virtual ~CFrameMessageConfig();

	virtual int32_t Initialize(const char* szFileName = NULL, const int32_t type=0);

	virtual int32_t Reload(const char* szFileName = NULL, const int32_t type=0);

	virtual int32_t Uninitialize();

	EntityType GetSourceType(uint32_t nMsgID);

	EntityType GetDestType(uint32_t nMsgID);

	TransType GetTransType(uint32_t nMsgID);

	int64_t GetTimeoutValue(uint32_t nMsgID);

	uint32_t GetRespMsgID(uint32_t nMsgID);

	uint32_t GetNotiMsgID(uint32_t nMsgID);

	virtual int32_t LoadMessageConfig(TiXmlElement *pRoot);

protected:
	char				m_szConfigFile[enmMaxFileNameLength];

	map<uint32_t, CMessageConfig> m_stMessageConfigMap;
};

FRAME_NAMESPACE_END


#endif /* FRAME_MESSAGE_CONFIG_H_ */
