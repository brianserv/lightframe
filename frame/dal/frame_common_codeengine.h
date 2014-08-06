/*
 * frame_common_codeengine.h
 *
 *  Created on: 2011-11-26
 *      Author: jimm
 */

#ifndef FRAME_COMMON_CODEENGINE_H_
#define FRAME_COMMON_CODEENGINE_H_

#include "common/common_typedef.h"
#include "common/common_codeengine.h"
#include "common/common_string.h"
#include "frame_namespace.h"
#include "frame_typedef.h"
#include "frame_protocolhead.h"

FRAME_NAMESPACE_BEGIN

class CFrameCodeEngine : public CCodeEngine
{
public:
	CFrameCodeEngine();
	virtual ~CFrameCodeEngine();

public:
	/* 数据打包，将编码结果放到缓冲区中
	*  buf      - 目的缓冲区
	*  size     - 目的缓冲区长度
	*  offset   - 输入打包数据放入到缓冲区的起始偏移量，输出为该偏移量与
	*             新打包的数据长度之和
	*  data     - 待打包的数据
	*  return   - 错误码
	*/

	static int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset, const ConnHead& data);
	static int32_t MessageDecode(const uint8_t* buf, const uint32_t size, uint32_t& offset, ConnHead& data);

	static int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset, const MessageHeadSS& data);
	static int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset, MessageHeadSS& data);

	static int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset, const ConnUin& data);
	static int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset, ConnUin& data);

	template<uint16_t SIZE>
	static int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset, const CString<SIZE>& data)
	{
		int32_t ret = S_OK;

		if(data.Length() > 0)
		{
			ret = Encode(buf, size, offset, data.GetString(), data.GetBufSize());
			if (0 > ret)
			{
				return ret;
			}
		}
		else
		{
			ret = Encode(buf, size, offset, data.Length());
			if (0 > ret)
			{
				return ret;
			}
		}

		return ret;
	}

	template<uint16_t SIZE>
	static int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset, CString<SIZE>& data)
	{
		int32_t ret = S_OK;

		char str[0xffff] = { 0 };

		ret = Decode(buf, size, offset, str, sizeof(str));
		if (0 > ret)
		{
			return ret;
		}

		data = str;

		return ret;
	}
};


FRAME_NAMESPACE_END

#endif /* FRAME_COMMON_CODEENGINE_H_ */
