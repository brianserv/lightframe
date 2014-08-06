/*
 * frame_crypt.cpp
 *
 *  Created on: 2012-12-10
 *      Author: jimm
 */

#include "common/common_codeengine.h"
#include "common/common_message.h"
#include "frame_typedef.h"
#include "frame_crypt.h"

FRAME_NAMESPACE_BEGIN

//static char g_arrSSKey[auth_service_key_length + 1] = {'A', 'u', 't', 'h', 'e', 'n', 't', 'i', 'c', '@', '5','1','.', 'C', 'O', 'M', '\0'};
static char g_arrTokeKey[auth_service_key_length + 1] = {'9', 'X', 'i', 'u', 'M', 'i', 'M', 'a', 'J', 'i', 'a', 'Q', 'i', 'a', 'n', 'g', '\0'};

CFrameCrypt::CFrameCrypt()
{

}

CFrameCrypt::~CFrameCrypt()
{

}

int32_t CFrameCrypt::Encrypt(MessageHeadCS *pMsgHead, uint8_t* pBody, uint32_t &nBodySize,
		CFrameConnection *pConnection)
{
	if((pBody == NULL) || (nBodySize < 0) || (pConnection == NULL))
	{
		return E_UNKNOWN;
	}

	uint8_t arrEncryptBody[enmMaxMessageSize];
	int32_t nEncryptSize = sizeof(arrEncryptBody);
	int32_t nOutSize = CXTEA::Encrypt((char *)pBody, nBodySize,
			(char *)arrEncryptBody, nEncryptSize, (char *)pConnection->m_stPlayerSignature.m_arrKey);
	if(nOutSize <= 0)
	{
		return E_UNKNOWN;
	}

	memcpy(pBody, arrEncryptBody, nOutSize);
	nBodySize = nOutSize;

	return nOutSize;
}

int32_t CFrameCrypt::Decrypt(MessageHeadCS *pMsgHead, uint8_t* pBody, uint32_t &nBodySize,
		CFrameConnection *pConnection)
{
	int32_t nRet = S_OK;

	if((pBody == NULL) || (nBodySize < 0) || (pConnection == NULL))
	{
		return E_UNKNOWN;
	}

	uint32_t nOffset = 0;
	uint16_t nTokenSize = 0;
	uint8_t nAccountType = 0;

	//½â³ötoken
	uint8_t arrDecryptBuf[max_signature_buffer_length];
	int32_t nDecryptBufLen = sizeof(arrDecryptBuf);
	uint32_t nDecryptLen = 0;

	if(!pConnection->m_bIsInit)
	{
		nRet = CCodeEngine::Decode(pBody, nBodySize, nOffset, nAccountType);
		if(nRet < 0)
		{
			return nRet;
		}

		nRet = CCodeEngine::Decode(pBody, nBodySize, nOffset, nTokenSize);
		if(nRet < 0)
		{
			return nRet;
		}

		//½âÃÜtoken
		nDecryptLen = CXTEA::Decrypt((char *)(pBody + nOffset), nTokenSize,
				(char *)arrDecryptBuf, nDecryptBufLen, g_arrTokeKey);
		if(nDecryptLen <= 0)
		{
			return E_UNKNOWN;
		}

		uint32_t nDecryptOffset = 0;
		memcpy(pConnection->m_stPlayerSignature.m_arrKey, arrDecryptBuf + nDecryptOffset, crypt_key_length);
		nDecryptOffset += crypt_key_length;

		int32_t nRoleID = enmInvalidRoleID;
		nRet = CCodeEngine::Decode((uint8_t *)arrDecryptBuf, nDecryptLen, nDecryptOffset, nRoleID);
		if(nRet < 0)
		{
			return nRet;
		}
		pConnection->m_stPlayerSignature.m_nRoleID = nRoleID;

//		int32_t nUid = 0;
//		nRet = CCodeEngine::Decode(arrDecryptBuf, nDecryptLen, nDecryptOffset, nUid);
//		if(nRet < 0)
//		{
//			return nRet;
//		}
//		pConnection->m_stPlayerSignature.m_nUid = nUid;
//
//		int8_t nEmailLen = 0;
//		nRet = CCodeEngine::Decode(arrDecryptBuf, nDecryptLen, nDecryptOffset, nEmailLen);
//		if(nRet < 0)
//		{
//			return nRet;
//		}

//		pConnection->m_stPlayerSignature.m_nEmailLen = nEmailLen;
//		if(nEmailLen > 0)
//		{
////			nRet = CCodeEngine::Decode(arrDecryptBuf, nDecryptLen, nDecryptOffset,
////					pConnection->m_stPlayerSignature.m_strEmail, max_email_addr_length);
////			if(nRet < 0)
////			{
////				return nRet;
////			}
//			strncpy(pConnection->m_stPlayerSignature.m_strEmail, (char *)arrDecryptBuf + nDecryptOffset, nEmailLen);
//			nDecryptOffset += nEmailLen;
//		}

//		nRet = CCodeEngine::Decode(arrDecryptBuf, nDecryptLen, nDecryptOffset, pConnection->m_stPlayerSignature.m_arrServerBitMap,
//				max_server_bitmap_length, max_server_bitmap_length);
//		if(nRet < 0)
//		{
//			return nRet;
//		}
		memcpy(pConnection->m_stPlayerSignature.m_arrServerBitMap, &arrDecryptBuf[nDecryptOffset], sizeof(pConnection->m_stPlayerSignature.m_arrServerBitMap));
		nDecryptOffset += sizeof(pConnection->m_stPlayerSignature.m_arrServerBitMap);

		nRet = CCodeEngine::Decode(arrDecryptBuf, nDecryptLen, nDecryptOffset, pConnection->m_stPlayerSignature.m_nTimeStamp);
		if(nRet < 0)
		{
			return nRet;
		}

		nOffset += nTokenSize;
		pConnection->m_stConnInfo.nAccountType = nAccountType;
		pConnection->m_bIsInit = true;
	}

	uint8_t arrDecryptBody[enmMaxMessageSize];
	int32_t nDecryptSize = sizeof(arrDecryptBody);

	int32_t nOutSize = CXTEA::Decrypt((char *)(pBody + nOffset), nBodySize - nOffset,
			(char *)arrDecryptBody, nDecryptSize, (char *)pConnection->m_stPlayerSignature.m_arrKey);
	if(nOutSize <= 0)
	{
		return E_UNKNOWN;
	}

	int32_t nBodyOffset = 0;
	if(nTokenSize > 0)
	{
		nBodyOffset = sizeof(nAccountType)+ sizeof(nTokenSize);
		memcpy(pBody + nBodyOffset, arrDecryptBuf, nTokenSize);
		nBodyOffset += nTokenSize;
	}

	memcpy(pBody + nBodyOffset, arrDecryptBody, nOutSize);
	nBodyOffset += nOutSize;

	nBodySize = nBodyOffset;

	return nRet;
}


FRAME_NAMESPACE_END
