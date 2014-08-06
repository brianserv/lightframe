/*
 * frame_crypt.h
 *
 *  Created on: 2012-12-10
 *      Author: jimm
 */

#ifndef FRAME_CRYPT_H_
#define FRAME_CRYPT_H_

#include "common/common_crypt.h"
#include "common/common_string.h"
#include "frame_namespace.h"
#include "frame_connection.h"
#include "frame_protocolhead.h"

FRAME_NAMESPACE_BEGIN

enum
{
	max_signature_buffer_length		= 2048,		//玩家签名buffer的最大长度
};

class CFrameCrypt
{
public:
	CFrameCrypt();
	~CFrameCrypt();

	static int32_t Encrypt(MessageHeadCS *pMsgHead, uint8_t* pBody, uint32_t &nBodySize,
			CFrameConnection *pConnection);

	static int32_t Decrypt(MessageHeadCS *pMsgHead, uint8_t* pBody, uint32_t &nBodySize,
			CFrameConnection *pConnection);

private:
	static CXTEA				m_stXTEA;
};

FRAME_NAMESPACE_END

#endif /* FRAME_CRYPT_H_ */
