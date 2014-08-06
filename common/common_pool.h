/*
 * common_pool.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_POOL_H_
#define COMMON_POOL_H_

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "common_typedef.h"
#include "common_errordef.h"
#include "common_object.h"
#include "common_complextype.h"
#include "common_api.h"


//����һ������ع���ģ����
template<typename T, uint32_t CAPACITY, bool RECYCLETOTAIL = true>
class CPool : public CObject
{
public:
	enum
	{
		enmMaxAdditionalDataSize		= 4,		//��󸽼��ֶγ���
		enmIndexFlag_Free				= 0,		//������������״̬
		enmIndexFlag_Used				= 1,		//����������ʹ��״̬
		enmInvalidObjectID				= -1,		//��Ч�Ķ���ID
	};

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
			m_nIndex = enmInvalidObjectID;
			//m_pPrev = NULL;
			//m_pNext = NULL;
			m_nPrev = enmInvalidObjectID;
			m_nNext = enmInvalidObjectID;

			ClearAdditionalData();

			//m_object.Initialize();

			return S_OK;
		}
		//�ָ���������
		virtual int32_t Resume()
		{
			//m_object.Resume();

			return S_OK;
		}
		//ע����������
		virtual int32_t Uninitialize()
		{
			m_nFlag = enmIndexFlag_Free;
			m_nIndex = enmInvalidObjectID;
			//m_pPrev = NULL;
			//m_pNext = NULL;
			m_nPrev = enmInvalidObjectID;
			m_nNext = enmInvalidObjectID;

			ClearAdditionalData();

			//m_object.Uninitialize();

			return S_OK;
		}

	public:
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
		//��ȡ�����ǰһ�ڵ�
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

		//��ȡ����ָ��
		inline T& Object()
		{
			return m_object.GetValue();
		}

		//��ȡ����ָ��
		inline T* ObjectPtr()
		{
			return (T*)&m_object.GetValue();
		}

		//��ȡ����ID
		inline int32_t Index()
		{
			return m_nIndex;
		}

	protected:
		//���ø�����ָ��
		inline void SetContainer(CPool* pContainer)
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

	public:
		//��ȡ��������
		int32_t GetAdditionalData(int32_t nIndex, uint64_t& nValue)
		{
			if (nIndex < 0 || nIndex >= enmMaxAdditionalDataSize)
			{
				return E_UNKNOWN;
			}

			nValue = m_arrAdditionalData[nIndex];

			return S_OK;
		}
		//���ø�������
		int32_t SetAdditionalData(int32_t nIndex, uint64_t nValue)
		{
			if (nIndex < 0 || nIndex >= enmMaxAdditionalDataSize)
			{
				return E_UNKNOWN;
			}

			m_arrAdditionalData[nIndex] = nValue;

			return S_OK;
		}
		//��ȡ��������
		int32_t GetAdditionalData(int32_t nIndex, uint32_t& nValue)
		{
			if (nIndex < 0 || nIndex >= enmMaxAdditionalDataSize)
			{
				return E_UNKNOWN;
			}

			nValue = (uint32_t)m_arrAdditionalData[nIndex];

			return S_OK;
		}
		//���ø�������
		int32_t SetAdditionalData(int32_t nIndex, uint32_t nValue)
		{
			if (nIndex < 0 || nIndex >= enmMaxAdditionalDataSize)
			{
				return E_UNKNOWN;
			}

			m_arrAdditionalData[nIndex] = (uint64_t)nValue;

			return S_OK;
		}
		//�����������
		void ClearAdditionalData()
		{
			memset(m_arrAdditionalData, 0, sizeof(m_arrAdditionalData));
		}


	//protected:
	public:
		CPool*			m_pContainer;

		uint32_t		m_nFlag;		//����״̬��־
		int32_t			m_nIndex;		//����������
		//CIndex*			m_pNext;		//��һ�ڵ�
		//CIndex*			m_pPrev;		//ǰһ�ڵ�
		int32_t			m_nNext;		//��һ�ڵ�
		int32_t			m_nPrev;		//ǰһ�ڵ�
		uint64_t		m_arrAdditionalData[enmMaxAdditionalDataSize];	//��������

		CComplexType<T>	m_object;		//�����Ķ���

		friend class CPool;
	};


public:
	CPool()
	{
	}
	virtual ~CPool()
	{
	}

public:
	//��ʼ�������
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

		BuildIndexList();

		return S_OK;
	}
	//�ָ������
	virtual int32_t Resume()
	{
		for (uint32_t i = 0; i < CAPACITY; ++i)
		{
			m_arrIndex[i].Resume();
		}
		return S_OK;
	}
	//ע�������
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

		BuildIndexList();

		return S_OK;
	}

public:
	//��������
	CIndex* CreateObject()
	{
		return CreateIndex();
	}
	//���ٶ���
	int32_t DestroyObject(CIndex* pIndex)
	{
		return DestroyIndex(pIndex);
	}
	//���ٶ���
	int32_t DestroyObject(int32_t nIndex)
	{
		CIndex* pIndex = GetIndex(nIndex);
		if (NULL == pIndex)
		{
			return E_INVALIDARGUMENT;
		}
		return DestroyIndex(pIndex);
	}
	//������ж���
	void Clear()
	{
		Uninitialize();
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
	//�����ȡ����
	inline CIndex* GetRandomIndex()
	{
		if (0 == m_nObjectCount)
		{
			return NULL;
		}

		//srand((unsigned int)time(NULL)); ����������ʼ��һ�ξͿ�����
		uint32_t nIndex = (rand() % m_nObjectCount);
		CIndex* pIndex = First();
		while ((nIndex > 0) && (NULL != pIndex))
		{
			pIndex = pIndex->Next();
			--nIndex;
		}

		return pIndex;
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
	//����������������
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

	//	pIndex->m_object.Initialize();

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

	//	pIndex->m_object.Uninitialize();
	//
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

		pIndex->m_object.Initialize();

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

		pIndex->m_object.Uninitialize();

		return S_OK;
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

//protected:
	public:
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
};


#endif /* COMMON_POOL_H_ */
