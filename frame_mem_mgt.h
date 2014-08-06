/*
 * frame_mem_mgt.h
 *
 *  Created on: 2012-12-18
 *      Author: jimm
 */

#ifndef FRAME_MEM_MGT_H_
#define FRAME_MEM_MGT_H_

#include "common/common_object.h"
#include "common/common_singleton.h"
#include "common/common_mutex.h"
#include "frame_namespace.h"
#include "frame_logengine.h"

#include <map>
#include <list>
#include <string>

using namespace std;

FRAME_NAMESPACE_BEGIN

//ÿ�������ڴ�ĸ���
#define ALLOC_MEM_STEP				50

class MemBlockHead
{
public:
	MemBlockHead()
	{
		m_nIndex = -1;
		m_nBlockSize = 0;
		m_nReferCount = 0;
	}

	int32_t			m_nIndex;				//���ڴ���е�����λ��
	uint32_t		m_nBlockSize;			//�ڴ��ʵ�ʴ�С
	uint16_t		m_nReferCount;			//���ڶ��ٵط�����
};

class MemBlockInfo
{
public:
	MemBlockInfo()
	{
		m_nBlockSize = 0;
		m_nBlockCount = 0;
	}

	CriticalSection	m_stMemBlockLock;
	uint32_t		m_nBlockSize;			//���ڴ���С
	int32_t			m_nBlockCount;			//�ڴ��ĸ���
	list<uint8_t *>	m_stMemBlockList;		//�ڴ��ָ������
};

typedef map<int32_t, MemBlockInfo *> MemInfoTable;

class MemOperationRecord
{
public:
	int32_t			m_nLineNo;
	uint32_t		m_nBlockSize;
	uint32_t		m_nOperationCount;
};

typedef map<int32_t, MemOperationRecord *>			LineNoMap;
typedef map<string, LineNoMap *>  					MemRecordMap;

class CFrameMemMgt : public CObject
{
public:

	CFrameMemMgt();
	virtual ~CFrameMemMgt();

	//ע�⣬��ʼ���ڴ�һ��Ҫ��֤���̲߳�����
	int32_t Initialize();
	int32_t Resume();
	int32_t Uinitialize();

	//�����ڴ��
	uint8_t *AllocBlock(int32_t nWantSize);
	//�����ڴ��
	void RecycleBlock(uint8_t *pMemBlock);
	//��¼�ڴ�й¶��Ϣ
	void RecordMemLeakInfo(uint8_t *pMemBlock);
	//ͳ��Ŀǰ�����ڴ�������
	void PrintMemBlockInfo();

	int32_t GetBlockSize(int32_t nWantSize);
	//ͳ��������Ϣ
	void RecordAllocInfo(const char*pFileName, int32_t nLineNo, uint32_t nBlockSize);
	//ͳ���ͷ���Ϣ
	void RecordRecycleInfo(const char*pFileName, int32_t nLineNo, uint32_t nBlockSize);

protected:
	int32_t MallocMemBlock(int32_t nBytes, int32_t nWantCount);

	MemBlockInfo *CreateMemBlockInfo(int32_t nBytes);

	MemBlockInfo *GetMemBlockInfo(int32_t nIndex);

	int32_t GetTableIndexByBytes(int32_t nDataSize);

protected:
	//�����ڴ���С
	uint32_t			m_nMaxBlockSize;

	MemInfoTable		m_stMemInfoTable;
	//�ڴ��й¶�ĸ���
	uint32_t			m_nMemLeakCount;

	CriticalSection		m_stAllocMemRecordLock;
	MemRecordMap		m_stAllocMemRecordMap;

	CriticalSection		m_stRecycleMemRecordLock;
	MemRecordMap		m_stRecycleMemRecordMap;
};

#define	CREATE_FRAMEMEMMGT_INSTANCE				CSingleton<CFrameMemMgt>::CreateInstance
#define	g_FrameMemMgt							CSingleton<CFrameMemMgt>::GetInstance()
#define	DESTROY_FRAMEMEMMGT_INSTANCE			CSingleton<CFrameMemMgt>::DestroyInstance

#define MALLOC(size)	\
	(g_FrameMemMgt.RecordAllocInfo(__FILE__, __LINE__, g_FrameMemMgt.GetBlockSize(size)),g_FrameMemMgt.AllocBlock(size))

#define FREE(ptr)		\
	if(ptr != NULL)		\
	{	\
		MemBlockHead *pHead = (MemBlockHead *)(ptr - sizeof(MemBlockHead));		\
		g_FrameMemMgt.RecordRecycleInfo(__FILE__, __LINE__, pHead->m_nBlockSize);		\
		g_FrameMemMgt.RecycleBlock(ptr);		\
	}


FRAME_NAMESPACE_END

#endif /* FRAME_MEM_MGT_H_ */
