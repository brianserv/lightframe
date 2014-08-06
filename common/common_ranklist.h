/*
 * common_ranklist.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_RANKLIST_H_
#define COMMON_RANKLIST_H_

#include <stdio.h>
#include <memory.h>
#include "common_typedef.h"
#include "common_errordef.h"
#include "common_object.h"
#include "common_hashlist.h"
#include "common_pool.h"
#include "common_datetime.h"


//���ж���ʵ�����ο�)
class CRankObject
{
public:
	CRankObject(){}
	~CRankObject(){}

	int32_t GetTargetID(){return nTargetID;}
	int32_t GetRank(){return nRank;}
	void SetRank(int32_t rank) {nRank = rank;}

	bool operator < (const CRankObject& obj) const
	{
		return nValue < obj.nValue;
	}

	bool operator == (const CRankObject& obj) const
	{
		return nValue == obj.nValue;
	}

	void operator = (const CRankObject& obj)
	{
		nTargetID = obj.nTargetID;
		nValue = obj.nValue;
		nRank = obj.nRank;
	}
public:
	int32_t nTargetID;
	int32_t	nRank;
	int32_t nValue;
	int32_t nLastRankUpdateTime;
};

//����һ�������б�ģ����
template<typename T, uint32_t CAPACITY>
class CRankList : public CObject
{
public:

	enum
	{
		enmInvalidObjectID				= -1,		//��Ч�Ķ�������ID
		enmIndexFlag_Free				= 0,		//������������״̬
		enmIndexFlag_Used				= 1,		//����������ʹ��״̬
	};

	typedef CHashList<int32_t, CAPACITY, CAPACITY*8>					CTargetHashList;	//�����б�
	typedef typename CHashList<int32_t, CAPACITY, CAPACITY*8>::CIndex 	CHashIndex;
	typedef typename CHashList<int32_t, CAPACITY, CAPACITY*8>::Key		Key;

	class CIndex
	{
	public:
		CIndex()
		{
		}
		~CIndex()
		{
		}

		inline T& Object()
		{
			return m_object;
		}
		inline T* ObjectPtr()
		{
			return (T*)&m_object;
		}

		//���ö���
		inline void SetObject(T& object)
		{
			m_object = object;
		}

		//��ȡ����ID
		inline int32_t Index()
		{
			return m_nIndex;
		}

	protected:
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

		//���ö���ID
		inline void SetIndex(int32_t nIndex)
		{
			m_nIndex = nIndex;
		}
	protected:
		int32_t			m_nFlag;
		int32_t			m_nIndex;
		T				m_object;

		friend class CRankList;
	};

public:
	CRankList()
	{
	}
	virtual ~CRankList()
	{
	}

public:
	//��ʼ�������б�
	virtual int32_t Initialize()
	{
		m_nObjectCount = 0;
		m_TargetHashList.Initialize();

		for(uint32_t i = 0; i < CAPACITY; i++)
		{
			m_arrIndex[i].SetIndex(i);
		}

		m_nLastChangeTime = (int32_t)CDateTime::CurrentDateTime().Seconds();
		return S_OK;
	}
	//�ָ������б�
	virtual int32_t Resume()
	{
		return Initialize();
	}
	//ע�������б�
	virtual int32_t Uninitialize()
	{
		m_TargetHashList.Uninitialize();
		m_nObjectCount = 0;
		return S_OK;
	}

public:
	//�������ж���
	int32_t Append(T& object)
	{
		if(IsFull())
		{
			//��������һ����ҪС������
			CIndex *pLast = Last();
			if(pLast)
			{
				T& lastObj = pLast->Object();
				if( object < lastObj || object == lastObj)
				{
					return S_FALSE;
				}
			}

			//ɾ�����һ������
			int32_t ret = EraseLast();
			if(0 > ret)
			{
				return E_UNKNOWN;
			}
		}

		CIndex *pIndex = InsertLast(object);
		if(NULL == pIndex)
		{
			return E_UNKNOWN;
		}

		//���´���
		pIndex = Update(pIndex);
		return S_OK;
	}

	//���ݼ�ֵ���Ҷ���
	int32_t Find(const int32_t nTargetID, T* &object)
	{
		CIndex *pIndex = NULL;
		int32_t ret = Find(nTargetID, pIndex);
		if(0 > ret)
		{
			return ret;
		}
		object = &(pIndex->Object());
		return S_OK;
	}

	//��ȡǰһ������(������ͬ)
	int32_t FindPrev(const int32_t nTargetID, T* &object)
	{
		object = NULL;
		CIndex *pIndex = NULL;
		int32_t ret = Find(nTargetID, pIndex);
		if(0 > ret)
		{
			return ret;
		}

		CIndex *pPrev = GetPrev(pIndex);
		while(pPrev)
		{
			if(pPrev->Object().GetRank() != pIndex->Object().GetRank())
			{
				break;
			}

			pPrev = GetPrev(pPrev);
		}

		if(NULL == pPrev)
		{
			return E_UNKNOWN;
		}
		object = pPrev->ObjectPtr();
		return S_OK;
	}

	//�������ж���
	int32_t Update(const int32_t nTargetID)
	{
		CIndex *pIndex = NULL;
		int32_t ret = Find(nTargetID, pIndex);
		if(0 > ret)
		{
			return ret;
		}

		//���´���
		pIndex = Update(pIndex);
		//������Ҳ�п��ܵ�����������
		Rerank(pIndex);
		return S_OK;
	}

	//��ȡ�����б�
	int32_t GetRankList(T arrTargetID[], int32_t begin, int32_t max, int32_t &count)
	{
		count = 0;
		if(begin < 0)
		{
			return E_UNKNOWN;
		}

		if( begin >= m_nObjectCount)
		{
			return S_OK;
		}

		for(int32_t i = begin; i < m_nObjectCount; i++)
		{
			arrTargetID[count] = m_arrIndex[i].Object();
			count++;

			if(count >= max)
			{
				break;
			}
		}

		return S_OK;
	}

	//��ȡ��Ȼ���
	int32_t GetNormalRank(const int32_t nTargetID)
	{
		CIndex *pIndex = NULL;
		int32_t ret = Find(nTargetID, pIndex);
		if(0 > ret)
		{
			return -1;
		}
		return pIndex->Index();
	}

	//������ж���
	void Clear()
	{
		for(int32_t i = 0; i < m_nObjectCount; i++)
		{
			CIndex *pIndex = &m_arrIndex[i];
			pIndex->SetIndex(i);
			pIndex->SetFree();
		}

		//���Hash��
		m_TargetHashList.Clear();

		m_nObjectCount = 0;

		m_nLastChangeTime = (int32_t)CDateTime::CurrentDateTime().Seconds();
	}

	//��ȡ������ж�������
	uint32_t ObjectCount() const
	{
		return m_nObjectCount;
	}

	//��ȡ���һ��
	int32_t GetLastRank(T& object)
	{
		CIndex *pIndex = Last();
		if(pIndex)
		{
			object = pIndex->Object();
			return S_OK;
		}

		return E_UNKNOWN;
	}

	//��ȡ��һ��
	int32_t GetFirstRank(T& object)
	{
		CIndex *pIndex = First();
		if(pIndex)
		{
			object = pIndex->Object();
			return S_OK;
		}
		return E_UNKNOWN;
	}

	int32_t GetLastChangeTime()
	{
		return m_nLastChangeTime;
	}
protected:
	//��ĩβ�������
	CIndex* InsertLast(T& object)
	{
		//β���������
		int32_t nIndex = m_nObjectCount;
		CIndex *pIndex = &m_arrIndex[nIndex];
		m_nObjectCount ++;

		//����hash��
		Key key = MakeKey(object.GetTargetID());
		m_TargetHashList.Insert(key, nIndex);

		pIndex->SetObject(object);
		pIndex->SetUsed();

		//��������ֵ(�����ǰһ�ڵ���ȣ���ʹ��ǰһ�ڵ�����)
		pIndex->Object().SetRank(nIndex+1);

		CIndex *pPrev = GetPrev(pIndex);
		if((NULL != pPrev) && (pPrev->Object() == pIndex->Object()))
		{
			pIndex->Object().SetRank(pPrev->Object().GetRank());
		}

		m_nLastChangeTime = (int32_t)CDateTime::CurrentDateTime().Seconds();
		return pIndex;
	}

	//ɾ�����ж���
	int32_t EraseLast()
	{
		CIndex *pIndex = Last();
		if(NULL == pIndex)
		{
			return S_OK;
		}

		T &obj = pIndex->Object();

		//��HashList��ɾ��
		Key key = MakeKey(obj.GetTargetID());
		m_TargetHashList.Erase(key);

		//ִ��������
		pIndex->SetFree();

		m_nObjectCount--;
		return S_OK;
	}

	//���ݼ�ֵ���Ҷ���
	int32_t Find(const int32_t nTargetID, CIndex* &pTarget)
	{
		//HashListȡ��������
		Key key = MakeKey(nTargetID);
		CHashIndex* pHashIndex = m_TargetHashList.Find(key);
		if(NULL == pHashIndex)
		{
			return E_UNKNOWN;
		}

		int32_t nObjectIndex = pHashIndex->Object();
		if(nObjectIndex < 0 || nObjectIndex >= m_nObjectCount)
		{
			return E_UNKNOWN;
		}

		pTarget = &m_arrIndex[nObjectIndex];

		return S_OK;
	}

	//�ƶ�����
	CIndex* Update(CIndex *pIndex)
	{
		T &object = pIndex->Object();

		//��ǰ��ĶԱ�
		CIndex *pPrev = GetPrev(pIndex);
		if(pPrev)
		{
			T& objPrev = pPrev->Object();
			if(objPrev < object)
			{
				CIndex *pNewIndex = SwapUp(pIndex);
				if(pNewIndex)
				{
					return Update(pNewIndex);
				}
				return pIndex;
			}
		}

		//������ĶԱ�
		CIndex *pNext = GetNext(pIndex);
		if(pNext)
		{
			T& objNext = pNext->Object();
			if(object < objNext)
			{
				CIndex * pNewIndex = SwapDown(pIndex);
				if(pNewIndex)
				{
					return Update(pNewIndex);
				}
			}
			return pIndex;
		}
		return pIndex;
	}

	//��ȡ���һ��
	inline CIndex* Last()
	{
		return (m_nObjectCount <= 0) ? NULL : &m_arrIndex[m_nObjectCount-1];
	}

	//��ȡ���һ��
	inline CIndex* First()
	{
		return (m_nObjectCount <= 0) ? NULL : &m_arrIndex[0];
	}

	//��ȡ��һ���ڵ�
	inline CIndex * GetNext(CIndex *pIndex)
	{
		int32_t nIndex = pIndex->Index();
		if(nIndex >= (m_nObjectCount -1))
		{
			return NULL;
		}

		return &m_arrIndex[nIndex + 1];
	}

	//��ȡǰһ���ڵ�
	inline CIndex * GetPrev(CIndex *pIndex)
	{
		int32_t nIndex = pIndex->Index();
		if(nIndex <= 0)
		{
			return NULL;
		}

		return &m_arrIndex[nIndex - 1];
	}

	//��ȡ���������
	inline uint32_t Capacity() const
	{
		return CAPACITY;
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

	void Rerank(CIndex *pIndex)
	{
		T& object = pIndex->Object();

		int32_t oldRank = object.GetRank();

		//�����Լ���Ӧ�õ�����
		object.SetRank(pIndex->Index() + 1);

		//�Ƿ���Ҫ��ǰһ�ڵ�ϲ�
		CIndex *pPrev = GetPrev(pIndex);
		if(NULL != pPrev)
		{
			if(pPrev->Object() == object)
			{
				object.SetRank(pPrev->Object().GetRank());
			}
		}

		//����Ľڵ��Ƿ���Ҫ���Լ��ϲ�
		CIndex *pNext = GetNext(pIndex);
		while(NULL != pNext)
		{
			T& nextObj = pNext->Object();
			if(nextObj == object)
			{
				nextObj.SetRank(object.GetRank());
			}
			else
			{
				//����������������
				if(nextObj.GetRank() != (pNext->Index()+1))
				{
					pPrev = GetPrev(pNext);
					if(pPrev && (nextObj == pPrev->Object()))
					{
						nextObj.SetRank(pPrev->Object().GetRank());
					}
					else
					{
						nextObj.SetRank(pNext->Index()+1);
					}
				}
				else
				{
					break;
				}
			}

			pNext = GetNext(pNext);
		}


		if(object.GetRank() != oldRank)
		{
			m_nLastChangeTime = (int32_t)CDateTime::CurrentDateTime().Seconds();
		}
	}
protected:
	//���½���
	CIndex* SwapDown(CIndex* pIndex)
	{
		//�������û�нڵ�
		CIndex *pNext = GetNext(pIndex);
		if(NULL == pNext)
		{
			return NULL;
		}

		//����2���ڵ�
		Swap(pIndex, pNext);

		//ˢ����
		Rerank(pIndex);
		Rerank(pNext);
		return pNext;
	}

	//���Ͻ���
	CIndex* SwapUp(CIndex* pIndex)
	{
		//���ǰ��û�нڵ�
		CIndex *pPrev = GetPrev(pIndex);
		if(NULL == pPrev)
		{
			return NULL;
		}

		//����2���ڵ�
		Swap(pIndex, pPrev);

		//ˢ����
		Rerank(pPrev);
		Rerank(pIndex);
		return pPrev;
	}

	//����2��λ��
	void Swap(CIndex *pA, CIndex *pB)
	{
		//�޸�Hash����
		Key keyA = MakeKey(pA->Object().GetTargetID());
		Key keyB = MakeKey(pB->Object().GetTargetID());
		CHashIndex *pHashIndexA = m_TargetHashList.Find(keyA);
		int32_t nIndexA = pHashIndexA->Object();

		CHashIndex *pHashIndexB = m_TargetHashList.Find(keyB);
		int32_t nIndexB = pHashIndexB->Object();

		pHashIndexA->SetObject(nIndexB);
		pHashIndexB->SetObject(nIndexA);

		//��������
		T obj = pA->Object();
		pA->SetObject(pB->Object());
		pB->SetObject(obj);
	}

	Key MakeKey(const int32_t nTargetID)
	{
		Key key ={0};
		key.nKeyLength = 4;
		*(int32_t*)key.arrKey = nTargetID;
		return key;
	}
protected:
	CIndex				m_arrIndex[CAPACITY];		//���б�
	CTargetHashList		m_TargetHashList;			//��������
	int32_t				m_nObjectCount;				//��Ч����
	int32_t				m_nLastChangeTime;			//���һ�����б䶯�¼�
};

#endif /* COMMON_RANKLIST_H_ */
