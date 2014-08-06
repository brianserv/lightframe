/*
 * frame_mem_mgt.cpp
 *
 *  Created on: 2012-12-18
 *      Author: jimm
 */

#include "common/common_errordef.h"
#include "frame_mem_mgt.h"

FRAME_NAMESPACE_BEGIN

CFrameMemMgt::CFrameMemMgt()
{
	m_nMaxBlockSize = 0;
	m_nMemLeakCount = 0;
}

CFrameMemMgt::~CFrameMemMgt()
{

}

int32_t CFrameMemMgt::Initialize()
{
	int32_t nWantCount = 0;
	//��1K-1024K������0���ڴ��,��Ҫ��Ϊ�������ڴ������
	for(int32_t i = 1; i <= (10 * 1024); ++i)
	{
		//ע�⣬��ʼ���ڴ�һ��Ҫ��֤���̲߳�����
		int32_t nMallocCount = MallocMemBlock(i * 1024, nWantCount);
		if(nMallocCount < nWantCount)
		{
			return -1;
		}
	}

	return S_OK;
}

int32_t CFrameMemMgt::Resume()
{
	return S_OK;
}

int32_t CFrameMemMgt::Uinitialize()
{
	return S_OK;
}

uint8_t *CFrameMemMgt::AllocBlock(int32_t nWantSize)
{
	if(nWantSize <= 0)
	{
		return NULL;
	}

	int32_t nIndex = GetTableIndexByBytes(nWantSize);
	if(nIndex <= 0)
	{
		return NULL;
	}

	MemBlockInfo *pMemBlockInfo = GetMemBlockInfo(nIndex);
	if(pMemBlockInfo == NULL)
	{
		return NULL;
	}

	MUTEX_GUARD(MemBlockLock, pMemBlockInfo->m_stMemBlockLock);

	if(pMemBlockInfo->m_stMemBlockList.empty())
	{
		MallocMemBlock(pMemBlockInfo->m_nBlockSize, ALLOC_MEM_STEP);
	}

	if(pMemBlockInfo->m_stMemBlockList.empty())
	{
		return NULL;
	}

	uint8_t *pMem = NULL;
	MemBlockHead *pHead = NULL;
	while(!pMemBlockInfo->m_stMemBlockList.empty())
	{
		uint8_t *pMemTemp = pMemBlockInfo->m_stMemBlockList.front();
		pMemBlockInfo->m_stMemBlockList.pop_front();
		//�������ü���
		pHead = (MemBlockHead *)(pMemTemp - sizeof(MemBlockHead));
		//���ڴ��Ƿ�����ʹ��
		if(pHead->m_nReferCount > 0)
		{
			RecordMemLeakInfo(pMemTemp);
			continue;
		}
		pMem = pMemTemp;
		++pHead->m_nReferCount;
		break;
	}

	return pMem;
}

void CFrameMemMgt::RecycleBlock(uint8_t *pMemBlock)
{
	if(pMemBlock == NULL)
	{
		return;
	}

	MemBlockHead *pHead = (MemBlockHead *)(pMemBlock - sizeof(MemBlockHead));
	int32_t nIndex = pHead->m_nIndex;
	if(m_stMemInfoTable.count(nIndex) <= 0)
	{
		RecordMemLeakInfo(pMemBlock);
		return;
	}

	//���ڴ���С
	uint32_t nBlockSize = pHead->m_nBlockSize;
	if(nBlockSize == 0 || nBlockSize > m_nMaxBlockSize)
	{
		RecordMemLeakInfo(pMemBlock);
		return;
	}

//	int32_t nIndex = GetTableIndexByBytes(nBlockSize);
//	if(nIndex <= 0)
//	{
//		RecordMemLeakInfo(pMemBlock);
//		return;
//	}

	MemBlockInfo *pMemBlockInfo = GetMemBlockInfo(nIndex);
	if(pMemBlockInfo == NULL)
	{
		RecordMemLeakInfo(pMemBlock);
		return;
	}

	if(pMemBlockInfo->m_nBlockSize != nBlockSize)
	{
		RecordMemLeakInfo(pMemBlock);
		return;
	}

	//���ü�����һ
	if(pHead->m_nReferCount > 0)
	{
		--pHead->m_nReferCount;

		MUTEX_GUARD(MemBlockLock, pMemBlockInfo->m_stMemBlockLock);

		pMemBlockInfo->m_stMemBlockList.push_front(pMemBlock);
	}
	else
	{
		RecordMemLeakInfo(pMemBlock);
	}
}

