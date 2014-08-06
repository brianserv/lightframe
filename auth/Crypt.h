/********************************************************************
created:	2008/02/18
created:	18:2:2008   9:52
filename: 	crypt.h
author:		veazhang

purpose:	
*********************************************************************/

#pragma once

#ifndef _MSC_VER
#include <stddef.h>
#endif

#include "common/common_typedef.h"
//#include "auth/common_base.h"

#define XTEA_KEY_LENGTH 16
#define MD5_HASH_LENGTH 16

class MD5Hash
{



public:
	typedef struct tagMD5HashValue
	{
		char arrby[MD5_HASH_LENGTH];
	}MD5HashValue;

};

class CXTEA
{
public:
	// 32�ֵ�XTEA�ӽ���
	// ����ֵ�ǲ������Buffer��С��ʧ���򷵻�0
	static size_t Encrypt(char* pbyInBuffer, size_t nInBufferLength, char* pbyOutBuffer, size_t nOutBufferLength, char arrbyKey[16]);
	static size_t Decrypt(char* pbyInBuffer, size_t nInBufferLength, char* pbyOutBuffer, size_t nOutBufferLength, char arrbyKey[16]);
	
	// 2009-3-1. Balon Fan.
	// ����֧��ָ���������ܵ�XTEA�㷨
	// ����ֵ�ǲ������Buffer��С��ʧ���򷵻�0
	static size_t EncryptRoundN(char* pbyInBuffer, size_t nInBufferLength, char* pbyOutBuffer, size_t nOutBufferLength, char arrbyKey[16], int32_t nRound);
	static size_t DecryptRoundN(char* pbyInBuffer, size_t nInBufferLength, char* pbyOutBuffer, size_t nOutBufferLength, char arrbyKey[16], int32_t nRound);

	// ��ȡ�㹻��ļӡ��������Buffer�ĳ���
	static size_t GetSafeEncryptOutBufferLength(size_t nInBufferLength);
	static size_t GetSafeDecryptOutBufferLength(size_t nInBufferLength);




public:
	typedef struct tagXTEAKey
	{
		char arrby[XTEA_KEY_LENGTH];
	}XTEAKey;

	enum{
		MAX_ENCRYPT_PADDING_LENGTH = 16,
	};
};
