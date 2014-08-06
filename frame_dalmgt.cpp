/*
 *  frame_dal.cpp
 *
 *  To do£º
 *  Created on: 	2012-12-17
 *  Author: 		luocj
 */

#include "frame_errordef.h"
#include "frame_configmgt.h"
#include "frame_dalmgt.h"

FRAME_NAMESPACE_BEGIN

CFrameDalMgt::CFrameDalMgt()
{
	m_nDataCenterCount = 0;
	memset(m_arrDataCenter, 0, sizeof(m_arrDataCenter));
}

CFrameDalMgt::~CFrameDalMgt()
{

}

int32_t CFrameDalMgt::Initialize()
{
	for(int32_t i = 0; i < m_nDataCenterCount; i++)
	{
		IDataCenter *pDataCenter = m_arrDataCenter[i];

		int32_t nRet = pDataCenter->Initialize();
		if(0 > nRet)
		{
			return nRet;
		}
	}
	return S_OK;
}

int32_t CFrameDalMgt::Resume()
{
	for(int32_t i = 0; i < m_nDataCenterCount; i++)
	{
		IDataCenter *pDataCenter = m_arrDataCenter[i];

		int32_t nRet = pDataCenter->Resume();
		if(0 > nRet)
		{
			return nRet;
		}
	}

	return S_OK;
}

int32_t CFrameDalMgt::Uninitialize()
{
	for(int32_t i = 0; i < m_nDataCenterCount; i++)
	{
		IDataCenter *pDataCenter = m_arrDataCenter[i];
		pDataCenter->Uninitialize();
	}

	return S_OK;
}


void CFrameDalMgt::AddDataCenter(IDataCenter* pDataCenter)
{
	if(NULL != pDataCenter)
	{
		m_arrDataCenter[m_nDataCenterCount++] = pDataCenter;
	}
}

IDataCenter* CFrameDalMgt::GetDataCenter(const char *szName)
{
	for(int32_t i = 0; i < m_nDataCenterCount; i++)
	{
		if(strcmp(szName, m_arrDataCenter[i]->GetName()) == 0)
		{
			return m_arrDataCenter[i];
		}
	}

	return NULL;
}

FRAME_NAMESPACE_END