//��¼�ڴ�й¶��Ϣ
void CFrameMemMgt::RecordMemLeakInfo(uint8_t *pMemBlock)
{
	if(pMemBlock != NULL)
	{
		++m_nMemLeakCount;
	}
}

//ͳ��Ŀǰ�����ڴ�������
void CFrameMemMgt::PrintMemBlockInfo()
{
	int32_t nOffset = 0;
	static char strMemBlockInfo[1024 * 1024];

	memset(strMemBlockInfo, 0, sizeof(strMemBlockInfo));

	sprintf(strMemBlockInfo + nOffset, "frame mem block info : {");
	nOffset = strlen(strMemBlockInfo);

	//���ڴ�����Ϣ
	for(MemInfoTable::iterator it = m_stMemInfoTable.begin(); it != m_stMemInfoTable.end(); it++)
	{
		MemBlockInfo *pMemBlockInfo = it->second;
		if(pMemBlockInfo == NULL)
		{
			continue;
		}

		if(pMemBlockInfo->m_nBlockCount <= ALLOC_MEM_STEP)
		{
			continue;
		}

		MUTEX_GUARD(MemBlockLock, pMemBlockInfo->m_stMemBlockLock);

		sprintf(strMemBlockInfo + nOffset, "{BlockSize(%d) : BlockCount(%d) : LeftCount(%d)}",
				pMemBlockInfo->m_nBlockSize, pMemBlockInfo->m_nBlockCount,
				(int32_t)pMemBlockInfo->m_stMemBlockList.size());
		nOffset = strlen(strMemBlockInfo);
	}

	sprintf(strMemBlockInfo + nOffset, ", m_nMemLeakCount = %d}", m_nMemLeakCount);
	nOffset = strlen(strMemBlockInfo);

	g_FrameLogEngine.WriteMainLog(enmLogLevel_Notice, "%s\n", strMemBlockInfo);

	//�ڴ������¼
	nOffset = 0;
	memset(strMemBlockInfo, 0, sizeof(strMemBlockInfo));

	sprintf(strMemBlockInfo + nOffset, "frame mem malloc record : {");
	nOffset = strlen(strMemBlockInfo);

	m_stAllocMemRecordLock.enter();

	for(MemRecordMap::iterator it = m_stAllocMemRecordMap.begin(); it != m_stAllocMemRecordMap.end(); it++)
	{
		LineNoMap *pLineNoMap = it->second;
		for(LineNoMap::iterator st = pLineNoMap->begin(); st != pLineNoMap->end(); st++)
		{
			MemOperationRecord *pRecord = st->second;
			if(pRecord == NULL)
			{
				continue;
			}

			sprintf(strMemBlockInfo + nOffset, "{FileName=%s, LineNo=%d, BlockSize=%u, MallocCount=%u}",
					it->first.c_str(), pRecord->m_nLineNo, pRecord->m_nBlockSize, pRecord->m_nOperationCount);
			nOffset = strlen(strMemBlockInfo);
		}
	}

	m_stAllocMemRecordLock.leave();

	sprintf(strMemBlockInfo + nOffset, "}");
	nOffset = strlen(strMemBlockInfo);

	g_FrameLogEngine.WriteMainLog(enmLogLevel_Notice, "%s\n", strMemBlockInfo);

	//�ڴ��ͷż�¼
	nOffset = 0;
	memset(strMemBlockInfo, 0, sizeof(strMemBlockInfo));

	sprintf(strMemBlockInfo + nOffset, "frame mem free record : {");
	nOffset = strlen(strMemBlockInfo);

	m_stRecycleMemRecordLock.enter();

	for(MemRecordMap::iterator it = m_stRecycleMemRecordMap.begin(); it != m_stRecycleMemRecordMap.end(); it++)
	{
		LineNoMap *pLineNoMap = it->second;
		for(LineNoMap::iterator st = pLineNoMap->begin(); st != pLineNoMap->end(); st++)
		{
			MemOperationRecord *pRecord = st->second;
			if(pRecord == NULL)
			{
				continue;
			}

			sprintf(strMemBlockInfo + nOffset, "{FileName=%s, LineNo=%d, BlockSize=%u, MallocCount=%u}",
					it->first.c_str(), pRecord->m_nLineNo, pRecord->m_nBlockSize, pRecord->m_nOperationCount);
			nOffset = strlen(strMemBlockInfo);
		}
	}

	m_stRecycleMemRecordLock.leave();

	sprintf(strMemBlockInfo + nOffset, "}");
	nOffset = strlen(strMemBlockInfo);

	g_FrameLogEngine.WriteMainLog(enmLogLevel_Notice, "%s\n", strMemBlockInfo);
}

