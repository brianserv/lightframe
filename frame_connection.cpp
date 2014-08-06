/*
 * frame_connection.cpp
 *
 *  Created on: 2012-12-11
 *      Author: jimm
 */

#include "frame_connection.h"

FRAME_NAMESPACE_BEGIN

CFrameConnection::CFrameConnection()
{
	m_bIsInit = false;
}

CFrameConnection::~CFrameConnection()
{

}

int32_t CFrameConnection::Initialize()
{
	m_bIsInit = false;
	return S_OK;
}

int32_t CFrameConnection::Resume()
{
	m_bIsInit = false;
	return S_OK;
}

int32_t CFrameConnection::Uninitialize()
{
	m_bIsInit = false;
	return S_OK;
}

int32_t CFrameConnection::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t ret = S_OK;

	return ret;
}

int32_t CFrameConnection::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t ret = S_OK;

	return ret;
}

FRAME_NAMESPACE_END
