/*
 * common_heap.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_HEAP_H_
#define COMMON_HEAP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include "common_typedef.h"
#include "common_errordef.h"
#include "common_object.h"
#include "common_complextype.h"

//����һ���ѹ���ģ����
template<typename TKEY, typename T, uint32_t CAPACITY>
class CMinHeap: public CObject
{
public:
	enum
	{
		enmIndexFlag_Free				= 0,		//������������״̬
		enmIndexFlag_Used				= 1,		//����������ʹ��״̬
	};

	enum
	{
		enmInvalidHeapIndex = -1
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
			m_nFlag = enmIndexFlag_Free;
			m_pNext = NULL;
			m_pPrev = NULL;

			memset(&m_key, 0, sizeof(m_key));

			m_iHeapIndex = enmInvalidHeapIndex;

			m_object.Initialize();

			return S_OK;
		}
		//�ָ���������
		virtual int32_t Resume()
		{
			m_object.Resume();

			return S_OK;
		}
		//ע����������
		virtual int32_t Uninitialize()
		{
			m_nFlag = enmIndexFlag_Free;
			m_pNext = NULL;
			m_pPrev = NULL;

			memset(&m_key, 0, sizeof(m_key));

			m_object.Uninitialize();

			return S_OK;
		}

	public:

		//��ȡ��ֵ
		inline const TKEY& GetKey() const
		{
			return m_key;
		}
		//��ȡ����
		inline T& GetObject()
		{
			return m_object.GetValue();
		}
		//���ö���
		inline void SetObject(T& object)
		{
			m_object.SetValue(object);
		}

		inline int32_t & GetHeapIndex()
		{
			return m_iHeapIndex;
		}

		inline void SetHeapIndex(int32_t nIndex)
		{
			m_iHeapIndex = nIndex;
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

		//��ȡ����ĺ�һ�ڵ�
		inline CIndex* GetNext() const
		{
			return m_pNext;
		}
		//��ȡ�����ǰһ�ڵ�
		inline CIndex* GetPrev() const
		{
			return m_pPrev;
		}
		//��������ĺ�һ�ڵ�
		inline void SetNext(CIndex* pNext)
		{
			m_pNext = pNext;
		}
		//���������ǰһ�ڵ�
		inline void SetPrev(CIndex* pPrev)
		{
			m_pPrev = pPrev;
		}

		//���ü�ֵ
		inline void SetKey(TKEY key)
		{
			m_key = key;
		}
	protected:
		uint32_t		m_nFlag;		//����״̬��־

		CIndex*			m_pNext;		//��һ�ڵ㣬�����ڴ����
		CIndex*			m_pPrev;		//ǰһ�ڵ㣬�����ڴ����

		TKEY			m_key;			//��ֵ
		CComplexType<T>	m_object;		//�����Ķ���

		int32_t				m_iHeapIndex;

		friend class CMinHeap;
	};


public:
	CMinHeap()
	{
	}
	virtual ~CMinHeap()
	{
	}

public:
	//��ʼ����
	virtual int32_t Initialize()
	{
		m_nObjectCount = 0;
		m_nFreeCount = CAPACITY;
		m_pFreeHead = &m_arrIndex[0];
		m_pFreeTail = &m_arrIndex[CAPACITY - 1];
		m_pUsedHead = NULL;
		m_pUsedTail = NULL;

		for (uint32_t i = 0; i < CAPACITY; ++i)
		{
			m_arrIndex[i].Initialize();
		}

		BuildIndexList();

		return S_OK;
	}
	//�ָ���
	virtual int32_t Resume()
	{
		for (uint32_t i = 0; i < CAPACITY; ++i)
		{
			m_arrIndex[i].Resume();
		}
		return S_OK;
	}
	//ע����
	virtual int32_t Uninitialize()
	{
		m_nObjectCount = 0;
		m_nFreeCount = CAPACITY;
		m_pFreeHead = &m_arrIndex[0];
		m_pFreeTail = &m_arrIndex[CAPACITY - 1];
		m_pUsedHead = NULL;
		m_pUsedTail = NULL;

		for (uint32_t i = 0; i < CAPACITY; ++i)
		{
			m_arrIndex[i].Uninitialize();
		}

		BuildIndexList();

		return S_OK;
	}

public:
	//�������
	CIndex* Insert(TKEY key, T object)
	{
		CIndex* pNode = NULL;

		pNode = CreateIndex();
		if (NULL == pNode)
		{
			return NULL;
		}

		//����object
		pNode->SetObject(object);
		pNode->SetKey(key);

		//��Ҫ�����Ԫ�طŵ�����β��
		m_arrIndexPrt[m_nObjectCount - 1] = pNode;
		pNode->SetHeapIndex(m_nObjectCount - 1);

		//��β����ʼ���ϵ���
		ShiftUp(m_nObjectCount - 1);

		return pNode;
	}

	void Pop()
	{
		CIndex*	pIndex = NULL;
		if (IsEmpty())
		{
			return;
		}

		pIndex = m_arrIndexPrt[0];
		m_arrIndexPrt[0] = m_arrIndexPrt[m_nObjectCount - 1];
		m_arrIndexPrt[0]->SetHeapIndex(0);

		DestroyIndex(pIndex);

		if (IsEmpty())
		{
			return;
		}
		ShiftDown(0);
	}

	//ɾ������
	uint32_t Erase(TKEY key)
	{
		uint32_t		nIndex = 0;
		CIndex*	pIndex = NULL;

		for (nIndex = 0; nIndex < m_nObjectCount; ++nIndex)
		{
			if (m_arrIndexPrt[nIndex]->m_key == key)
			{
				pIndex = m_arrIndexPrt[nIndex];
				break;
			}
		}

		//û�з�����ָ��KEY
		if (NULL == pIndex)
		{
			return S_OK;
		}

		//����CIndex����, m_nObjectCount���1
		DestroyIndex(m_arrIndexPrt[nIndex]);


		if (IsEmpty())
		{
			return S_OK;
		}

		//���ɾ���������һ��Ԫ��,ע��m_nObjectCount�Ѿ���1
		if (nIndex == m_nObjectCount)
		{
			return S_OK;
		}

		//���������һ��Ԫ�طŵ�nIndexλ��
		m_arrIndexPrt[nIndex] = m_arrIndexPrt[m_nObjectCount - 1];


		//��nIndex��ʼ���µ���
		ShiftDown(nIndex);

		return S_OK;
	}
	//ɾ������
	uint32_t Erase(CIndex* pNode)
	{
		int32_t iHeapIndex = enmInvalidHeapIndex;

		if (IsEmpty())
		{
			return S_OK;
		}

		iHeapIndex = pNode->m_iHeapIndex;

		if (iHeapIndex < 0 || iHeapIndex >= m_nObjectCount)
		{
			return S_OK;
		}

		m_arrIndexPrt[iHeapIndex] = m_arrIndexPrt[m_nObjectCount - 1];
		m_arrIndexPrt[iHeapIndex]->m_iHeapIndex = iHeapIndex;

		DestroyIndex(pNode);

		ShiftDown(iHeapIndex);

		return S_OK;
	}
	//������ж���
	void Clear()
	{
		Uninitialize();
	}
	//���Ҷ���
	CIndex* Find(TKEY key)
	{
		return FindNode(key);
	}
	//��ȡ��С��������
	CIndex* First() const
	{
		if (IsEmpty())
		{
			return NULL;
		}

		return m_arrIndexPrt[0];
	}
	//��ȡ����������
	CIndex* Last() const
	{
		if (IsEmpty())
		{
			return NULL;
		}

		if (1 == ObjectCount())
		{
			return m_arrIndexPrt[0];
		}

		//���жѵ�����������Ǹ��������һ������ߵ�Ŀ���Ǳ�����һ��Ԫ�ء�
		RShiftDown(ObjectCount() - 1);

		return m_arrIndexPrt[ObjectCount() - 1];
	}

	//��ȡ���������
	uint32_t Capacity() const
	{
		return CAPACITY;
	}
	//��ȡ������ж�������
	uint32_t ObjectCount() const
	{
		return m_nObjectCount;
	}

	//������Ƿ�Ϊ��
	bool IsEmpty() const
	{
		return (0 == m_nObjectCount);
	}
	//������Ƿ�����
	bool IsFull() const
	{
		return (m_nObjectCount == CAPACITY);
	}


	void Debug()
	{
		printf("Index\tKey\tValue\n");
		for (int i = 0; i < ObjectCount(); ++i)
		{
			printf("%d\t%d\t%d\n",
				m_arrIndexPrt[i]->GetHeapIndex(),
				m_arrIndexPrt[i]->GetKey(),
				(int32_t)m_arrIndexPrt[i]->GetObject());
		}
	}
protected:
	//����������������
	int32_t BuildIndexList()
	{
		//���õ�0������
		m_arrIndex[0].SetPrev(NULL);

		for (uint32_t i = 1; i < CAPACITY; ++i)
		{
			m_arrIndex[i - 1].SetNext(&m_arrIndex[i]);
			m_arrIndex[i].SetPrev(&m_arrIndex[i - 1]);
		}

		//�������һ������
		m_arrIndex[CAPACITY - 1].SetNext(NULL);

		return S_OK;
	}

	//��������
	CIndex* CreateIndex()
	{
		//���������
		if (IsFull())
		{
			return NULL;
		}

		//�ӿ�������ͷ��ȡ��һ��������
		CIndex* pIndex = m_pFreeHead;
		if (NULL == pIndex)
		{
			return NULL;
		}

		//�����������������ͷ����β��ָ��
		if (m_pFreeTail == pIndex)
		{
			m_pFreeTail = pIndex->GetNext();
		}
		m_pFreeHead = pIndex->GetNext();
		if (NULL != m_pFreeHead)
		{
			m_pFreeHead->SetPrev(NULL);
		}

		//����������ӵ���ʹ����������β��
		pIndex->SetPrev(m_pUsedTail);
		pIndex->SetNext(NULL);
		if (NULL != m_pUsedTail)
		{
			m_pUsedTail->SetNext(pIndex);
		}
		m_pUsedTail = pIndex;
		if (NULL == m_pUsedHead)
		{
			m_pUsedHead = m_pUsedTail;
		}

		pIndex->SetUsed();

		++m_nObjectCount;
		--m_nFreeCount;

		return pIndex;
	}
	//���ٶ���
	int32_t DestroyIndex(CIndex* pIndex)
	{
		int32_t ret = CheckIndex(pIndex);
		if (ret < 0)
		{
			return ret;
		}

		//������ʹ�����������ͷ����β��ָ��
		if (m_pUsedHead == pIndex)
		{
			m_pUsedHead = pIndex->GetNext();
		}
		if (m_pUsedTail == pIndex)
		{
			m_pUsedTail = pIndex->GetPrev();
		}

		//���ڵ��������ɾ��
		if (NULL != pIndex->GetPrev())
		{
			pIndex->GetPrev()->SetNext(pIndex->GetNext());
		}
		if (NULL != pIndex->GetNext())
		{
			pIndex->GetNext()->SetPrev(pIndex->GetPrev());
		}

		//��������ӵ�������������β��
		pIndex->SetPrev(m_pFreeTail);
		pIndex->SetNext(NULL);
		if (NULL != m_pFreeTail)
		{
			m_pFreeTail->SetNext(pIndex);
		}
		m_pFreeTail = pIndex;
		if (NULL == m_pFreeHead)
		{
			m_pFreeHead = m_pFreeTail;
		}

		pIndex->SetFree();

		--m_nObjectCount;
		++m_nFreeCount;

		return S_OK;
	}

	//ȷ����������ָ���Ƿ���Ч
	int32_t CheckIndex(CIndex* pIndex)
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

	//����λ�û�ȡ����
	CIndex* GetIndex(int32_t nIndex)
	{
		if ((0 > nIndex) || ((int32_t)CAPACITY < nIndex))
		{
			return NULL;
		}
		if (!m_arrIndex[nIndex].IsUsed())
		{
			return NULL;
		}
		return &m_arrIndex[nIndex];
	}

protected:

	inline void SwapIndex(int32_t & iHeapIndex1, int32_t & iHeapIndex2)
	{
		int32_t iTemp = enmInvalidHeapIndex;

		iTemp = iHeapIndex1;
		iHeapIndex1 = iHeapIndex2;
		iHeapIndex2 = iTemp;
	}
	inline void Swap(CIndex*& pRIndex1, CIndex*& pRIndex2)
	{
		CIndex* pTemp = pRIndex2;
		pRIndex2 = pRIndex1;
		pRIndex1 = pTemp;

		int32_t & iHeapIndex1 = pRIndex1->m_iHeapIndex;
		int32_t & iHeapIndex2 = pRIndex2->m_iHeapIndex;

		int32_t iTemp = iHeapIndex1;
		iHeapIndex1 = iHeapIndex2;
		iHeapIndex2 = iTemp;
	}


	//���ҽڵ�
	CIndex* FindNode(TKEY key)
	{
		uint32_t		nIndex = 0;
		for (nIndex = 0; nIndex < m_nObjectCount; ++nIndex)
		{
			if (m_arrIndexPrt[nIndex]->m_key == key)
			{
				return m_arrIndexPrt[nIndex];
			}
		}
		return NULL;
	}

	//���Ҳ����


	int32_t ShiftUp(int32_t nIndex)
	{
		int32_t	iP = 0;
		int32_t	iC = nIndex;
		int32_t ret = enmInvalidHeapIndex;

		if ((iC <= 0) || (iC >= m_nObjectCount))
		{
			return S_OK;//����Ӧ���޸�Ϊ����һ�����������ʾ
		}

		while(0 != iC)
		{
			//ȷ�����ڵ�iP,����A[2]��A[1]�ĸ��׽ڵ���A[0]
			iP = (iC - 1) / 2;

			//���iC��Ӧ�Ľڵ���ڸ��ڵ��ֹͣ�Ϸ�
			if (m_arrIndexPrt[iP]->m_key <= m_arrIndexPrt[iC]->m_key)
			{
				break;
			}

			//��������
			Swap(m_arrIndexPrt[iP], m_arrIndexPrt[iC]);

			//iC��λ�����Ƶ�֮ǰ���ڵ�
			iC = iP;
		}
		return S_OK;
	}
	int32_t ShiftDown(int32_t nIndex)
	{
		int32_t	iL = 0;
		int32_t	iR = 0;
		int32_t	iC = nIndex;
		int32_t	iSmallest = iC;
		bool		bOut = false;

		if ((iC < 0) || (iC >= m_nObjectCount))
		{
			return S_OK;//��Ҫ�޸ĳɴ��󷵻�ֵ
		}

		do
		{
			//ȷ�����ӽڵ�
			iL =  (iC << 1) + 1;
			//ȷ�����ӽڵ�
			iR =  (iC << 1) + 2;

			//������ӽڵ����
			if (iL < m_nObjectCount)
			{
				if (m_arrIndexPrt[iL]->m_key < m_arrIndexPrt[iC]->m_key)
				{
					iSmallest = iL;
				}
				else
				{
					iSmallest = iC;
				}
			}

			//������ӽڵ����
			if ((iR < m_nObjectCount) && (m_arrIndexPrt[iR]->m_key < m_arrIndexPrt[iSmallest]->m_key))
			{
				iSmallest = iR;
			}

			//�����С�ڵ㲻�ǵ�ǰ�ڵ㣬���н���
			if (iC != iSmallest)
			{
				Swap(m_arrIndexPrt[iC], m_arrIndexPrt[iSmallest]);
				iC = iSmallest;
			}
			else
			{
				bOut = true;
			}

		} while (!bOut);

		return S_OK;
	}


/*
#define ReserseIndex(nIndex) (ObjectCount() - nIndex - 1)

	//�潨���ѣ���������һ��Ԫ��
	int32_t RShiftDown(int32_t nIndex)
	{

			//������һ��Ԫ��
			if ((1 == nIndex) || (0 == nIndex))
			{
				return S_OK;
			}

			int32_t iC = 0;
			int32_t iL = 0;
			int32_t iR = 0;
			int32_t iBiggest = 0;
			bool bOut = false;
			int32_t iTotalSize = ObjectCount() - 1;

			do
			{
				iC = ReverseIndex(nIndex);
				iL = iC * 2 + 1;
				iR = iL + 1;

				if (iL < iTotalSize)
				{
					if (m_arrIndexPrt[ReverseIndex(iL)] > m_arrIndexPrt[ReverseIndex(iC)])
					{
						iBiggest = iL;
					}
					else
					{
						iBiggest = iC;
					}
				}

				if (iC < iTotalSize && (m_arrIndexPrt[ReverseIndex(iC)]->GetKey() >
					m_arrIndexPrt[ReverseIndex(iBiggest)]->GetKey()))
				{
					iBiggest = iR;
				}

				if (iC != iBiggest)
				{
					Swap(m_arrIndexPrt[ReserseIndex(iC)], ReserseIndex(iBiggest));
					iC = iBiggest;
				}else
				{
					bOut = true;
				}
			} while (!bOut);

			return S_OK;
	}*/
protected:
	uint32_t		m_nObjectCount;		//�ѷ������ĸ���
	uint32_t		m_nFreeCount;		//δ�������ĸ���
	CIndex*			m_pFreeHead;		//δ���������ͷ�����������
	CIndex*			m_pFreeTail;		//δ���������β�����������
	CIndex*			m_pUsedHead;		//�ѷ��������ͷ�����������
	CIndex*			m_pUsedTail;		//�ѷ��������β�����������

	CIndex			m_arrIndex[CAPACITY];	//����������
	CIndex*		m_arrIndexPrt[CAPACITY];//ָ�����
};

#endif /* COMMON_HEAP_H_ */