int32_t CFrameMemMgt::GetBlockSize(int32_t nWantSize)
{
	int32_t nBlockSize = 0;
	if(nWantSize % 1024 == 0)
	{
		nBlockSize = nWantSize;
	}
	else
	{
		nBlockSize = (nWantSize + 1024 - (nWantSize % 1024));
	}

	return nBlockSize;
}

//ͳ��������Ϣ
void CFrameMemMgt::RecordAllocInfo(const char*pFileName, int32_t nLineNo, uint32_t nBlockSize)
{
	if(pFileName == NULL)
	{
		return;
	}

	MUTEX_GUARD(AllocMemRecordLock, m_stAllocMemRecordLock);

	//�Ѿ����ļ���¼
	if(m_stAllocMemRecordMap.count(string(pFileName)) > 0)
	{
		LineNoMap *pLineNoMap = m_stAllocMemRecordMap[string(pFileName)];
		LineNoMap::iterator st = pLineNoMap->find(nLineNo);
		if(st == pLineNoMap->end())
		{
			MemOperationRecord *pRecord = new MemOperationRecord();
			if(pRecord == NULL)
			{
				return;
			}

			pRecord->m_nLineNo = nLineNo;
			pRecord->m_nBlockSize = nBlockSize;
			pRecord->m_nOperationCount = 1;

			pLineNoMap->insert(make_pair(nLineNo, pRecord));
		}
		else
		{
			++st->second->m_nOperationCount;
		}
	}
	else
	{
		LineNoMap *pLineNoMap = new LineNoMap;
		if(pLineNoMap == NULL)
		{
			return;
		}

		MemOperationRecord *pRecord = new MemOperationRecord();
		if(pRecord == NULL)
		{
			delete pLineNoMap;
			return;
		}

		pRecord->m_nLineNo = nLineNo;
		pRecord->m_nBlockSize = nBlockSize;
		pRecord->m_nOperationCount = 1;

		pLineNoMap->insert(make_pair(nLineNo, pRecord));
		m_stAllocMemRecordMap[string(pFileName)] = pLineNoMap;
	}
}

