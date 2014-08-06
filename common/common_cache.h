/*
 * common_cache.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_CACHE_H_
#define COMMON_CACHE_H_

#include <stdio.h>
#include <memory.h>
#include "common_typedef.h"
#include "common_errordef.h"
#include "common_object.h"
#include "common_complextype.h"


//����һ��Cacheģ����
template<typename T, uint32_t CAPACITY, uint32_t BUCKETSIZE>
class CCache : public CObject
{
public:
	class CIndex;

	enum
	{
		enmMaxKeyLength					= 8,		//��������
		//enmIndexFlag_Free				= 0,		//������������״̬
		//enmIndexFlag_Used				= 1,		//����������ʹ��״̬
		enmInvalidObjectID				= -1,		//��Ч�Ķ���ID
	};

	//��������Ծ״̬
	typedef uint8_t								ActiveState;
	enum
	{
		enmActiveState_Unused					= 0,
		enmActiveState_Active					= 1,
		enmActiveState_Inactive					= 2,
	};
	enum
	{
		enmMaxActiveObjectCount					= CAPACITY / 2,
		enmMaxInactiveObjectCount				= CAPACITY / 2,
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
		int32_t			nNextConflict;				//��ͻ������һ���ڵ�
	} ConflictData;

	//ͰԪ�ؽṹ
	typedef struct tagBucket
	{
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

	protected:
		//��ʼ����������
		virtual int32_t Initialize()
		{
			m_pContainer = NULL;
			//m_nFlag = enmIndexFlag_Free;
			m_nPrev = enmInvalidObjectID;
			m_nNext = enmInvalidObjectID;
			m_activeState = enmActiveState_Unused;

			memset(&m_conflictData.key, 0, sizeof(m_conflictData.key));
			m_conflictData.nNextConflict = enmInvalidObjectID;

			return S_OK;
		}
		//�ָ���������
		virtual int32_t Resume()
		{
			return S_OK;
		}
		//ע����������
		virtual int32_t Uninitialize()
		{
			//m_nFlag = enmIndexFlag_Free;
			m_nPrev = enmInvalidObjectID;
			m_nNext = enmInvalidObjectID;
			m_activeState = enmActiveState_Unused;

			memset(&m_conflictData.key, 0, sizeof(m_conflictData.key));
			m_conflictData.nNextConflict = enmInvalidObjectID;

			return S_OK;
		}

	public:
		//��ȡ��һ����ͻ�ڵ�
		CIndex* NextConflict() const
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

		//��ȡ��Ծ״̬
		ActiveState GetActiveState()
		{
			return m_activeState;
		}

		CIndex* Prev()
		{
			if (enmInvalidObjectID == m_nPrev)
			{
				return NULL;
			}
			return &m_pContainer->m_arrIndex[m_nPrev];
		}
		//��ȡ����ĺ�һ�ڵ�
		CIndex* Next()
		{
			if (enmInvalidObjectID == m_nNext)
			{
				return NULL;
			}
			return &m_pContainer->m_arrIndex[m_nNext];
		}

		//��ȡ����
		T& GetObject()
		{
			return m_conflictData.object.GetValue();
		}

		//��ȡ����2(�������error C2039: 'GetObjectW' : is not a member of 'CCache<T,CAPACITY,BUCKETSIZE>::CIndex')
		T& Object()
		{
			return m_conflictData.object.GetValue();
		}

		//���ö���
		void SetObject(T& object)
		{
			m_conflictData.object.SetValue(object);
		}

		//��ȡ����ID
		int32_t Index()
		{
			return m_nIndex;
		}

	protected:
		//���ø�����ָ��
		void SetContainer(CCache* pContainer)
		{
			m_pContainer = pContainer;
		}
		////���ýڵ�Ϊ��ʹ��
		//void SetUsed()
		//{
		//	m_nFlag = enmIndexFlag_Used;
		//}
		////���ýڵ�Ϊ����
		//void SetFree()
		//{
		//	m_nFlag = enmIndexFlag_Free;
		//}
		//�ڵ��Ƿ���ʹ��
		bool IsUsed() const
		{
			return ((enmActiveState_Active == m_activeState) || (enmActiveState_Inactive == m_activeState));
		}

		//��ȡ����ĺ�һ�ڵ�
		int32_t GetNext() const
		{
			return m_nNext;
		}
		//��ȡ�����ǰһ�ڵ�
		int32_t GetPrev() const
		{
			return m_nPrev;
		}
		//��������ĺ�һ�ڵ�
		void SetNext(int32_t nNext)
		{
			m_nNext = nNext;
		}
		//���������ǰһ�ڵ�
		void SetPrev(int32_t nPrev)
		{
			m_nPrev = nPrev;
		}

		//���ö���ID
		void SetIndex(int32_t nIndex)
		{
			m_nIndex = nIndex;
		}

		//���ýڵ��ֵ
		void SetKey(Key key)
		{
			memcpy(&m_conflictData.key, &key, sizeof(Key));
		}

		//���ýڵ�״̬
		void SetActiveState(ActiveState activeState)
		{
			m_activeState = activeState;
		}

		//��ȡ��һ����ͻ�ڵ�
		int32_t GetNextConflict()
		{
			return m_conflictData.nNextConflict;
		}
		//������һ����ͻ�ڵ�
		void SetNextConflict(int32_t nNextConflict)
		{
			m_conflictData.nNextConflict = nNextConflict;
		}

		//���ó�ͻ���ڵ�����
		void ResetConflictData()
		{
			memset(&m_conflictData.key, 0, sizeof(m_conflictData.key));
			m_conflictData.nNextConflict = enmInvalidObjectID;
		}

	protected:
		CCache*		m_pContainer;

		//uint32_t		m_nFlag;		//����״̬��־
		int32_t			m_nIndex;		//����������
		int32_t			m_nNext;		//��һ�ڵ�
		int32_t			m_nPrev;		//ǰһ�ڵ�
		ActiveState		m_activeState;	//��Ծ״̬

		ConflictData	m_conflictData;	//�����ĳ�ͻ��Ԫ��

		friend class CCache;
	};


public:
	CCache()
	{
	}
	virtual ~CCache()
	{
	}

public:
	//��ʼ����ϣ��
	virtual int32_t Initialize()
	{
		m_nFreeCount = CAPACITY;
		m_nFreeHead = 0;
		m_nFreeTail = CAPACITY - 1;

		m_nActiveCount = 0;
		m_nActiveHead = enmInvalidObjectID;
		m_nActiveTail = enmInvalidObjectID;

		m_nInactiveCount = 0;
		m_nInactiveHead = enmInvalidObjectID;
		m_nInactiveTail = enmInvalidObjectID;

		for (uint32_t i = 0; i < CAPACITY; ++i)
		{
			m_arrIndex[i].Initialize();
		}

		for (uint32_t i = 0; i < BUCKETSIZE; ++i)
		{
			m_arrActiveBucket[i].nHeadConflict = enmInvalidObjectID;
			m_arrInactiveBucket[i].nHeadConflict = enmInvalidObjectID;
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
		m_nFreeCount = CAPACITY;
		m_nFreeHead = 0;
		m_nFreeTail = CAPACITY - 1;

		m_nActiveCount = 0;
		m_nActiveHead = enmInvalidObjectID;
		m_nActiveTail = enmInvalidObjectID;

		m_nInactiveCount = 0;
		m_nInactiveHead = enmInvalidObjectID;
		m_nInactiveTail = enmInvalidObjectID;

		for (uint32_t i = 0; i < CAPACITY; ++i)
		{
			m_arrIndex[i].Uninitialize();
		}

		for (uint32_t i = 0; i < BUCKETSIZE; ++i)
		{
			m_arrActiveBucket[i].nHeadConflict = enmInvalidObjectID;
			m_arrInactiveBucket[i].nHeadConflict = enmInvalidObjectID;
		}

		BuildIndexList();

		return S_OK;
	}

public:
	//��Cache�д�������
	CIndex* CreateObject(Key key)
	{
		CIndex* pIndex = NULL;

		//����ֵ�Ƿ��ͻ
		pIndex = Find(key);
		if (NULL != pIndex)
		{
			return NULL;
		}
		//��Ծ�б�����
		if (m_nActiveCount >= enmMaxActiveObjectCount)
		{
			MoveFirstActiveToInactive();
		}
		//��������
		pIndex = NewActiveIndex(key);
		if (NULL == pIndex)
		{
			return NULL;
		}
		////�������
		//AddToBucket(pIndex, enmActiveState_Active, key);

		return pIndex;
	}
	//��Cache��ɾ������
	int32_t DeleteObject(Key key)
	{
		CIndex* pIndex = DeleteFromBucket(enmActiveState_Active, key);
		if (NULL == pIndex)
		{
			pIndex = DeleteFromBucket(enmActiveState_Inactive, key);
			if (NULL == pIndex)
			{
				return E_OBJECTNOTEXIST;
			}
		}
		//������ӻ�Ծ���ƶ���������
		ActiveState activeState = pIndex->GetActiveState();
		int32_t ret = MoveIndex(pIndex, activeState, enmActiveState_Unused);
		if (0 > ret)
		{
			return ret;
		}
		return S_OK;
	}
	//��cache��ɾ������
	int32_t DeleteObject(CIndex* pIndex)
	{
		int32_t ret = CheckIndex(pIndex);
		if (0 > ret)
		{
			return ret;
		}

		return DeleteObject(pIndex->GetKey());
	}
	//������ж���
	void Clear()
	{
		Uninitialize();
	}
	//��ȡ����
	CIndex* FindObject(Key key)
	{
		CIndex* pIndex = Find(key);
		if (NULL == pIndex)
		{
			return NULL;
		}
		if (enmActiveState_Inactive == pIndex->GetActiveState())
		{
			int32_t ret = ActivateObject(pIndex);
			if (0 > ret)
			{
				//
			}
		}
		return pIndex;
	}
	//�������
	int32_t ActivateObject(CIndex* pIndex)
	{
		int32_t ret = CheckIndex(pIndex);
		if (0 > ret)
		{
			return ret;
		}
		if (enmActiveState_Active == pIndex->GetActiveState())
		{
			return S_OK;
		}
		if (enmActiveState_Inactive != pIndex->GetActiveState())
		{
			return E_NOTINACTIVE;
		}
		//��ʱ�ƶ����������������ٶ������ݣ�
		ret = MoveIndex(pIndex, enmActiveState_Inactive, enmActiveState_Unused, false);
		if (0 > ret)
		{
			return ret;
		}
		Key key = pIndex->GetKey();
		//ɾ����������
		DeleteFromBucket(enmActiveState_Inactive, key);
		//ȷ����Ծ���Ƿ�����
		if (enmMaxActiveObjectCount <= m_nActiveCount)
		{
			MoveFirstActiveToInactive();
		}
		//�������ƶ�����Ծ��
		ret = MoveIndex(pIndex, enmActiveState_Unused, enmActiveState_Active, false);
		if (0 > ret)
		{
			return ret;
		}
		//��Ӷ�������
		AddToBucket(pIndex, enmActiveState_Active, key);

		return S_OK;
	}

	//��ȡ��Ծͷ����������
	CIndex* FirstActive()
	{
		return (enmInvalidObjectID == m_nActiveHead) ? NULL : &m_arrIndex[m_nActiveHead];
	}
	//��ȡ��Ծβ����������
	CIndex* LastActive()
	{
		return (enmInvalidObjectID == m_nActiveTail) ? NULL : &m_arrIndex[m_nActiveTail];
	}
	//��ȡ�ǻ�Ծͷ����������
	CIndex* FirstInactive()
	{
		return (enmInvalidObjectID == m_nInactiveHead) ? NULL : &m_arrIndex[m_nInactiveHead];
	}
	//��ȡ�ǻ�Ծβ����������
	CIndex* LastInactive()
	{
		return (enmInvalidObjectID == m_nInactiveTail) ? NULL : &m_arrIndex[m_nInactiveTail];
	}

	//��ȡ���������
	uint32_t Capacity() const
	{
		return CAPACITY;
	}
	//��ȡ������ж�������
	uint32_t ObjectCount() const
	{
		return m_nActiveCount + m_nInactiveCount;
	}
	//��ȡ������ж�������
	uint32_t ActiveCount() const
	{
		return m_nActiveCount;
	}
	//��ȡ������ж�������
	uint32_t InactiveCount() const
	{
		return m_nInactiveCount;
	}

	//������Ƿ�Ϊ��
	bool IsEmpty() const
	{
		return (0 == (m_nActiveCount + m_nInactiveCount));
	}
	//������Ƿ�����
	bool IsFull() const
	{
		return ((m_nActiveCount + m_nInactiveCount) == CAPACITY);
	}

	//����λ�û�ȡ����
	CIndex* GetIndex(const int32_t nIndex)
	{
		int32_t ret = CheckIndex(nIndex);
		if (0 > ret)
		{
			return NULL;
		}
		return &m_arrIndex[nIndex];
	}
	//����λ�û�ȡ����
	int32_t GetIndex(const CIndex* pIndex)
	{
		int32_t ret = CheckIndex(pIndex);
		if (0 > ret)
		{
			return enmInvalidObjectID;
		}
		return (int32_t)(((uint8_t*)pIndex - (uint8_t*)m_arrIndex) / sizeof(CIndex));
	}

protected:
	//����Ծ���еĵ�һ���ƶ����ǻ�Ծ��
	void MoveFirstActiveToInactive()
	{
		if (enmInvalidObjectID == m_nActiveHead)
		{
			return;
		}
		//�ǻ�Ծ�б�����
		if (m_nInactiveCount >= enmMaxInactiveObjectCount)
		{
			DeleteFirstInactive();
		}
		CIndex* pIndex = &m_arrIndex[m_nActiveHead];
		int32_t ret = MoveActiveToInactive(pIndex);
		if (0 > ret)
		{
			//
		}
	}
	//ɾ���ǻ�Ծ���еĵ�һ��
	void DeleteFirstInactive()
	{
		if (enmInvalidObjectID == m_nInactiveHead)
		{
			return;
		}
		CIndex* pIndex = &m_arrIndex[m_nInactiveHead];
		int32_t ret = FreeIndex(pIndex);
		if (0 > ret)
		{
			//
		}
	}

protected:
	//�½���Ծ����
	CIndex* NewActiveIndex(Key key)
	{
		//��Ծ������
		if (enmMaxActiveObjectCount <= m_nActiveCount)
		{
			return NULL;
		}
		//�ӿ�������ͷ��ȡ��һ��������
		if (enmInvalidObjectID == m_nFreeHead)
		{
			return NULL;
		}
		CIndex* pIndex = &m_arrIndex[m_nFreeHead];

		//������ӿ������ƶ�����Ծ��
		int32_t ret = MoveIndex(pIndex, enmActiveState_Unused, enmActiveState_Active);
		if (0 > ret)
		{
			return NULL;
		}
		//��ӻ�Ծ��������
		AddToBucket(pIndex, enmActiveState_Active, key);

		return pIndex;
	}
	//�½��ǻ�Ծ����
	CIndex* NewInactiveIndex(Key key)
	{
		//�ǻ�Ծ������
		if (enmMaxInactiveObjectCount <= m_nInactiveCount)
		{
			return NULL;
		}
		//�ӿ�������ͷ��ȡ��һ��������
		if (enmInvalidObjectID == m_nFreeHead)
		{
			return NULL;
		}
		CIndex* pIndex = &m_arrIndex[m_nFreeHead];

		//������ӿ������ƶ����ǻ�Ծ��
		int32_t ret = MoveIndex(pIndex, enmActiveState_Unused, enmActiveState_Inactive);
		if (0 > ret)
		{
			return NULL;
		}
		//��ӷǻ�Ծ��������
		AddToBucket(pIndex, enmActiveState_Inactive, key);

		return pIndex;
	}
	//���ٻ�Ծ����
	int32_t FreeIndex(CIndex* pIndex)
	{
		int32_t ret = CheckIndex(pIndex);
		if (0 > ret)
		{
			return ret;
		}
		//������ӻ�Ծ���ƶ���������
		Key key = pIndex->GetKey();
		ActiveState activeState = pIndex->GetActiveState();
		ret = MoveIndex(pIndex, activeState, enmActiveState_Unused);
		if (0 > ret)
		{
			return ret;
		}
		//ɾ����Ծ��������
		DeleteFromBucket(activeState, key);

		return S_OK;
	}
	//���ݼ�ֵ���Ҷ���
	CIndex* Find(Key key)
	{
		CIndex* pIndex = FindInBucket(enmActiveState_Active, key);
		if (NULL == pIndex)
		{
			pIndex = FindInBucket(enmActiveState_Inactive, key);
		}

		return pIndex;
	}
	//����Ծ��������Ϊ����Ծ����
	int32_t MoveActiveToInactive(CIndex* pIndex)
	{
		//�ǻ�Ծ������
		if (enmMaxInactiveObjectCount <= m_nInactiveCount)
		{
			return E_INACTIVEISFULL;
		}
		//������ӻ�Ծ���ƶ����ǻ�Ծ��
		int32_t ret = MoveIndex(pIndex, enmActiveState_Active, enmActiveState_Inactive, false);
		if (0 > ret)
		{
			return ret;
		}
		Key key = pIndex->GetKey();
		//ɾ����Ծ��������
		DeleteFromBucket(enmActiveState_Active, key);
		//��ӷǻ�Ծ��������
		AddToBucket(pIndex, enmActiveState_Inactive, key);

		return S_OK;
	}
	//���Ծ��������Ϊ��Ծ����
	void MoveInactiveToActive(CIndex* pIndex)
	{
		//�ǻ�Ծ������
		if (enmMaxActiveObjectCount <= m_nActiveCount)
		{
			return E_ACTIVEISFULL;
		}
		//������ӷǻ�Ծ���ƶ�����Ծ��
		int32_t ret = MoveIndex(pIndex, enmActiveState_Inactive, enmActiveState_Active, false);
		if (0 > ret)
		{
			return ret;
		}
		Key key = pIndex->GetKey();
		//ɾ���ǻ�Ծ��������
		DeleteFromBucket(enmActiveState_Inactive, key);
		//��ӻ�Ծ��������
		AddToBucket(enmActiveState_Active, key);

		return S_OK;
	}

protected:
	int32_t BuildIndexList()
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

	//�����ƶ�
	int32_t MoveIndex(CIndex* pIndex, ActiveState fromState, ActiveState toState, bool bInit = true)
	{
		int32_t ret = CheckIndex(pIndex, true);
		if (0 > ret)
		{
			return ret;
		}
		if (fromState != pIndex->GetActiveState())
		{
			return E_ACTIVESTATE;
		}
		if (fromState == toState)
		{
			return S_FALSE;
		}

		//Ϊ�˱���ͳһ��������ȡ������Ա������ָ��
		uint32_t *pFromCount = GetCountPointer(fromState);
		int32_t *pFromHead = GetHeadPointer(fromState);
		int32_t *pFromTail = GetTailPointer(fromState);
		uint32_t *pToCount = GetCountPointer(toState);
		int32_t *pToHead = GetHeadPointer(toState);
		int32_t *pToTail = GetTailPointer(toState);

		//����ԭ���������ͷ����β��ָ��
		if (*pFromHead == pIndex->Index())
		{
			*pFromHead = pIndex->GetNext();
		}
		if (*pFromTail == pIndex->Index())
		{
			*pFromTail = pIndex->GetPrev();
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

		//��������ӵ�����������β��
		pIndex->SetPrev(*pToTail);
		pIndex->SetNext(enmInvalidObjectID);
		if (enmInvalidObjectID != *pToTail)
		{
			m_arrIndex[*pToTail].SetNext(pIndex->Index());
		}
		*pToTail = pIndex->Index();
		if (enmInvalidObjectID == *pToHead)
		{
			*pToHead = *pToTail;
		}

		pIndex->SetActiveState(toState);

		--(*pFromCount);
		++(*pToCount);

		//�Ƿ�Զ�����г�ʼ������
		if (bInit)
		{
			if (enmActiveState_Unused == fromState)
			{
				pIndex->m_conflictData.object.Initialize();
			}
			else if (enmActiveState_Unused == toState)
			{
				pIndex->m_conflictData.object.Uninitialize();
			}
		}

		return S_OK;
	}
	//�������
	void AddToBucket(CIndex* pIndex, ActiveState activeState, Key key)
	{
		Bucket *pBucket = (activeState == enmActiveState_Active) ? m_arrActiveBucket : m_arrInactiveBucket;

		//�����ϣֵ
		uint32_t nHashCode = GetHashCode(key);
		uint32_t nBucketIndex = nHashCode % BUCKETSIZE;

		if (enmInvalidObjectID == pBucket[nBucketIndex].nHeadConflict)
		{
			//���½ڵ���뵽��ͻ��ͷ��
			pBucket[nBucketIndex].nHeadConflict = pIndex->Index();
		}
		else
		{
			int32_t nTailConflict = pBucket[nBucketIndex].nHeadConflict;
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
	}
	//ɾ������
	CIndex* DeleteFromBucket(ActiveState activeState, Key key)
	{
		Bucket *pBucket = (activeState == enmActiveState_Active) ? m_arrActiveBucket : m_arrInactiveBucket;

		//�����ϣֵ
		uint32_t nHashCode = GetHashCode(key);
		uint32_t nBucketIndex = nHashCode % BUCKETSIZE;

		int32_t nPrevConflict = enmInvalidObjectID;
		int32_t nConflict = pBucket[nBucketIndex].nHeadConflict;
		if (enmInvalidObjectID == nConflict)
		{
			return NULL;
		}
		//������ָ����ֵ��ȵ����нڵ�
		while (enmInvalidObjectID != nConflict)
		{
			if (IsEqual(key, m_arrIndex[nConflict].GetKey()))
			{
				if (enmInvalidObjectID == nPrevConflict)
				{
					pBucket[nBucketIndex].nHeadConflict = m_arrIndex[nConflict].GetNextConflict();
				}
				else
				{
					m_arrIndex[nPrevConflict].SetNextConflict(m_arrIndex[nConflict].GetNextConflict());
				}
				m_arrIndex[nConflict].ResetConflictData();
				return &m_arrIndex[nConflict];
			}
			nPrevConflict = nConflict;
			nConflict = m_arrIndex[nConflict].GetNextConflict();
		}

		return NULL;
	}
	//���ݼ�ֵ���Ҷ���
	CIndex* FindInBucket(ActiveState activeState, Key key)
	{
		Bucket *pBucket = (activeState == enmActiveState_Active) ? m_arrActiveBucket : m_arrInactiveBucket;

		//�����ϣֵ
		uint32_t nHashCode = GetHashCode(key);
		uint32_t nBucketIndex = nHashCode % BUCKETSIZE;

		int32_t nConflict = pBucket[nBucketIndex].nHeadConflict;
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

	//�����ϣֵ
	uint32_t GetHashCode(Key key)
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
	bool IsEqual(const Key keySrc, const Key keyDest)
	{
		return ((keySrc.nKeyLength == keyDest.nKeyLength) && (0 == memcmp(keySrc.arrKey, keyDest.arrKey, keySrc.nKeyLength)));
	}

	//ȷ����������ָ���Ƿ���Ч
	int32_t CheckIndex(const CIndex* pIndex, bool bFreeIsOK = false)
	{
		if ((pIndex < m_arrIndex) || (pIndex >= m_arrIndex + CAPACITY))
		{
			return E_INVALIDOBJECTINDEX;
		}
		if (((uint8_t*)pIndex - (uint8_t*)m_arrIndex) % sizeof(CIndex) != 0)
		{
			return E_INVALIDOBJECTINDEX;
		}
		if (!bFreeIsOK && !pIndex->IsUsed())
		{
			return E_OBJECTINDEXNOTUSED;
		}
		return S_OK;
	}
	//ȷ�����������Ƿ���Ч
	int32_t CheckIndex(const int32_t nIndex, bool bFreeIsOK = false)
	{
		if ((0 > nIndex) || ((int32_t)CAPACITY < nIndex))
		{
			return E_INVALIDOBJECTINDEX;
		}
		if (!bFreeIsOK && !m_arrIndex[nIndex].IsUsed())
		{
			return E_OBJECTINDEXNOTUSED;
		}
		return S_OK;
	}

	//��ȡ����ָ��
	uint32_t* GetCountPointer(ActiveState activeState)
	{
		switch (activeState)
		{
		case enmActiveState_Active:
			return &m_nActiveCount;
		case enmActiveState_Inactive:
			return &m_nInactiveCount;
		default:
			return &m_nFreeCount;
		}
	}
	//��ȡ����ͷָ��
	int32_t* GetHeadPointer(ActiveState activeState)
	{
		switch (activeState)
		{
		case enmActiveState_Active:
			return &m_nActiveHead;
		case enmActiveState_Inactive:
			return &m_nInactiveHead;
		default:
			return &m_nFreeHead;
		}
	}
	//��ȡ����βָ��
	int32_t* GetTailPointer(ActiveState activeState)
	{
		switch (activeState)
		{
		case enmActiveState_Active:
			return &m_nActiveTail;
		case enmActiveState_Inactive:
			return &m_nInactiveTail;
		default:
			return &m_nFreeTail;
		}
	}

protected:
	uint32_t		m_nFreeCount;		//δ�������ĸ���
	int32_t			m_nFreeHead;		//δ���������ͷ�����������
	int32_t			m_nFreeTail;		//δ���������β�����������

	uint32_t		m_nActiveCount;		//��Ծ��������
	int32_t			m_nActiveHead;		//��Ծ������ͷ�����������
	int32_t			m_nActiveTail;		//��Ծ������β�����������

	uint32_t		m_nInactiveCount;	//�ǻ�Ծ��������
	int32_t			m_nInactiveHead;	//�ǻ�Ծ������ͷ�����������
	int32_t			m_nInactiveTail;	//�ǻ�Ծ������β�����������

	CIndex			m_arrIndex[CAPACITY];	//����������

	Bucket			m_arrActiveBucket[BUCKETSIZE];//��Ծ����Ͱ
	Bucket			m_arrInactiveBucket[BUCKETSIZE];//�ǻ�Ծ����Ͱ
};


#endif /* COMMON_CACHE_H_ */
