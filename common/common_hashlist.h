/*
 * common_hashlist.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_HASHLIST_H_
#define COMMON_HASHLIST_H_

#include <stdio.h>
#include <memory.h>
#include "common_typedef.h"
#include "common_errordef.h"
#include "common_object.h"


//����һ����ϣ�б�ģ����
template<typename T, uint32_t CAPACITY, uint32_t BUCKETSIZE, bool RECYCLETOTAIL = true>
class CHashList : public CObject
{
public:
	class CIndex;

	enum
	{
		enmMaxKeyLength					= 8,		//��������
		enmIndexFlag_Free				= 0,		//������������״̬
		enmIndexFlag_Used				= 1,		//����������ʹ��״̬
		enmInvalidObjectID				= -1,		//��Ч�Ķ���ID
	};

	//������
	typedef struct tagKey
	{
		uint32_t		nKeyLength;					//������
		uint8_t			arrKey[enmMaxKeyLength];	//��ֵ
	} Key;

	//��ͻ���ڵ㶨��
	typedef struct tagConflictData
	{
		Key				key;						//��ֵ
		CComplexType<T>	object;
		//CIndex*			pNextConflict;				//��ͻ������һ���ڵ�
		int32_t			nNextConflict;				//��ͻ������һ���ڵ�
	} ConflictData;

	//ͰԪ�ؽṹ
	typedef struct tagBucket
	{
		//CIndex*			pHeadConflict;				//��ͻ�����׽ڵ�
		int32_t			nHeadConflict;				//��ͻ�����׽ڵ�
	} Bucket;


public:
	//��������
	class CIndex : public CObject
	{
	public:
		CIndex()
		{
		}
		virtual ~CIndex()
		{
		}

	public:
		//��ʼ����������
		virtual int32_t Initialize()
		{
			m_pContainer = NULL;
			m_nFlag = enmIndexFlag_Free;
			//m_pPrev = NULL;
			//m_pNext = NULL;
			m_nPrev = enmInvalidObjectID;
			m_nNext = enmInvalidObjectID;

			memset(&m_conflictData.key, 0, sizeof(m_conflictData.key));
			//m_conflictData.pNextConflict = NULL;
			m_conflictData.nNextConflict = enmInvalidObjectID;

			//m_conflictData.object.Initialize();

			return S_OK;
		}
		//�ָ���������
		virtual int32_t Resume()
		{
			//m_conflictData.object.Resume();

			return S_OK;
		}
		//ע����������
		virtual int32_t Uninitialize()
		{
			m_nFlag = enmIndexFlag_Free;
			//m_pPrev = NULL;
			//m_pNext = NULL;
			m_nPrev = enmInvalidObjectID;
			m_nNext = enmInvalidObjectID;

			memset(&m_conflictData.key, 0, sizeof(m_conflictData.key));
			//m_conflictData.pNextConflict = NULL;
			m_conflictData.nNextConflict = enmInvalidObjectID;

			//m_conflictData.object.Uninitialize();

			return S_OK;
		}

	public:
		////��ȡ��һ����ͻ�ڵ�
		//inline CIndex* NextConflict() const
		//{
		//	return m_conflictData.pNextConflict;
		//}
		//��ȡ��һ����ͻ�ڵ�
		inline CIndex* NextConflict() const
		{
			if (enmInvalidObjectID == m_conflictData.nNextConflict)
			{
				return NULL;
			}
			return &m_pContainer->m_arrIndex[m_conflictData.nNextConflict];
		}

		//��ȡ�����ֵ
		const Key& GetKey() const
		{
			return m_conflictData.key;
		}

		////��ȡ�����ǰһ�ڵ�
		//inline CIndex* Prev() const
		//{
		//	return m_pPrev;
		//}
		////��ȡ����ĺ�һ�ڵ�
		//inline CIndex* Next() const
		//{
		//	return m_pNext;
		//}
		inline CIndex* Prev()
		{
			if (enmInvalidObjectID == m_nPrev)
			{
				return NULL;
			}
			return &m_pContainer->m_arrIndex[m_nPrev];
		}
		//��ȡ����ĺ�һ�ڵ�
		inline CIndex* Next()
		{
			if (enmInvalidObjectID == m_nNext)
			{
				return NULL;
			}
			return &m_pContainer->m_arrIndex[m_nNext];
		}

		//��ȡ����
		inline T& GetObject()
		{
			return m_conflictData.object.GetValue();
		}

		//��ȡ����2(�������error C2039: 'GetObjectW' : is not a member of 'CHashList<T,CAPACITY,BUCKETSIZE>::CIndex')
		inline T& Object()
		{
			return m_conflictData.object.GetValue();
		}

		//���ö���
		inline void SetObject(T& object)
		{
			m_conflictData.object.SetValue(object);
		}

		//��ȡ����ID
		inline int32_t Index()
		{
			return m_nIndex;
		}

	protected:
		//���ø�����ָ��
		inline void SetContainer(CHashList* pContainer)
		{
			m_pContainer = pContainer;
		}
		//���ýڵ�Ϊ��ʹ��
		inline void SetUsed()
		{
			m_nFlag = enmIndexFlag_Used;
		}
		//���ýڵ�Ϊ����
		inline void SetFree()
		{
			m_nFlag = enmIndexFlag_Free;
		}
		//�ڵ��Ƿ���ʹ��
		inline bool IsUsed() const
		{
			return (enmIndexFlag_Used == m_nFlag);
		}

		////��ȡ����ĺ�һ�ڵ�
		//inline CIndex* GetNext() const
		//{
		//	return m_pNext;
		//}
		////��ȡ�����ǰһ�ڵ�
		//inline CIndex* GetPrev() const
		//{
		//	return m_pPrev;
		//}
		////��������ĺ�һ�ڵ�
		//inline void SetNext(CIndex* pNext)
		//{
		//	m_pNext = pNext;
		//}
		////���������ǰһ�ڵ�
		//inline void SetPrev(CIndex* pPrev)
		//{
		//	m_pPrev = pPrev;
		//}
		//��ȡ����ĺ�һ�ڵ�
		inline int32_t GetNext() const
		{
			return m_nNext;
		}
		//��ȡ�����ǰһ�ڵ�
		inline int32_t GetPrev() const
		{
			return m_nPrev;
		}
		//��������ĺ�һ�ڵ�
		inline void SetNext(int32_t nNext)
		{
			m_nNext = nNext;
		}
		//���������ǰһ�ڵ�
		inline void SetPrev(int32_t nPrev)
		{
			m_nPrev = nPrev;
		}

		//���ö���ID
		inline void SetIndex(int32_t nIndex)
		{
			m_nIndex = nIndex;
		}

		//���ýڵ��ֵ
		inline void SetKey(Key key)
		{
			memcpy(&m_conflictData.key, &key, sizeof(Key));
		}

		////��ȡ��һ����ͻ�ڵ�
		//inline CIndex* GetNextConflict()
		//{
		//	return m_conflictData.pNextConflict;
		//}
		////������һ����ͻ�ڵ�
		//inline void SetNextConflict(CIndex* pNextConflict)
		//{
		//	m_conflictData.pNextConflict = pNextConflict;
		//}
		//��ȡ��һ����ͻ�ڵ�
		inline int32_t GetNextConflict()
		{
			return m_conflictData.nNextConflict;
		}
		//������һ����ͻ�ڵ�
		inline void SetNextConflict(int32_t nNextConflict)
		{
			m_conflictData.nNextConflict = nNextConflict;
		}

		//���ó�ͻ���ڵ�����
		inline void ResetConflictData()
		{
			memset(&m_conflictData.key, 0, sizeof(m_conflictData.key));
			//m_conflictData.pNextConflict = NULL;
			m_conflictData.nNextConflict = enmInvalidObjectID;
		}

	protected:
		CHashList*		m_pContainer;

		uint32_t		m_nFlag;		//����״̬��־
		int32_t			m_nIndex;		//����������
		//CIndex*			m_pNext;		//��һ�ڵ�
		//CIndex*			m_pPrev;		//ǰһ�ڵ�
		int32_t			m_nNext;		//��һ�ڵ�
		int32_t			m_nPrev;		//ǰһ�ڵ�

		ConflictData	m_conflictData;	//�����ĳ�ͻ��Ԫ��

		friend class CHashList;
	};


public:
	CHashList()
	{
	}
	virtual ~CHashList()
	{
	}

public:
	//��ʼ����ϣ��
	virtual int32_t Initialize()
	{
		m_nObjectCount = 0;
		m_nFreeCount = CAPACITY;
		//m_pFreeHead = &m_arrIndex[0];
		//m_pFreeTail = &m_arrIndex[CAPACITY - 1];
		//m_pUsedHead = NULL;
		//m_pUsedTail = NULL;
		m_nFreeHead = 0;
		m_nFreeTail = CAPACITY - 1;
		m_nUsedHead = enmInvalidObjectID;
		m_nUsedTail = enmInvalidObjectID;

		for (uint32_t i = 0; i < CAPACITY; ++i)
		{
			m_arrIndex[i].Initialize();
		}

		for (uint32_t i = 0; i < BUCKETSIZE; ++i)
		{
			//m_arrBucket[i].pHeadConflict = NULL;
			m_arrBucket[i].nHeadConflict = enmInvalidObjectID;
		}

		BuildIndexList();

		return S_OK;
	}
	//�ָ���ϣ��
	virtual int32_t Resume()
	{
		for (uint32_t i = 0; i < CAPACITY; ++i)
		{
			m_arrIndex[i].Resume();
		}
		return S_OK;
	}
	//ע����ϣ��
	virtual int32_t Uninitialize()
	{
		m_nObjectCount = 0;
		m_nFreeCount = CAPACITY;
		//m_pFreeHead = &m_arrIndex[0];
		//m_pFreeTail = &m_arrIndex[CAPACITY - 1];
		//m_pUsedHead = NULL;
		//m_pUsedTail = NULL;
		m_nFreeHead = 0;
		m_nFreeTail = CAPACITY - 1;
		m_nUsedHead = enmInvalidObjectID;
		m_nUsedTail = enmInvalidObjectID;

		for (uint32_t i = 0; i < CAPACITY; ++i)
		{
			m_arrIndex[i].Uninitialize();
		}

		for (uint32_t i = 0; i < BUCKETSIZE; ++i)
		{
			//m_arrBucket[i].pHeadConflict = NULL;
			m_arrBucket[i].nHeadConflict = enmInvalidObjectID;
		}

		BuildIndexList();

		return S_OK;
	}

public:
	////���ϣ���в������
	//CIndex* Insert(Key key, T object)
	//{
	//	CIndex* pIndex = NULL;

	//	//����ֵ�Ƿ��ͻ
	//	pIndex = Find(key);
	//	if (NULL != pIndex)
	//	{
	//		return NULL;
	//	}

	//	//��������
	//	pIndex = CreateIndex();
	//	if (NULL == pIndex)
	//	{
	//		return NULL;
	//	}

	//	//�����ϣֵ
	//	uint32_t nHashCode = GetHashCode(key);
	//	uint32_t nBucketIndex = nHashCode % BUCKETSIZE;

	//	if (NULL == m_arrBucket[nBucketIndex].pHeadConflict)
	//	{
	//		//���½ڵ���뵽��ͻ��ͷ��
	//		m_arrBucket[nBucketIndex].pHeadConflict = pIndex;
	//	}
	//	else
	//	{
	//		CIndex* pTailConflict = m_arrBucket[nBucketIndex].pHeadConflict;
	//		CIndex* pNextConflict = pTailConflict->GetNextConflict();
	//		//�ҵ���ͻ����β���ڵ�
	//		while (NULL != pNextConflict)
	//		{
	//			pTailConflict = pNextConflict;
	//			pNextConflict = pTailConflict->GetNextConflict();
	//		}
	//		//���½ڵ���ӵ�β��
	//		pTailConflict->SetNextConflict(pIndex);
	//	}

	//	//�����½ڵ��ͻ����
	//	pIndex->SetKey(key);
	//	pIndex->SetNextConflict(NULL);
	//	pIndex->SetObject(object);

	//	return pIndex;
	//}
	////�ӹ�ϣ����ɾ������
	//int32_t Erase(Key key)
	//{
	//	//�����ϣֵ
	//	uint32_t nHashCode = GetHashCode(key);
	//	uint32_t nBucketIndex = nHashCode % BUCKETSIZE;

	//	CIndex* pPrevConflict = NULL;
	//	CIndex* pConflict = m_arrBucket[nBucketIndex].pHeadConflict;
	//	if (NULL == pConflict)
	//	{
	//		return E_OBJECTNOTEXIST;
	//	}
	//	//������ָ����ֵ��ȵ����нڵ�
	//	while (NULL != pConflict)
	//	{
	//		if (IsEqual(key, pConflict->GetKey()))
	//		{
	//			if (NULL == pPrevConflict)
	//			{
	//				m_arrBucket[nBucketIndex].pHeadConflict = pConflict->GetNextConflict();
	//			}
	//			else
	//			{
	//				pPrevConflict->SetNextConflict(pConflict->GetNextConflict());
	//			}
	//			DestroyIndex(pConflict);
	//			pConflict->ResetConflictData();
	//			if (NULL == pPrevConflict)
	//			{
	//				pConflict = m_arrBucket[nBucketIndex].pHeadConflict;
	//			}
	//			else
	//			{
	//				pConflict = pPrevConflict->GetNextConflict();
	//			}
	//			continue;
	//		}
	//		pPrevConflict = pConflict;
	//		pConflict = pConflict->GetNextConflict();
	//	}

	//	return S_OK;
	//}
	////�ӹ�ϣ����ɾ������
	//int32_t Erase(CIndex* pIndex)
	//{
	//	int32_t ret = CheckIndex(pIndex);
	//	if (ret < 0)
	//	{
	//		return ret;
	//	}

	//	//�����ϣֵ
	//	uint32_t nHashCode = GetHashCode(pIndex->GetKey());
	//	uint32_t nBucketIndex = nHashCode % BUCKETSIZE;

	//	CIndex* pPrevConflict = NULL;
	//	CIndex* pConflict = m_arrBucket[nBucketIndex].pHeadConflict;
	//	//����ָ���Ľڵ�
	//	while (NULL != pConflict)
	//	{
	//		if (pConflict == pIndex)
	//		{
	//			if (NULL == pPrevConflict)
	//			{
	//				m_arrBucket[nBucketIndex].pHeadConflict = pConflict->GetNextConflict();
	//			}
	//			else
	//			{
	//				pPrevConflict->SetNextConflict(pConflict->GetNextConflict());
	//			}
	//			return DestroyIndex(pIndex);
	//		}
	//		pPrevConflict = pConflict;
	//		pConflict = pConflict->GetNextConflict();
	//	}

	//	return E_OBJECTNOTEXIST;
	//}
	////������ж���
	//void Clear()
	//{
	//	Uninitialize();
	//}
	////���ݼ�ֵ���Ҷ���
	//CIndex* Find(Key key)
	//{
	//	//�����ϣֵ
	//	uint32_t nHashCode = GetHashCode(key);
	//	uint32_t nBucketIndex = nHashCode % BUCKETSIZE;

	//	CIndex* pConflict = m_arrBucket[nBucketIndex].pHeadConflict;
	//	//�ҵ���ͻ����β���ڵ�
	//	while (NULL != pConflict)
	//	{
	//		if (IsEqual(key, pConflict->GetKey()))
	//		{
	//			return pConflict;
	//		}
	//		pConflict = pConflict->GetNextConflict();
	//	}

	//	return NULL;
	//}
	//���ϣ���в������
	CIndex* Insert(Key key, T object)
	{
		CIndex* pIndex = NULL;

		//����ֵ�Ƿ��ͻ
		pIndex = Find(key);
		if (NULL != pIndex)
		{
			return NULL;
		}

		//��������
		pIndex = CreateIndex();
		if (NULL == pIndex)
		{
			return NULL;
		}

		//�����ϣֵ
		uint32_t nHashCode = GetHashCode(key);
		uint32_t nBucketIndex = nHashCode % BUCKETSIZE;

		if (enmInvalidObjectID == m_arrBucket[nBucketIndex].nHeadConflict)
		{
			//���½ڵ���뵽��ͻ��ͷ��
			m_arrBucket[nBucketIndex].nHeadConflict = pIndex->Index();
		}
		else
		{
			int32_t nTailConflict = m_arrBucket[nBucketIndex].nHeadConflict;
			int32_t nNextConflict = m_arrIndex[nTailConflict].GetNextConflict();
			//�ҵ���ͻ����β���ڵ�
			while (enmInvalidObjectID != nNextConflict)
			{
				nTailConflict = nNextConflict;
				nNextConflict = m_arrIndex[nTailConflict].GetNextConflict();
			}
			//���½ڵ���ӵ�β��
			m_arrIndex[nTailConflict].SetNextConflict(pIndex->Index());
		}

		//�����½ڵ��ͻ����
		pIndex->SetKey(key);
		pIndex->SetNextConflict(enmInvalidObjectID);
		pIndex->SetObject(object);

		return pIndex;
	}
	//�ӹ�ϣ����ɾ������
	int32_t Erase(Key key)
	{
		//�����ϣֵ
		uint32_t nHashCode = GetHashCode(key);
		uint32_t nBucketIndex = nHashCode % BUCKETSIZE;

		int32_t nPrevConflict = enmInvalidObjectID;
		int32_t nConflict = m_arrBucket[nBucketIndex].nHeadConflict;
		if (enmInvalidObjectID == nConflict)
		{
			return E_OBJECTNOTEXIST;
		}
		//������ָ����ֵ��ȵ����нڵ�
		while (enmInvalidObjectID != nConflict)
		{
			if (IsEqual(key, m_arrIndex[nConflict].GetKey()))
			{
				if (enmInvalidObjectID == nPrevConflict)
				{
					m_arrBucket[nBucketIndex].nHeadConflict = m_arrIndex[nConflict].GetNextConflict();
				}
				else
				{
					m_arrIndex[nPrevConflict].SetNextConflict(m_arrIndex[nConflict].GetNextConflict());
				}
				DestroyIndex(&m_arrIndex[nConflict]);
				m_arrIndex[nConflict].ResetConflictData();
				if (enmInvalidObjectID == nPrevConflict)
				{
					nConflict = m_arrBucket[nBucketIndex].nHeadConflict;
				}
				else
				{
					nConflict = m_arrIndex[nPrevConflict].GetNextConflict();
				}
				continue;
			}
			nPrevConflict = nConflict;
			nConflict = m_arrIndex[nConflict].GetNextConflict();
		}

		return S_OK;
	}
	//�ӹ�ϣ����ɾ������
	int32_t Erase(CIndex* pIndex)
	{
		int32_t ret = CheckIndex(pIndex);
		if (ret < 0)
		{
			return ret;
		}

		//�����ϣֵ
		uint32_t nHashCode = GetHashCode(pIndex->GetKey());
		uint32_t nBucketIndex = nHashCode % BUCKETSIZE;

		int32_t nPrevConflict = enmInvalidObjectID;
		int32_t nConflict = m_arrBucket[nBucketIndex].nHeadConflict;
		//����ָ���Ľڵ�
		while (enmInvalidObjectID != nConflict)
		{
			if (nConflict == pIndex->Index())
			{
				if (enmInvalidObjectID == nPrevConflict)
				{
					m_arrBucket[nBucketIndex].nHeadConflict = m_arrIndex[nConflict].GetNextConflict();
				}
				else
				{
					m_arrIndex[nPrevConflict].SetNextConflict(m_arrIndex[nConflict].GetNextConflict());
				}
				return DestroyIndex(pIndex);
			}
			nPrevConflict = nConflict;
			nConflict = m_arrIndex[nConflict].GetNextConflict();
		}

		return E_OBJECTNOTEXIST;
	}
	//������ж���
	void Clear()
	{
		Uninitialize();
	}
	//���ݼ�ֵ���Ҷ���
	CIndex* Find(Key key)
	{
		//�����ϣֵ
		uint32_t nHashCode = GetHashCode(key);
		uint32_t nBucketIndex = nHashCode % BUCKETSIZE;

		int32_t nConflict = m_arrBucket[nBucketIndex].nHeadConflict;
		//�ҵ���ͻ����β���ڵ�
		while (enmInvalidObjectID != nConflict)
		{
			if (IsEqual(key, m_arrIndex[nConflict].GetKey()))
			{
				return &m_arrIndex[nConflict];
			}
			nConflict = m_arrIndex[nConflict].GetNextConflict();
		}

		return NULL;
	}

	////��ȡͷ����������
	//CIndex* First() const
	//{
	//	return m_pUsedHead;
	//}
	////��ȡβ����������
	//CIndex* Last() const
	//{
	//	return m_pUsedTail;
	//}
	//��ȡͷ����������
	inline CIndex* First()
	{
		return (enmInvalidObjectID == m_nUsedHead) ? NULL : &m_arrIndex[m_nUsedHead];
	}
	//��ȡβ����������
	inline CIndex* Last()
	{
		return (enmInvalidObjectID == m_nUsedTail) ? NULL : &m_arrIndex[m_nUsedTail];
	}

	//��ȡ���������
	inline uint32_t Capacity() const
	{
		return CAPACITY;
	}
	//��ȡ������ж�������
	inline uint32_t ObjectCount() const
	{
		return m_nObjectCount;
	}

	//������Ƿ�Ϊ��
	inline bool IsEmpty() const
	{
		return (0 == m_nObjectCount);
	}
	//������Ƿ�����
	inline bool IsFull() const
	{
		return (m_nObjectCount == CAPACITY);
	}

	//����λ�û�ȡ����
	inline CIndex* GetIndex(const int32_t nIndex)
	{
		int32_t ret = CheckIndex(nIndex);
		if (0 > ret)
		{
			return NULL;
		}
		return &m_arrIndex[nIndex];
	}
	//����λ�û�ȡ����
	inline int32_t GetIndex(const CIndex* pIndex)
	{
		int32_t ret = CheckIndex(pIndex);
		if (0 > ret)
		{
			return enmInvalidObjectID;
		}
		return ((uint8_t*)pIndex - (uint8_t*)m_arrIndex) / sizeof(CIndex);
	}

protected:
	////����������������
	//int32_t BuildIndexList()
	//{
	//	//���õ�0������
	//	m_arrIndex[0].SetIndex(0);
	//	m_arrIndex[0].SetPrev(NULL);

	//	for (uint32_t i = 1; i < CAPACITY; ++i)
	//	{
	//		m_arrIndex[i].SetIndex(i);
	//		m_arrIndex[i - 1].SetNext(&m_arrIndex[i]);
	//		m_arrIndex[i].SetPrev(&m_arrIndex[i - 1]);
	//	}

	//	//�������һ������
	//	m_arrIndex[CAPACITY - 1].SetNext(NULL);

	//	return S_OK;
	//}
	inline int32_t BuildIndexList()
	{
		//���õ�0������
		m_arrIndex[0].SetContainer(this);
		m_arrIndex[0].SetIndex(0);
		m_arrIndex[0].SetPrev(enmInvalidObjectID);

		for (uint32_t i = 1; i < CAPACITY; ++i)
		{
			m_arrIndex[i].SetContainer(this);
			m_arrIndex[i].SetIndex(i);
			m_arrIndex[i].SetPrev(i - 1);
			m_arrIndex[i - 1].SetNext(i);
		}

		//�������һ������
		m_arrIndex[CAPACITY - 1].SetNext(enmInvalidObjectID);

		return S_OK;
	}

	////��������
	//CIndex* CreateIndex()
	//{
	//	//���������
	//	if (IsFull())
	//	{
	//		return NULL;
	//	}

	//	//�ӿ�������ͷ��ȡ��һ��������
	//	CIndex* pIndex = m_pFreeHead;
	//	if (NULL == pIndex)
	//	{
	//		return NULL;
	//	}

	//	//�����������������ͷ����β��ָ��
	//	if (m_pFreeTail == pIndex)
	//	{
	//		m_pFreeTail = pIndex->GetNext();
	//	}
	//	m_pFreeHead = pIndex->GetNext();
	//	if (NULL != m_pFreeHead)
	//	{
	//		m_pFreeHead->SetPrev(NULL);
	//	}

	//	//����������ӵ���ʹ����������β��
	//	pIndex->SetPrev(m_pUsedTail);
	//	pIndex->SetNext(NULL);
	//	if (NULL != m_pUsedTail)
	//	{
	//		m_pUsedTail->SetNext(pIndex);
	//	}
	//	m_pUsedTail = pIndex;
	//	if (NULL == m_pUsedHead)
	//	{
	//		m_pUsedHead = m_pUsedTail;
	//	}

	//	pIndex->SetUsed();

	//	++m_nObjectCount;
	//	--m_nFreeCount;

	//	return pIndex;
	//}
	////���ٶ���
	//int32_t DestroyIndex(CIndex* pIndex)
	//{
	//	int32_t ret = CheckIndex(pIndex);
	//	if (ret < 0)
	//	{
	//		return ret;
	//	}

	//	//������ʹ�����������ͷ����β��ָ��
	//	if (m_pUsedHead == pIndex)
	//	{
	//		m_pUsedHead = pIndex->GetNext();
	//	}
	//	if (m_pUsedTail == pIndex)
	//	{
	//		m_pUsedTail = pIndex->GetPrev();
	//	}

	//	//���ڵ��������ɾ��
	//	if (NULL != pIndex->GetPrev())
	//	{
	//		pIndex->GetPrev()->SetNext(pIndex->GetNext());
	//	}
	//	if (NULL != pIndex->GetNext())
	//	{
	//		pIndex->GetNext()->SetPrev(pIndex->GetPrev());
	//	}

	//	//��������ӵ�������������β��
	//	pIndex->SetPrev(m_pFreeTail);
	//	pIndex->SetNext(NULL);
	//	if (NULL != m_pFreeTail)
	//	{
	//		m_pFreeTail->SetNext(pIndex);
	//	}
	//	m_pFreeTail = pIndex;
	//	if (NULL == m_pFreeHead)
	//	{
	//		m_pFreeHead = m_pFreeTail;
	//	}

	//	pIndex->SetFree();

	//	--m_nObjectCount;
	//	++m_nFreeCount;

	//	return S_OK;
	//}
	//��������
	inline CIndex* CreateIndex()
	{
		//���������
		if (IsFull())
		{
			return NULL;
		}

		//�ӿ�������ͷ��ȡ��һ��������
		if (enmInvalidObjectID == m_nFreeHead)
		{
			return NULL;
		}
		CIndex* pIndex = &m_arrIndex[m_nFreeHead];

		//�����������������ͷ����β��ָ��
		if (m_nFreeTail == pIndex->Index())
		{
			m_nFreeTail = pIndex->GetNext();
		}
		m_nFreeHead = pIndex->GetNext();
		if (enmInvalidObjectID != m_nFreeHead)
		{
			m_arrIndex[m_nFreeHead].SetPrev(enmInvalidObjectID);
		}

		//����������ӵ���ʹ����������β��
		pIndex->SetPrev(m_nUsedTail);
		pIndex->SetNext(enmInvalidObjectID);
		if (enmInvalidObjectID != m_nUsedTail)
		{
			m_arrIndex[m_nUsedTail].SetNext(pIndex->Index());
		}
		m_nUsedTail = pIndex->Index();
		if (enmInvalidObjectID == m_nUsedHead)
		{
			m_nUsedHead = m_nUsedTail;
		}

		pIndex->SetUsed();

		++m_nObjectCount;
		--m_nFreeCount;

		pIndex->m_conflictData.object.Initialize();

		return pIndex;
	}
	//���ٶ���
	inline int32_t DestroyIndex(CIndex* pIndex)
	{
		int32_t ret = CheckIndex(pIndex);
		if (ret < 0)
		{
			return ret;
		}

		//������ʹ�����������ͷ����β��ָ��
		if (m_nUsedHead == pIndex->Index())
		{
			m_nUsedHead = pIndex->GetNext();
		}
		if (m_nUsedTail == pIndex->Index())
		{
			m_nUsedTail = pIndex->GetPrev();
		}

		//���ڵ��������ɾ��
		if (enmInvalidObjectID != pIndex->GetPrev())
		{
			pIndex->Prev()->SetNext(pIndex->GetNext());
		}
		if (enmInvalidObjectID != pIndex->GetNext())
		{
			pIndex->Next()->SetPrev(pIndex->GetPrev());
		}

		if (RECYCLETOTAIL)
		{
			//��������ӵ�������������β��
			pIndex->SetPrev(m_nFreeTail);
			pIndex->SetNext(enmInvalidObjectID);
			if (enmInvalidObjectID != m_nFreeTail)
			{
				m_arrIndex[m_nFreeTail].SetNext(pIndex->Index());
			}
			m_nFreeTail = pIndex->Index();
			if (enmInvalidObjectID == m_nFreeHead)
			{
				m_nFreeHead = m_nFreeTail;
			}
		}
		else
		{
			//��������ӵ�������������ͷ��
			pIndex->SetNext(m_nFreeHead);
			pIndex->SetPrev(enmInvalidObjectID);
			if (enmInvalidObjectID != m_nFreeHead)
			{
				m_arrIndex[m_nFreeHead].SetPrev(pIndex->Index());
			}
			m_nFreeHead = pIndex->Index();
			if (enmInvalidObjectID == m_nFreeTail)
			{
				m_nFreeTail = m_nFreeHead;
			}
		}

		pIndex->SetFree();

		--m_nObjectCount;
		++m_nFreeCount;

		pIndex->m_conflictData.object.Uninitialize();

		return S_OK;
	}

	//�����ϣֵ
	inline uint32_t GetHashCode(Key key)
	{
		uint32_t nHashCode = 0;
		uint16_t *tmp1 = (uint16_t*)key.arrKey;

		for (uint32_t i = 0; i < key.nKeyLength / sizeof(uint16_t); ++i)
		{
			nHashCode += tmp1[i];
		}

		uint16_t n = 0;
		uint8_t* tmp2 = NULL;
		if (key.nKeyLength % sizeof(uint16_t) > 0)
		{
			tmp2 = (key.arrKey + (key.nKeyLength - (key.nKeyLength % sizeof(uint16_t))));
			memcpy(&n, tmp2, key.nKeyLength % sizeof(uint16_t));
		}

		nHashCode += n;
		nHashCode = (nHashCode & (uint16_t)0x7FFF);

		return nHashCode;
	}

	//��ֵ�Ƚ�
	inline bool IsEqual(const Key keySrc, const Key keyDest)
	{
		return ((keySrc.nKeyLength == keyDest.nKeyLength) && (0 == memcmp(keySrc.arrKey, keyDest.arrKey, keySrc.nKeyLength)));
	}

	//ȷ����������ָ���Ƿ���Ч
	inline int32_t CheckIndex(CIndex* pIndex)
	{
		if ((pIndex < m_arrIndex) || (pIndex >= m_arrIndex + CAPACITY))
		{
			return E_INVALIDOBJECTINDEX;
		}
		if (((uint8_t*)pIndex - (uint8_t*)m_arrIndex) % sizeof(CIndex) != 0)
		{
			return E_INVALIDOBJECTINDEX;
		}
		if (!pIndex->IsUsed())
		{
			return E_OBJECTINDEXNOTUSED;
		}
		return S_OK;
	}
	//ȷ�����������Ƿ���Ч
	inline int32_t CheckIndex(int32_t nIndex)
	{
		if ((0 > nIndex) || ((int32_t)CAPACITY < nIndex))
		{
			return E_INVALIDOBJECTINDEX;
		}
		if (!m_arrIndex[nIndex].IsUsed())
		{
			return E_OBJECTINDEXNOTUSED;
		}
		return S_OK;
	}

protected:
	uint32_t		m_nObjectCount;		//�ѷ������ĸ���
	uint32_t		m_nFreeCount;		//δ�������ĸ���
	//CIndex*			m_pFreeHead;		//δ���������ͷ�����������
	//CIndex*			m_pFreeTail;		//δ���������β�����������
	//CIndex*			m_pUsedHead;		//�ѷ��������ͷ�����������
	//CIndex*			m_pUsedTail;		//�ѷ��������β�����������
	int32_t			m_nFreeHead;		//δ���������ͷ�����������
	int32_t			m_nFreeTail;		//δ���������β�����������
	int32_t			m_nUsedHead;		//�ѷ��������ͷ�����������
	int32_t			m_nUsedTail;		//�ѷ��������β�����������

	CIndex			m_arrIndex[CAPACITY];	//����������

	Bucket			m_arrBucket[BUCKETSIZE];//Ͱ
};


#endif /* COMMON_HASHLIST_H_ */