//ͳ���ͷ���Ϣ
void CFrameMemMgt::RecordRecycleInfo(const char*pFileName, int32_t nLineNo, uint32_t nBlockSize)
{
	if(pFileName == NULL)
	{
		return;
	}

	MUTEX_GUARD(RecycleMemRecordLock, m_stRecycleMemRecordLock);

	//�Ѿ����ļ���¼
	if(m_stRecycleMemRecordMap.count(string(pFileName)) > 0)
	{
		LineNoMap *pLineNoMap = m_stRecycleMemRecordMap[string(pFileName)];
		LineNoMap::iterator st = pLineNoMap->find(nLineNo);
		if(st == pLineNoMap->end())
		{
			MemOperationRecord *pRecord = new MemOperationRecord();
			if(pRecord == NULL)
			{
				return;
			}

			pRecord->m_nLineNo = nLineNo;
			pRecord->m_nBlockSize = nBlockSize;
			pRecord->m_nOperationCount = 1;

			pLineNoMap->insert(make_pair(nLineNo, pRecord));
		}
		else
		{
			++st->second->m_nOperationCount;
		}
	}
	else
	{
		LineNoMap *pLineNoMap = new LineNoMap;
		if(pLineNoMap == NULL)
		{
			return;
		}

		MemOperationRecord *pRecord = new MemOperationRecord();
		if(pRecord == NULL)
		{
			delete pLineNoMap;
			return;
		}

		pRecord->m_nLineNo = nLineNo;
		pRecord->m_nBlockSize = nBlockSize;
		pRecord->m_nOperationCount = 1;

		pLineNoMap->insert(make_pair(nLineNo, pRecord));
		m_stRecycleMemRecordMap[string(pFileName)] = pLineNoMap;
	}
}

int32_t CFrameMemMgt::MallocMemBlock(int32_t nBytes, int32_t nWantCount)
{
	int32_t nIndex = GetTableIndexByBytes(nBytes);
	if(nIndex <= 0)
	{
		return 0;
	}

	MemBlockInfo *pMemBlockInfo = GetMemBlockInfo(nIndex);
	if(pMemBlockInfo == NULL)
	{
		pMemBlockInfo = CreateMemBlockInfo(nBytes);
		if(pMemBlockInfo == NULL)
		{
			return 0;
		}
	}

//	MUTEX_GUARD(MemBlockLock, pMemBlockInfo->m_stMemBlockLock);

	int32_t nMallocCount = 0;
	for(int32_t i = 0; i < nWantCount; ++i)
	{
		//ǰ��8�ֽڴ���ڴ����Ϣ
		uint8_t *pMem = new uint8_t[pMemBlockInfo->m_nBlockSize + sizeof(MemBlockHead)];
		if(pMem == NULL)
		{
			break;
		}

		MemBlockHead *pHead = new(pMem)MemBlockHead();
		pHead->m_nIndex = nIndex;
		pHead->m_nBlockSize = pMemBlockInfo->m_nBlockSize;
		pHead->m_nReferCount = 0;
		pMemBlockInfo->m_stMemBlockList.push_back(&pMem[sizeof(MemBlockHead)]);
		++pMemBlockInfo->m_nBlockCount;

		++nMallocCount;
	}

	return nMallocCount;
}

int32_t CFrameMemMgt::GetTableIndexByBytes(int32_t nBytes)
{
	int32_t nKbIndex = 0;
	int32_t nIndex = nBytes / 1024;
	if(nIndex < 0)
	{
		nKbIndex = -1;
	}
	else
	{
		if(nBytes % 1024 > 0)
		{
			nKbIndex = nIndex + 1;
		}
		else
		{
			nKbIndex = nIndex;
		}
	}

	return nKbIndex;
}


MemBlockInfo *CFrameMemMgt::CreateMemBlockInfo(int32_t nBytes)
{
	int32_t nIndex = GetTableIndexByBytes(nBytes);
	if(nIndex <= 0)
	{
		return NULL;
	}

	MemBlockInfo *pMemBlockInfo = new MemBlockInfo();
	if(pMemBlockInfo == NULL)
	{
		return NULL;
	}

	m_stMemInfoTable[nIndex] = pMemBlockInfo;
	pMemBlockInfo->m_nBlockSize = nBytes;

	//��������ڴ���С
	if((uint32_t)nBytes > m_nMaxBlockSize)
	{
		m_nMaxBlockSize = (uint32_t)nBytes;
	}

	return pMemBlockInfo;
}

MemBlockInfo *CFrameMemMgt::GetMemBlockInfo(int32_t nIndex)
{
	if(m_stMemInfoTable.count(nIndex) <= 0)
	{
		return NULL;
	}

	return m_stMemInfoTable[nIndex];
}

FRAME_NAMESPACE_END

