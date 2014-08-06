/***************************************************************************************************
*	file name:	common_memory.cpp
*	created:	2008-11-24
*	author:		teras xiao
*	version:	1.0
*	purpose:	�����ڴ������Ķ���
*
*	revision:
*
***************************************************************************************************/


#include "common/common_memory.h"
#include "common/common_errordef.h"
#include "common/common_def.h"


uint8_t* CShareMemory::ms_pMemory = NULL;


CShareMemory::CShareMemory()
{
	m_pAvailableMemory = ms_pMemory + sizeof(CShareMemory);
}


CShareMemory::CShareMemory(key_t key, size_t nSize, uint8_t nShmMode)
{
	m_pAvailableMemory = ms_pMemory + sizeof(CShareMemory);

	if (enmShmMode_Resume != nShmMode)
	{
		Initialize(key, nSize);
	}
	else
	{
		Resume(key, nSize);
	}
}

CShareMemory::~CShareMemory()
{

}

//new����������
void* CShareMemory::operator new(size_t nSize) throw()
{
	if (NULL == ms_pMemory)
	{
		return NULL;
	}

	return (void*)ms_pMemory;
}

//delete����������
void CShareMemory::operator delete(void *pShareMemory)
{
}

//�����СΪnSize���ڴ��
void* CShareMemory::CreateSegment(size_t nSize)
{
	if (nSize <= 0)
	{
		return NULL;
	}
	if (nSize > GetFreeSize())
	{
		return NULL;
	}

	uint8_t* pMemory = m_pAvailableMemory;
	m_pAvailableMemory += nSize;

	return (void*)pMemory;
}

//��ȡʣ��ռ��С(��λ: Byte)
size_t CShareMemory::GetFreeSize()
{
	size_t nUsedSize = GetUsedSize();

	return (m_nTotalSize - nUsedSize);
}

//��ȡ��ʹ�ÿռ��С(��λ: Byte)
size_t CShareMemory::GetUsedSize()
{
	return (size_t)(m_pAvailableMemory - (uint8_t*)this);
}

//���ù����ڴ�ʱ���	
void CShareMemory::SetTimeStamp()
{
	time_t tmNow;
	time(&tmNow);
	m_tmLastTimeStamp = tmNow;

	m_nCRC = (uint32_t)m_key ^ (uint32_t)m_nTotalSize ^ (uint32_t)m_tmCreateTime ^ (uint32_t)m_tmLastTimeStamp;
}

uint8_t CShareMemory::GetMode()
{
	return m_nShmMode;
}

int32_t CShareMemory::Initialize(key_t key, size_t nSize)
{
	m_nShmMode = enmShmMode_Init;
	m_key = key;
	m_nTotalSize = nSize;

	time_t tmNow;
	time(&tmNow);
	m_tmCreateTime = tmNow;
	m_tmLastTimeStamp = tmNow;

	//����У���
	m_nCRC = (uint32_t)m_key ^ (uint32_t)m_nTotalSize ^ (uint32_t)m_tmCreateTime ^ (uint32_t)m_tmLastTimeStamp;

	return S_OK;
}

int32_t CShareMemory::Resume(key_t key, size_t nSize)
{
	//
	if (key != m_key || nSize != m_nTotalSize)
	{
		return Initialize(key, nSize);
	}

	//���У���
	uint32_t nCrc = (uint32_t)m_key ^ (uint32_t)m_nTotalSize ^ (uint32_t)m_tmCreateTime ^ 
		(uint32_t)m_tmLastTimeStamp ^ m_nCRC;
	if (0 != nCrc)
	{
		//У�鲻ͨ��,����г�ʼ��
		return Initialize(key, nSize);
	}

	time_t tmNow;
	time(&tmNow);
	if (((tmNow - m_tmCreateTime) < 0) || ((tmNow - m_tmCreateTime) > 1 * SECOND_PER_YEAR))
	{
		return Initialize(key, nSize);
	}
	if ((tmNow - m_tmLastTimeStamp) > 2 * SECOND_PER_HOUR)
	{
		return Initialize(key, nSize);
	}

	SetTimeStamp();
	m_nShmMode = enmShmMode_Resume;

	return S_OK;
}

CShareMemory* CreateShareMemory(key_t key, size_t nSize, uint8_t nShmMode/* = 0*/)
{
	if (nSize <= 0)
	{
		return NULL;
	}

	size_t nRealSize = nSize + sizeof(CShareMemory);

#ifdef WIN32
	CShareMemory::ms_pMemory = new uint8_t[nRealSize];
	if (NULL == CShareMemory::ms_pMemory)
	{
		return NULL;
	}
#else
	int nShmID = 0;

	//����ָ����С�Ĺ����ڴ�
	nShmID = shmget(key, nRealSize,  IPC_CREAT | IPC_EXCL | 0666);
	if (nShmID < 0)
	{
		if (errno != EEXIST)
		{
			//�����ڴ洳��ʧ��
			return NULL;
		}
		
		//�����ڴ��Ѿ�������ֱ��Attachָ����С���ڴ�
		nShmID = shmget(key, nRealSize, 0666);
		if (nShmID < 0)
		{
			//Attachʧ����Attachȫ�������ڴ�
			nShmID = shmget(key, 0, 0666);
			if (nShmID < 0)
			{
				//Attachʧ��
				return NULL;
			}
			else
			{
				//ɾ��Attach���Ĺ����ڴ�
				if (shmctl(nShmID, IPC_RMID, NULL))
				{
					return NULL;
				}

				//���´��������ڴ�
				nShmID = shmget(key, nRealSize,  IPC_CREAT | IPC_EXCL | 0666);
				if (nShmID < 0)
				{
					return NULL;
				}
			}

		}
		else
		{
			//Attach�������ڴ�����
		}

	}

	//��ȡ�����ڴ��׵�ַ
	CShareMemory::ms_pMemory = (uint8_t*)shmat(nShmID, NULL, 0);
	if (NULL == CShareMemory::ms_pMemory)
	{
		return NULL;
	}
#endif

	//�ڵõ����ڴ��ϴ���һ��CShareMemory����
	return (new CShareMemory(key, nRealSize, nShmMode));
}

int32_t DestroyShareMemory(key_t key)
{
#ifdef WIN32
	if (NULL != CShareMemory::ms_pMemory)
	{
		delete[] CShareMemory::ms_pMemory;
		CShareMemory::ms_pMemory = NULL;
	}
#else
	int nShmID = 0;

	//Attach���ù����ڴ�
	nShmID = shmget(key, 0, 0666);
	if (nShmID < 0)
	{
		return E_SHMATTACH;
	}
	
	//ɾ���ù����ڴ�
	if (shmctl(nShmID, IPC_RMID, NULL))
	{
		return E_SHMDELETE;
	}
#endif

	return S_OK;
}

