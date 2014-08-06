/*
 * frame_connection.h
 *
 *  Created on: 2012-12-11
 *      Author: jimm
 */

#ifndef FRAME_CONNECTION_H_
#define FRAME_CONNECTION_H_

#include "common/common_errordef.h"
#include "common/common_complextype.h"
#include "frame_protocolhead.h"
#include "frame_typedef.h"
#include "frame_namespace.h"

FRAME_NAMESPACE_BEGIN

//��ǩ����֤�����Ϣ
enum
{
	auth_service_key_length			= 16,	//����֤������Լ������Կ����
	crypt_key_length				= 16,	//��ͻ��˵�ͨѶʹ�õļ�����Կ����

	max_signature_valid_period		= 43200,//(12*60*60) 12Сʱ
	min_signature_valid_period		= 3600,	//60*60	1Сʱ

	max_encrypt_key_length = 16,			/**< ������Կ�ĳ���*/
	max_email_addr_length  = 32,           //ǩ���е�����ĳ���
	max_server_bitmap_length = 16,         //ǩ���е�serverbitmap�ĳ���
};

class PlayerSignature
{
public:
	PlayerSignature()
	{
		m_nSize = 0;
		memset(m_arrKey, 0, sizeof(m_arrKey));
		m_nRoleID = enmInvalidRoleID;
		m_nUid = 0;
		m_nEmailLen = 0;
		memset(m_strEmail, 0, sizeof(m_strEmail));
		memset(m_arrServerBitMap, 0, sizeof(m_arrServerBitMap));
		m_nTimeStamp = 0;
	}
public:
	int16_t		m_nSize;				//��ʱû����
	uint8_t		m_arrKey[max_encrypt_key_length];
	RoleID		m_nRoleID;
	int32_t		m_nUid;
	int8_t		m_nEmailLen;
	char		m_strEmail[max_email_addr_length];
	uint8_t		m_arrServerBitMap[max_server_bitmap_length];
	int32_t		m_nTimeStamp;
};

class CFrameConnection : public CObject
{
public:
	CFrameConnection();
	~CFrameConnection();

	virtual int32_t Initialize();

	virtual int32_t Resume();

	virtual int32_t Uninitialize();

	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);

public:
	bool			m_bIsInit;
	PlayerSignature	m_stPlayerSignature;
	ConnInfo		m_stConnInfo;
};


FRAME_NAMESPACE_END

CCOMPLEXTYPE_SPECIFICALLY(FRAME_NAMESPACE::CFrameConnection)

#endif /* FRAME_CONNECTION_H_ */
