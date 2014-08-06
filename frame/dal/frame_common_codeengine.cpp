/*
 * frame_common_codeengine.cpp
 *
 *  Created on: 2011-11-26
 *      Author: jimm
 */

#include "frame/dal/frame_common_codeengine.h"
#include "frame_errordef.h"

FRAME_NAMESPACE_BEGIN


int32_t CFrameCodeEngine::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset, const ConnHead& data)
{
	int32_t ret = S_OK;

	ret = Encode(buf, size, offset, data.roleID);
	if (0 > ret)
	{
		return ret;
	}
	ret = Encode(buf, size, offset, data.connIndex);
	if (0 > ret)
	{
		return ret;
	}
	ret = Encode(buf, size, offset, data.controlInfo.nControlType);
	if (0 > ret)
	{
		return ret;
	}
	ret = Encode(buf, size, offset, data.controlInfo.nControlParam);
	if (0 > ret)
	{
		return ret;
	}
	ret = Encode(buf, size, offset, data.clientInfo.nClientAddress);
	if (0 > ret)
	{
		return ret;
	}
	ret = Encode(buf, size, offset, data.clientInfo.nClientPort);
	if (0 > ret)
	{
		return ret;
	}

	return S_OK;
}

int32_t CFrameCodeEngine::MessageDecode(const uint8_t* buf, const uint32_t size, uint32_t& offset, ConnHead& data)
{
	int32_t ret = S_OK;

	ret = Decode(buf, size, offset, data.roleID);
	if (0 > ret)
	{
		return ret;
	}
	ret = Decode(buf, size, offset, data.connIndex);
	if (0 > ret)
	{
		return ret;
	}
	ret = Decode(buf, size, offset, data.controlInfo.nControlType);
	if (0 > ret)
	{
		return ret;
	}
	ret = Decode(buf, size, offset, data.controlInfo.nControlParam);
	if (0 > ret)
	{
		return ret;
	}
	ret = Decode(buf, size, offset, data.clientInfo.nClientAddress);
	if (0 > ret)
	{
		return ret;
	}
	ret = Decode(buf, size, offset, data.clientInfo.nClientPort);
	if (0 > ret)
	{
		return ret;
	}

	return S_OK;
}


int32_t CFrameCodeEngine::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset, const MessageHeadSS& data)
{
	int32_t ret = S_OK;

	ret = Encode(buf, size, offset, data.nTotalSize);
	if (0 > ret)
	{
		return ret;
	}
	ret = Encode(buf, size, offset, data.nMessageID);
	if (0 > ret)
	{
		return ret;
	}
	ret = Encode(buf, size, offset, data.nHeadSize);
	if (0 > ret)
	{
		return ret;
	}
	ret = Encode(buf, size, offset, data.nMessageType);
	if (0 > ret)
	{
		return ret;
	}
	ret = Encode(buf, size, offset, data.nSourceType);
	if (0 > ret)
	{
		return ret;
	}
	ret = Encode(buf, size, offset, data.nDestType);
	if (0 > ret)
	{
		return ret;
	}
	ret = Encode(buf, size, offset, data.nSourceID);
	if (0 > ret)
	{
		return ret;
	}
	ret = Encode(buf, size, offset, data.nDestID);
	if (0 > ret)
	{
		return ret;
	}
	ret = Encode(buf, size, offset, data.nRoleID);
	if (0 > ret)
	{
		return ret;
	}
	ret = Encode(buf, size, offset, data.nSequence);
	if (0 > ret)
	{
		return ret;
	}
	ret = Encode(buf, size, offset, data.nTransType);
	if (0 > ret)
	{
		return ret;
	}
	ret = Encode(buf, size, offset, data.nSessionIndex);
	if (0 > ret)
	{
		return ret;
	}
	ret = Encode(buf, size, offset, data.nRoomID);
	if (0 > ret)
	{
		return ret;
	}
	ret = Encode(buf, size, offset, data.nZoneID);
	if (0 > ret)
	{
		return ret;
	}
	ret = Encode(buf, size, offset, data.nRouterIndex);
	if (0 > ret)
	{
		return ret;
	}

	return S_OK;
}

