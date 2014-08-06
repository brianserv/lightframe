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
	/* ���ݴ�������������ŵ���������
	*  buf      - Ŀ�Ļ�����
	*  size     - Ŀ�Ļ���������
	*  offset   - ���������ݷ��뵽����������ʼƫ���������Ϊ��ƫ������
	*             �´�������ݳ���֮��
	*  data     - �����������
	*  return   - ������
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