int32_t CFrameCodeEngine::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset, MessageHeadSS& data)
{
	int32_t ret = S_OK;

	ret = Decode(buf, size, offset, data.nTotalSize);
	if (0 > ret)
	{
		return ret;
	}
	ret = Decode(buf, size, offset, data.nMessageID);
	if (0 > ret)
	{
		return ret;
	}
	ret = Decode(buf, size, offset, data.nHeadSize);
	if (0 > ret)
	{
		return ret;
	}
	ret = Decode(buf, size, offset, data.nMessageType);
	if (0 > ret)
	{
		return ret;
	}
	ret = Decode(buf, size, offset, data.nSourceType);
	if (0 > ret)
	{
		return ret;
	}
	ret = Decode(buf, size, offset, data.nDestType);
	if (0 > ret)
	{
		return ret;
	}
	ret = Decode(buf, size, offset, data.nSourceID);
	if (0 > ret)
	{
		return ret;
	}
	ret = Decode(buf, size, offset, data.nDestID);
	if (0 > ret)
	{
		return ret;
	}
	ret = Decode(buf, size, offset, data.nRoleID);
	if (0 > ret)
	{
		return ret;
	}
	ret = Decode(buf, size, offset, data.nSequence);
	if (0 > ret)
	{
		return ret;
	}
	ret = Decode(buf, size, offset, data.nTransType);
	if (0 > ret)
	{
		return ret;
	}
	ret = Decode(buf, size, offset, data.nSessionIndex);
	if (0 > ret)
	{
		return ret;
	}
	ret = Decode(buf, size, offset, data.nRoomID);
	if (0 > ret)
	{
		return ret;
	}
	ret = Decode(buf, size, offset, data.nZoneID);
	if (0 > ret)
	{
		return ret;
	}
	ret = Decode(buf, size, offset, data.nRouterIndex);
	if (0 > ret)
	{
		return ret;
	}

	return S_OK;
}

int32_t CFrameCodeEngine::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset, const ConnUin& data)
{
	int32_t ret = S_OK;

	ret = Encode(buf, size, offset, data.nVersion);
	if (0 > ret)
	{
		return ret;
	}
	ret = Encode(buf, size, offset, data.nServerID);
	if (0 > ret)
	{
		return ret;
	}
	ret = Encode(buf, size, offset, data.nTunnelIndex);
	if (0 > ret)
	{
		return ret;
	}
	ret = Encode(buf, size, offset, data.nRoleID);
	if (0 > ret)
	{
		return ret;
	}
	ret = Encode(buf, size, offset, data.nCreateTime);
	if (0 > ret)
	{
		return ret;
	}
	ret = Encode(buf, size, offset, data.nIsn);
	if (0 > ret)
	{
		return ret;
	}
	ret = Encode(buf, size, offset, data.nAddress);
	if (0 > ret)
	{
		return ret;
	}
	ret = Encode(buf, size, offset, data.nPort);
	if (0 > ret)
	{
		return ret;
	}
	ret = Encode(buf, size, offset, data.nNetType);
	if (0 > ret)
	{
		return ret;
	}
	ret = Encode(buf, size, offset, data.nAccountType);
	if (0 > ret)
	{
		return ret;
	}
	ret = Encode(buf, size, offset, data.nRouterAddress);
	if (0 > ret)
	{
		return ret;
	}
	ret = Encode(buf, size, offset, data.nRouterPort);
	if (0 > ret)
	{
		return ret;
	}

	return ret;
}

int32_t CFrameCodeEngine::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset, ConnUin& data)
{
	int32_t ret = S_OK;

	ret = Decode(buf, size, offset, data.nVersion);
	if (0 > ret)
	{
		return ret;
	}
	ret = Decode(buf, size, offset, data.nServerID);
	if (0 > ret)
	{
		return ret;
	}
	ret = Decode(buf, size, offset, data.nTunnelIndex);
	if (0 > ret)
	{
		return ret;
	}
	ret = Decode(buf, size, offset, data.nRoleID);
	if (0 > ret)
	{
		return ret;
	}
	ret = Decode(buf, size, offset, data.nCreateTime);
	if (0 > ret)
	{
		return ret;
	}
	ret = Decode(buf, size, offset, data.nIsn);
	if (0 > ret)
	{
		return ret;
	}
	ret = Decode(buf, size, offset, data.nAddress);
	if (0 > ret)
	{
		return ret;
	}
	ret = Decode(buf, size, offset, data.nPort);
	if (0 > ret)
	{
		return ret;
	}
	ret = Decode(buf, size, offset, data.nNetType);
	if (0 > ret)
	{
		return ret;
	}
	ret = Decode(buf, size, offset, data.nAccountType);
	if (0 > ret)
	{
		return ret;
	}
	ret = Decode(buf, size, offset, data.nRouterAddress);
	if (0 > ret)
	{
		return ret;
	}
	ret = Decode(buf, size, offset, data.nRouterPort);
	if (0 > ret)
	{
		return ret;
	}

	return ret;
}

FRAME_NAMESPACE_END

