/*
 * common_rbtree.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_RBTREE_H_
#define COMMON_RBTREE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common_typedef.h"
#include "common_errordef.h"
#include "common_object.h"


//����һ�����������ģ����
template<typename TKEY, typename T, uint32_t CAPACITY>
class CRBTree : public CObject
{
public:
	enum
	{
		enmIndexFlag_Free				= 0,		//������������״̬
		enmIndexFlag_Used				= 1,		//����������ʹ��״̬
		enmInvalidObjectID				= -1,		//��Ч�Ķ���ID
	};

	enum
	{
		enmNodeColor_Red				= 0,		//��ڵ�
		enmNodeColor_Black				= 1,		//�ڽڵ�
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
			//m_pNext = NULL;
			//m_pPrev = NULL;
			m_nNext = enmInvalidObjectID;
			m_nPrev = enmInvalidObjectID;

			m_nColor = enmNodeColor_Red;
			//m_pParent = NULL;
			//m_pLeft = NULL;
			//m_pRight = NULL;
			m_nParent = enmInvalidObjectID;
			m_nLeft = enmInvalidObjectID;
			m_nRight = enmInvalidObjectID;

			memset(&m_key, 0, sizeof(m_key));

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
			//m_pNext = NULL;
			//m_pPrev = NULL;
			m_nNext = enmInvalidObjectID;
			m_nPrev = enmInvalidObjectID;

			m_nColor = enmNodeColor_Red;
			//m_pParent = NULL;
			//m_pLeft = NULL;
			//m_pRight = NULL;
			m_nParent = enmInvalidObjectID;
			m_nLeft = enmInvalidObjectID;
			m_nRight = enmInvalidObjectID;

			memset(&m_key, 0, sizeof(m_key));

			//m_object.Uninitialize();

			return S_OK;
		}

	public:
		////��ȡ�������ǰһ�ڵ�
		//CIndex* Prev() const
		//{
		//	if (NULL != m_pLeft)
		//	{
		//		return m_pLeft->MaxNode();
		//	}

		//	const CIndex* pNode = this;
		//	const CIndex* pParent = m_pParent;
		//	while (NULL != pParent)
		//	{
		//		if (pNode != pParent->GetLeft())
		//		{
		//			return (CIndex*)pParent;
		//		}
		//		pNode = pParent;
		//		pParent = pParent->GetParent();
		//	}

		//	return NULL;
		//}
		////��ȡ������ĺ�һ�ڵ�
		//CIndex* Next() const
		//{
		//	if (NULL != m_pRight)
		//	{
		//		return m_pRight->MinNode();
		//	}

		//	const CIndex* pNode = this;
		//	const CIndex* pParent = m_pParent;
		//	while (NULL != pParent)
		//	{
		//		if (pNode != pParent->GetRight())
		//		{
		//			return (CIndex*)pParent;
		//		}
		//		pNode = pParent;
		//		pParent = pParent->GetParent();
		//	}

		//	return NULL;
		//}
		//��ȡ�������ǰһ�ڵ�
		CIndex* Prev()
		{
			if (enmInvalidObjectID != m_nLeft)
			{
				return Left()->MaxNode();
			}

			CIndex* pNode = this;
			CIndex* pParent = Parent();
			while (NULL != pParent)
			{
				if (pNode != pParent->Left())
				{
					return pParent;
				}
				pNode = pParent;
				pParent = pParent->Parent();
			}

			return NULL;
		}
		//��ȡ������ĺ�һ�ڵ�
		CIndex* Next()
		{
			if (enmInvalidObjectID != m_nRight)
			{
				return Right()->MinNode();
			}

			CIndex* pNode = this;
			CIndex* pParent = Parent();
			while (NULL != pParent)
			{
				if (pNode != pParent->Right())
				{
					return pParent;
				}
				pNode = pParent;
				pParent = pParent->Parent();
			}

			return NULL;
		}
		//��ȡ����ĺ�һ�ڵ�
		inline CIndex* NextNode()
		{
			if (enmInvalidObjectID == m_nNext)
			{
				return NULL;
			}
			return &m_pContainer->m_arrIndex[m_nNext];
		}
		//��ȡ�����ǰһ�ڵ�
		inline CIndex* PrevNode()
		{
			if (enmInvalidObjectID == m_nPrev)
			{
				return NULL;
			}
			return &m_pContainer->m_arrIndex[m_nPrev];
		}

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
		inline T& Object()
		{
			return m_object.GetValue();
		}
		//���ö���
		inline void SetObject(T& object)
		{
			m_object.SetValue(object);
		}

		//��ȡ����ID
		inline int32_t Index()
		{
			return m_nIndex;
		}

	protected:
		//���ø�����ָ��
		inline void SetContainer(CRBTree* pContainer)
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

		//��ȡ�ڵ���ɫ
		inline uint32_t GetColor()
		{
			return m_nColor;
		}
		//���ýڵ���ɫ
		inline void SetColor(uint32_t nColor)
		{
			m_nColor = nColor;
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

		////��ȡ����ĸ��ڵ�
		//inline CIndex* GetParent() const
		//{
		//	return m_pParent;
		//}
		////��ȡ��������ӽڵ�
		//inline CIndex* GetLeft() const
		//{
		//	return m_pLeft;
		//}
		////��ȡ��������ӽڵ�
		//inline CIndex* GetRight() const
		//{
		//	return m_pRight;
		//}
		////��������ĸ��ڵ�
		//inline void SetParent(CIndex* pParent)
		//{
		//	m_pParent = pParent;
		//}
		////������������ӽڵ�
		//inline void SetLeft(CIndex* pLeft)
		//{
		//	m_pLeft = pLeft;
		//}
		////������������ӽڵ�
		//inline void SetRight(CIndex* pRight)
		//{
		//	m_pRight = pRight;
		//}
		//��ȡ����ĸ��ڵ�
		inline int32_t GetParent() const
		{
			return m_nParent;
		}
		//��ȡ��������ӽڵ�
		inline int32_t GetLeft() const
		{
			return m_nLeft;
		}
		//��ȡ��������ӽڵ�
		inline int32_t GetRight() const
		{
			return m_nRight;
		}
		//��������ĸ��ڵ�
		inline void SetParent(int32_t nParent)
		{
			m_nParent = nParent;
		}
		//������������ӽڵ�
		inline void SetLeft(int32_t nLeft)
		{
			m_nLeft = nLeft;
		}
		//������������ӽڵ�
		inline void SetRight(int32_t nRight)
		{
			m_nRight = nRight;
		}
		//��������ĸ��ڵ�
		inline CIndex* Parent()
		{
			if (enmInvalidObjectID == m_nParent)
			{
				return NULL;
			}
			return &m_pContainer->m_arrIndex[m_nParent];
		}
		//��ȡ��������ӽڵ�
		inline CIndex* Left()
		{
			if (enmInvalidObjectID == m_nLeft)
			{
				return NULL;
			}
			return &m_pContainer->m_arrIndex[m_nLeft];
		}
		//��ȡ��������ӽڵ�
		inline CIndex* Right()
		{
			if (enmInvalidObjectID == m_nRight)
			{
				return NULL;
			}
			return &m_pContainer->m_arrIndex[m_nRight];
		}

		//���ö���ID
		inline void SetIndex(int32_t nIndex)
		{
			m_nIndex = nIndex;
		}

		//���ü�ֵ
		inline void SetKey(TKEY key)
		{
			m_key = key;
		}

		////��λ������ڵ�
		//inline void ResetRBNode()
		//{
		//	m_pParent = NULL;
		//	m_pLeft = NULL;
		//	m_pRight = NULL;
		//	memset(&m_key, 0, sizeof(m_key));
		//}
		//��λ������ڵ�
		inline void ResetRBNode()
		{
			m_nParent = enmInvalidObjectID;
			m_nLeft = enmInvalidObjectID;
			m_nRight = enmInvalidObjectID;
			memset(&m_key, 0, sizeof(m_key));
		}

		////��ȡ��������С�ڵ�
		//CIndex* MinNode()
		//{
		//	CIndex* pNode = this;

		//	while (NULL != pNode->GetLeft())
		//	{
		//		pNode = pNode->GetLeft();
		//	}

		//	return pNode;
		//}
		////��ȡ���������ڵ�
		//CIndex* MaxNode()
		//{
		//	CIndex* pNode = this;

		//	while (NULL != pNode->GetRight())
		//	{
		//		pNode = pNode->GetRight();
		//	}

		//	return pNode;
		//}
		//��ȡ��������С�ڵ�
		CIndex* MinNode()
		{
			CIndex* pNode = this;

			while (NULL != pNode->Left())
			{
				pNode = pNode->Left();
			}

			return pNode;
		}
		//��ȡ���������ڵ�
		CIndex* MaxNode()
		{
			CIndex* pNode = this;

			while (NULL != pNode->Right())
			{
				pNode = pNode->Right();
			}

			return pNode;
		}

	protected:
		CRBTree*		m_pContainer;

		uint32_t		m_nFlag;		//����״̬��־
		int32_t			m_nIndex;		//����������
		//CIndex*			m_pNext;		//��һ�ڵ㣬�����ڴ����
		//CIndex*			m_pPrev;		//ǰһ�ڵ㣬�����ڴ����
		int32_t			m_nNext;		//��һ�ڵ㣬�����ڴ����
		int32_t			m_nPrev;		//ǰһ�ڵ㣬�����ڴ����

		uint32_t		m_nColor;		//�ڵ���ɫ
		//CIndex*			m_pParent;		//���ڵ�
		//CIndex*			m_pLeft;		//���ӽڵ�
		//CIndex*			m_pRight;		//���ӽڵ�
		int32_t			m_nParent;		//���ڵ�
		int32_t			m_nLeft;		//���ӽڵ�
		int32_t			m_nRight;		//���ӽڵ�

		TKEY			m_key;			//��ֵ
		CComplexType<T>	m_object;		//�����Ķ���

		friend class CRBTree;
	};


public:
	CRBTree()
	{
	}
	virtual ~CRBTree()
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

		//m_pRoot = NULL;
		m_nRoot = enmInvalidObjectID;

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

		//m_pRoot = NULL;
		m_nRoot = enmInvalidObjectID;

		for (uint32_t i = 0; i < CAPACITY; ++i)
		{
			m_arrIndex[i].Uninitialize();
		}

		BuildIndexList();

		return S_OK;
	}

public:
	////�������
	//CIndex* Insert(TKEY key, T object)
	//{
	//	CIndex* pParent = NULL;
	//	CIndex* pNode = NULL;

	//	pParent = FindInsertPostion(key);

	//	//��������
	//	pNode = CreateIndex();
	//	if (NULL == pNode)
	//	{
	//		return NULL;
	//	}

	//	pNode->SetParent(pParent);
	//	pNode->SetLeft(NULL);
	//	pNode->SetRight(NULL);
	//	pNode->SetColor(enmNodeColor_Red);

	//	pNode->SetKey(key);
	//	pNode->SetObject(object);

	//	if (NULL != pParent)
	//	{
	//		if (pParent->GetKey() > key)
	//		{
	//			pParent->SetLeft(pNode);
	//		}
	//		else
	//		{
	//			pParent->SetRight(pNode);
	//		}
	//	}
	//	else
	//	{
	//		m_pRoot = pNode;
	//	}

	//	InsertRebalance(pNode);

	//	return pNode;
	//}
	////ɾ������
	//uint32_t Erase(TKEY key)
	//{
	//	CIndex* pNode = NULL;
	//	CIndex* pNext = NULL;

	//	pNode = FindNode(key);
	//	while (NULL != pNode)
	//	{
	//		Erase(pNode);
	//		pNode = FindNode(key);
	//	}

	//	return S_OK;
	//}
	////ɾ������
	//uint32_t Erase(CIndex* pNode)
	//{
	//	CIndex* pChild = NULL;
	//	CIndex* pParent = NULL;
	//	CIndex* pOld = NULL;
	//	//CIndex* pLeft = NULL;
	//	uint32_t nColor = enmNodeColor_Red;

	//	pOld = pNode;
	//	if ((NULL != pNode->GetLeft()) && (NULL != pNode->GetRight()))
	//	{
	//		pNode = pNode->GetRight();
	//		while (NULL != pNode->GetLeft())
	//		{
	//			pNode = pNode->GetLeft();
	//		}
	//		pChild = pNode->GetRight();
	//		pParent = pNode->GetParent();
	//		nColor = pNode->GetColor();

	//		if (NULL != pChild)
	//		{
	//			pChild->SetParent(pParent);
	//		}
	//		if (NULL != pParent)
	//		{
	//			if (pParent->GetLeft() == pNode)
	//			{
	//				pParent->SetLeft(pChild);
	//			}
	//			else
	//			{
	//				pParent->SetRight(pChild);
	//			}
	//		}
	//		else
	//		{
	//			m_pRoot = pChild;
	//		}

	//		if (pNode->GetParent() == pOld)
	//		{
	//			pParent = pNode;
	//		}

	//		pNode->SetParent(pOld->GetParent());
	//		pNode->SetColor(pOld->GetColor());
	//		pNode->SetRight(pOld->GetRight());
	//		pNode->SetLeft(pOld->GetLeft());

	//		if (NULL != pOld->GetParent())
	//		{
	//			if (pOld->GetParent()->GetLeft() == pOld)
	//			{
	//				pOld->GetParent()->SetLeft(pNode);
	//			}
	//			else
	//			{
	//				pOld->GetParent()->SetRight(pNode);
	//			}
	//		}
	//		else
	//		{
	//			m_pRoot = pNode;
	//		}

	//		pOld->GetLeft()->SetParent(pNode);
	//		if (NULL != pOld->GetRight())
	//		{
	//			pOld->GetRight()->SetParent(pNode);
	//		}
	//	}
	//	else
	//	{
	//		if (NULL == pNode->GetLeft())
	//		{
	//			pChild = pNode->GetRight();
	//		}
	//		else if (NULL == pNode->GetRight())
	//		{
	//			pChild = pNode->GetLeft();
	//		}
	//		pParent = pNode->GetParent();
	//		nColor = pNode->GetColor();

	//		if (NULL != pChild)
	//		{
	//			pChild->SetParent(pParent);
	//		}
	//		if (NULL != pParent)
	//		{
	//			if (pParent->GetLeft() == pNode)
	//			{
	//				pParent->SetLeft(pChild);
	//			}
	//			else
	//			{
	//				pParent->SetRight(pChild);
	//			}
	//		}
	//		else
	//		{
	//			m_pRoot = pChild;
	//		}
	//	}

	//	DestroyIndex(pOld);
	//	pOld->ResetRBNode();

	//	if (enmNodeColor_Black == nColor)
	//	{
	//		EraseRebalance(pChild, pParent);
	//	}

	//	return S_OK;
	//}
	//�������
	CIndex* Insert(TKEY key, T object)
	{
		CIndex* pParent = NULL;
		CIndex* pNode = NULL;

		pParent = FindInsertPostion(key);

		//��������
		pNode = CreateIndex();
		if (NULL == pNode)
		{
			return NULL;
		}

		pNode->SetParent((NULL == pParent) ? enmInvalidObjectID : pParent->Index());
		pNode->SetLeft(enmInvalidObjectID);
		pNode->SetRight(enmInvalidObjectID);
		pNode->SetColor(enmNodeColor_Red);

		pNode->SetKey(key);
		pNode->SetObject(object);

		if (NULL != pParent)
		{
			if (pParent->GetKey() > key)
			{
				pParent->SetLeft(pNode->Index());
			}
			else
			{
				pParent->SetRight(pNode->Index());
			}
		}
		else
		{
			m_nRoot = pNode->Index();
		}

		InsertRebalance(pNode);

		return pNode;
	}
	//ɾ������
	uint32_t Erase(TKEY key)
	{
		CIndex* pNode = NULL;

		pNode = FindNode(key);
		while (NULL != pNode)
		{
			Erase(pNode);
			pNode = FindNode(key);
		}

		return S_OK;
	}
	//ɾ������
	uint32_t Erase(CIndex* pNode)
	{
		CIndex* pChild = NULL;
		CIndex* pParent = NULL;
		CIndex* pOld = NULL;
		//CIndex* pLeft = NULL;
		uint32_t nColor = enmNodeColor_Red;

		pOld = pNode;
		if ((NULL != pNode->Left()) && (NULL != pNode->Right()))
		{
			pNode = pNode->Right();
			while (NULL != pNode->Left())
			{
				pNode = pNode->Left();
			}
			pChild = pNode->Right();
			pParent = pNode->Parent();
			nColor = pNode->GetColor();

			if (NULL != pChild)
			{
				pChild->SetParent((NULL == pParent) ? enmInvalidObjectID : pParent->Index());
			}
			if (NULL != pParent)
			{
				if (pParent->Left() == pNode)
				{
					pParent->SetLeft((NULL == pChild) ? enmInvalidObjectID : pChild->Index());
				}
				else
				{
					pParent->SetRight((NULL == pChild) ? enmInvalidObjectID : pChild->Index());
				}
			}
			else
			{
				m_nRoot = (NULL == pChild) ? enmInvalidObjectID : pChild->Index();
			}

			if (pNode->Parent() == pOld)
			{
				pParent = pNode;
			}

			pNode->SetParent(pOld->GetParent());
			pNode->SetColor(pOld->GetColor());
			pNode->SetRight(pOld->GetRight());
			pNode->SetLeft(pOld->GetLeft());

			if (NULL != pOld->Parent())
			{
				if (pOld->Parent()->Left() == pOld)
				{
					pOld->Parent()->SetLeft(pNode->Index());
				}
				else
				{
					pOld->Parent()->SetRight(pNode->Index());
				}
			}
			else
			{
				m_nRoot = pNode->Index();
			}

			pOld->Left()->SetParent(pNode->Index());
			if (NULL != pOld->Right())
			{
				pOld->Right()->SetParent(pNode->Index());
			}
		}
		else
		{
			if (NULL == pNode->Left())
			{
				pChild = pNode->Right();
			}
			else if (NULL == pNode->Right())
			{
				pChild = pNode->Left();
			}
			pParent = pNode->Parent();
			nColor = pNode->GetColor();

			if (NULL != pChild)
			{
				pChild->SetParent((NULL == pParent) ? enmInvalidObjectID : pParent->Index());
			}
			if (NULL != pParent)
			{
				if (pParent->Left() == pNode)
				{
					pParent->SetLeft((NULL == pChild) ? enmInvalidObjectID : pChild->Index());
				}
				else
				{
					pParent->SetRight((NULL == pChild) ? enmInvalidObjectID : pChild->Index());
				}
			}
			else
			{
				m_nRoot = (NULL == pChild) ? enmInvalidObjectID : pChild->Index();
			}
		}

		DestroyIndex(pOld);
		pOld->ResetRBNode();

		if (enmNodeColor_Black == nColor)
		{
			EraseRebalance(pChild, pParent);
		}

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
	////��ȡ��С��������
	//CIndex* First() const
	//{
	//	if (NULL == m_pRoot)
	//	{
	//		return NULL;
	//	}

	//	return m_pRoot->GetMin();
	//}
	////��ȡ����������
	//CIndex* Last() const
	//{
	//	if (NULL == m_pRoot)
	//	{
	//		return NULL;
	//	}

	//	return m_pRoot->GetMax();
	//}
	//��ȡ��С��������
	inline CIndex* First()
	{
		if (enmInvalidObjectID == m_nRoot)
		{
			return NULL;
		}

		return m_arrIndex[m_nRoot].MinNode();
	}
	//��ȡ����������
	inline CIndex* Last()
	{
		if (enmInvalidObjectID == m_nRoot)
		{
			return NULL;
		}

		return m_arrIndex[m_nRoot].MaxNode();
	}
	//��ȡͷ����������
	inline CIndex* FirstNode()
	{
		return (enmInvalidObjectID == m_nUsedHead) ? NULL : &m_arrIndex[m_nUsedHead];
	}
	//��ȡβ����������
	inline CIndex* LastNode()
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
	//	m_arrIndex[0].SetPrev(NULL);

	//	for (uint32_t i = 1; i < CAPACITY; ++i)
	//	{
	//		m_arrIndex[i - 1].SetNext(&m_arrIndex[i]);
	//		m_arrIndex[i].SetPrev(&m_arrIndex[i - 1]);
	//	}

	//	//�������һ������
	//	m_arrIndex[CAPACITY - 1].SetNext(NULL);

	//	return S_OK;
	//}
	//����������������
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
	CIndex* CreateIndex()
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
	int32_t DestroyIndex(CIndex* pIndex)
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
			pIndex->PrevNode()->SetNext(pIndex->GetNext());
		}
		if (enmInvalidObjectID != pIndex->GetNext())
		{
			pIndex->NextNode()->SetPrev(pIndex->GetPrev());
		}

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

	//��ȡ���ڵ�
	inline CIndex* Root()
	{
		if (enmInvalidObjectID == m_nRoot)
		{
			return NULL;
		}
		return &m_arrIndex[m_nRoot];
	}

protected:
	//void RotateLeft(CIndex* pNode)
	//{
	//	CIndex* pRight = pNode->GetRight();

	//	pNode->SetRight(pRight->GetLeft());
	//	if (NULL != pNode->GetRight())
	//	{
	//		pRight->GetLeft()->SetParent(pNode);
	//	}
	//	pRight->SetLeft(pNode);

	//	pRight->SetParent(pNode->GetParent());
	//	if (NULL != pRight->GetParent())
	//	{
	//		if (pNode == pNode->GetParent()->GetRight())
	//		{
	//			pNode->GetParent()->SetRight(pRight);
	//		}
	//		else
	//		{
	//			pNode->GetParent()->SetLeft(pRight);
	//		}
	//	}
	//	else
	//	{
	//		m_pRoot = pRight;
	//	}
	//	pNode->SetParent(pRight);
	//}
	//void RotateRight(CIndex* pNode)
	//{
	//	CIndex* pLeft = pNode->GetLeft();

	//	pNode->SetLeft(pLeft->GetRight());
	//	if (NULL != pNode->GetLeft())
	//	{
	//		pLeft->GetRight()->SetParent(pNode);
	//	}
	//	pLeft->SetRight(pNode);

	//	pLeft->SetParent(pNode->GetParent());
	//	if (NULL != pLeft->GetParent())
	//	{
	//		if (pNode == pNode->GetParent()->GetRight())
	//		{
	//			pNode->GetParent()->SetRight(pLeft);
	//		}
	//		else
	//		{
	//			pNode->GetParent()->SetLeft(pLeft);
	//		}
	//	}
	//	else
	//	{
	//		m_pRoot = pLeft;
	//	}
	//	pNode->SetParent(pLeft);
	//}
	/*-----------------------------------------------------------
	|   node           right
	|   / \    ==>     / \
	|   a  right     node  y
	|       / \           / \
	|       b  y         a   b
	-----------------------------------------------------------*/
	void RotateLeft(CIndex* pNode)
	{
		CIndex* pRight = pNode->Right();

		pNode->SetRight(pRight->GetLeft());
		if (NULL != pNode->Right())
		{
			pRight->Left()->SetParent(pNode->Index());
		}
		pRight->SetLeft(pNode->Index());

		pRight->SetParent(pNode->GetParent());
		if (NULL != pRight->Parent())
		{
			if (pNode == pNode->Parent()->Right())
			{
				pNode->Parent()->SetRight(pRight->Index());
			}
			else
			{
				pNode->Parent()->SetLeft(pRight->Index());
			}
		}
		else
		{
			m_nRoot = pRight->Index();
		}
		pNode->SetParent(pRight->Index());
	}
	/*-----------------------------------------------------------
	|       node           left
	|       / \            / \
	|    left  y   ==>    a   node
	|   / \               / \
	|  a   b             b   y
	-----------------------------------------------------------*/
	void RotateRight(CIndex* pNode)
	{
		CIndex* pLeft = pNode->Left();

		pNode->SetLeft(pLeft->GetRight());
		if (NULL != pNode->Left())
		{
			pLeft->Right()->SetParent(pNode->Index());
		}
		pLeft->SetRight(pNode->Index());

		pLeft->SetParent(pNode->GetParent());
		if (NULL != pLeft->Parent())
		{
			if (pNode == pNode->Parent()->Right())
			{
				pNode->Parent()->SetRight(pLeft->Index());
			}
			else
			{
				pNode->Parent()->SetLeft(pLeft->Index());
			}
		}
		else
		{
			m_nRoot = pLeft->Index();
		}
		pNode->SetParent(pLeft->Index());
	}

	////����ڵ������ƽ��
	//void InsertRebalance(CIndex* pNode)
	//{
	//	CIndex* pParent = NULL;
	//	CIndex* pGParent = NULL;
	//	CIndex* pUncle = NULL;
	//	CIndex* p = NULL;

	//	pParent = pNode->GetParent();
	//	while ((NULL != pParent) && (enmNodeColor_Red == pParent->GetColor()))
	//	{
	//		pGParent = pParent->GetParent();

	//		if (pParent == pGParent->GetLeft())
	//		{
	//			pUncle = pGParent->GetRight();
	//			if ((NULL != pUncle) && (enmNodeColor_Red == pUncle->GetColor()))
	//			{
	//				pUncle->SetColor(enmNodeColor_Black);
	//				pParent->SetColor(enmNodeColor_Black);
	//				pGParent->SetColor(enmNodeColor_Red);
	//				pNode = pGParent;
	//			}
	//			else
	//			{
	//				if (pParent->GetRight() == pNode)
	//				{
	//					RotateLeft(pParent);
	//					p = pParent;
	//					pParent = pNode;
	//					pNode = p;
	//				}

	//				pParent->SetColor(enmNodeColor_Black);
	//				pGParent->SetColor(enmNodeColor_Red);
	//				RotateRight(pGParent);
	//			}
	//		}
	//		else
	//		{
	//			pUncle = pGParent->GetLeft();
	//			if ((NULL != pUncle) && (enmNodeColor_Red == pUncle->GetColor()))
	//			{
	//				pUncle->SetColor(enmNodeColor_Black);
	//				pParent->SetColor(enmNodeColor_Black);
	//				pGParent->SetColor(enmNodeColor_Red);
	//				pNode = pGParent;
	//			}
	//			else
	//			{
	//				if (pParent->GetLeft() == pNode)
	//				{
	//					RotateRight(pParent);
	//					p = pParent;
	//					pParent = pNode;
	//					pNode = p;
	//				}

	//				pParent->SetColor(enmNodeColor_Black);
	//				pGParent->SetColor(enmNodeColor_Red);
	//				RotateLeft(pGParent);
	//			}
	//		}

	//		pParent = pNode->GetParent();
	//	}

	//	m_pRoot->SetColor(enmNodeColor_Black);
	//}

	////ɾ���ڵ������ƽ��
	//void EraseRebalance(CIndex* pNode, CIndex* pParent)
	//{
	//	CIndex* pOther = NULL;
	//	CIndex* pOtherLeft = NULL;
	//	CIndex* pOtherRight = NULL;

	//	while (((NULL == pNode) || (enmNodeColor_Black == pNode->GetColor())) && (pNode != m_pRoot))
	//	{
	//		if (pParent->GetLeft() == pNode)
	//		{
	//			pOther = pParent->GetRight();
	//			if (enmNodeColor_Red == pOther->GetColor())
	//			{
	//				pOther->SetColor(enmNodeColor_Black);
	//				pParent->SetColor(enmNodeColor_Red);
	//				RotateLeft(pParent);
	//				pOther = pParent->GetRight();
	//			}
	//			if (((NULL == pOther->GetLeft()) || (enmNodeColor_Black == pOther->GetLeft()->GetColor())) &&
	//				((NULL == pOther->GetRight()) || (enmNodeColor_Black == pOther->GetRight()->GetColor())))
	//			{
	//				pOther->SetColor(enmNodeColor_Red);
	//				pNode = pParent;
	//				pParent = pNode->GetParent();
	//			}
	//			else
	//			{
	//				if ((NULL == pOther->GetRight()) || (enmNodeColor_Black == pOther->GetRight()->GetColor()))
	//				{
	//					pOtherLeft = pOther->GetLeft();
	//					if (NULL != pOtherLeft)
	//					{
	//						pOtherLeft->SetColor(enmNodeColor_Black);
	//					}
	//					pOther->SetColor(enmNodeColor_Red);
	//					RotateRight(pOther);
	//					pOther = pParent->GetRight();
	//				}
	//				pOther->SetColor(pParent->GetColor());
	//				pParent->SetColor(enmNodeColor_Black);
	//				if (NULL != pOther->GetRight())
	//				{
	//					pOther->GetRight()->SetColor(enmNodeColor_Black);
	//				}
	//				RotateLeft(pParent);
	//				pNode = m_pRoot;
	//				break;
	//			}
	//		}
	//		else
	//		{
	//			pOther = pParent->GetLeft();
	//			if (enmNodeColor_Red == pOther->GetColor())
	//			{
	//				pOther->SetColor(enmNodeColor_Black);
	//				pParent->SetColor(enmNodeColor_Red);
	//				RotateRight(pParent);
	//				pOther = pParent->GetLeft();
	//			}
	//			if (((NULL == pOther->GetLeft()) || (enmNodeColor_Black == pOther->GetLeft()->GetColor())) &&
	//				 ((NULL == pOther->GetRight()) || (enmNodeColor_Black == pOther->GetRight()->GetColor())))
	//			{
	//				pOther->SetColor(enmNodeColor_Red);
	//				pNode = pParent;
	//				pParent = pNode->GetParent();
	//			}
	//			else
	//			{
	//				if ((NULL == pOther->GetLeft()) || (enmNodeColor_Black == pOther->GetLeft()->GetColor()))
	//				{
	//					pOtherRight = pOther->GetRight();
	//					if (NULL != pOtherRight)
	//					{
	//						pOtherRight->SetColor(enmNodeColor_Black);
	//					}
	//					pOther->SetColor(enmNodeColor_Red);
	//					RotateLeft(pOther);
	//					pOther = pParent->GetLeft();
	//				}
	//				pOther->SetColor(pParent->GetColor());
	//				pParent->SetColor(enmNodeColor_Black);
	//				if (NULL != pOther->GetLeft())
	//				{
	//					pOther->GetLeft()->SetColor(enmNodeColor_Black);
	//				}
	//				RotateRight(pParent);
	//				pNode = m_pRoot;
	//				break;
	//			}
	//		}
	//	}

	//	if (NULL != pNode)
	//	{
	//		pNode->SetColor(enmNodeColor_Black);
	//	}
	//}
	//����ڵ������ƽ��
	void InsertRebalance(CIndex* pNode)
	{
		CIndex* pParent = NULL;
		CIndex* pGParent = NULL;
		CIndex* pUncle = NULL;
		CIndex* p = NULL;

		pParent = pNode->Parent();
		while ((NULL != pParent) && (enmNodeColor_Red == pParent->GetColor()))
		{
			pGParent = pParent->Parent();

			if (pParent == pGParent->Left())
			{
				pUncle = pGParent->Right();
				if ((NULL != pUncle) && (enmNodeColor_Red == pUncle->GetColor()))
				{
					pUncle->SetColor(enmNodeColor_Black);
					pParent->SetColor(enmNodeColor_Black);
					pGParent->SetColor(enmNodeColor_Red);
					pNode = pGParent;
				}
				else
				{
					if (pParent->Right() == pNode)
					{
						RotateLeft(pParent);
						p = pParent;
						pParent = pNode;
						pNode = p;
					}

					pParent->SetColor(enmNodeColor_Black);
					pGParent->SetColor(enmNodeColor_Red);
					RotateRight(pGParent);
				}
			}
			else
			{
				pUncle = pGParent->Left();
				if ((NULL != pUncle) && (enmNodeColor_Red == pUncle->GetColor()))
				{
					pUncle->SetColor(enmNodeColor_Black);
					pParent->SetColor(enmNodeColor_Black);
					pGParent->SetColor(enmNodeColor_Red);
					pNode = pGParent;
				}
				else
				{
					if (pParent->Left() == pNode)
					{
						RotateRight(pParent);
						p = pParent;
						pParent = pNode;
						pNode = p;
					}

					pParent->SetColor(enmNodeColor_Black);
					pGParent->SetColor(enmNodeColor_Red);
					RotateLeft(pGParent);
				}
			}

			pParent = pNode->Parent();
		}

		Root()->SetColor(enmNodeColor_Black);
	}

	//ɾ���ڵ������ƽ��
	void EraseRebalance(CIndex* pNode, CIndex* pParent)
	{
		CIndex* pOther = NULL;
		CIndex* pOtherLeft = NULL;
		CIndex* pOtherRight = NULL;

		while (((NULL == pNode) || (enmNodeColor_Black == pNode->GetColor())) && (pNode != Root()))
		{
			if (pParent->Left() == pNode)
			{
				pOther = pParent->Right();
				if (enmNodeColor_Red == pOther->GetColor())
				{
					pOther->SetColor(enmNodeColor_Black);
					pParent->SetColor(enmNodeColor_Red);
					RotateLeft(pParent);
					pOther = pParent->Right();
				}
				if (((NULL == pOther->Left()) || (enmNodeColor_Black == pOther->Left()->GetColor())) &&
					((NULL == pOther->Right()) || (enmNodeColor_Black == pOther->Right()->GetColor())))
				{
					pOther->SetColor(enmNodeColor_Red);
					pNode = pParent;
					pParent = pNode->Parent();
				}
				else
				{
					if ((NULL == pOther->Right()) || (enmNodeColor_Black == pOther->Right()->GetColor()))
					{
						pOtherLeft = pOther->Left();
						if (NULL != pOtherLeft)
						{
							pOtherLeft->SetColor(enmNodeColor_Black);
						}
						pOther->SetColor(enmNodeColor_Red);
						RotateRight(pOther);
						pOther = pParent->Right();
					}
					pOther->SetColor(pParent->GetColor());
					pParent->SetColor(enmNodeColor_Black);
					if (NULL != pOther->Right())
					{
						pOther->Right()->SetColor(enmNodeColor_Black);
					}
					RotateLeft(pParent);
					pNode = Root();
					break;
				}
			}
			else
			{
				pOther = pParent->Left();
				if (enmNodeColor_Red == pOther->GetColor())
				{
					pOther->SetColor(enmNodeColor_Black);
					pParent->SetColor(enmNodeColor_Red);
					RotateRight(pParent);
					pOther = pParent->Left();
				}
				if (((NULL == pOther->Left()) || (enmNodeColor_Black == pOther->Left()->GetColor())) &&
					((NULL == pOther->Right()) || (enmNodeColor_Black == pOther->Right()->GetColor())))
				{
					pOther->SetColor(enmNodeColor_Red);
					pNode = pParent;
					pParent = pNode->Parent();
				}
				else
				{
					if ((NULL == pOther->Left()) || (enmNodeColor_Black == pOther->Left()->GetColor()))
					{
						pOtherRight = pOther->Right();
						if (NULL != pOtherRight)
						{
							pOtherRight->SetColor(enmNodeColor_Black);
						}
						pOther->SetColor(enmNodeColor_Red);
						RotateLeft(pOther);
						pOther = pParent->Left();
					}
					pOther->SetColor(pParent->GetColor());
					pParent->SetColor(enmNodeColor_Black);
					if (NULL != pOther->Left())
					{
						pOther->Left()->SetColor(enmNodeColor_Black);
					}
					RotateRight(pParent);
					pNode = Root();
					break;
				}
			}
		}

		if (NULL != pNode)
		{
			pNode->SetColor(enmNodeColor_Black);
		}
	}

	////���ҽڵ�
	//CIndex* FindNode(TKEY key)
	//{
	//	CIndex* pNode = m_pRoot;
	//	CIndex* pParent = NULL;

	//	while (NULL != pNode)
	//	{
	//		pParent = pNode;
	//		if (pNode->GetKey() > key)
	//		{
	//			pNode = pNode->GetLeft();
	//		}
	//		else if (pNode->GetKey() < key)
	//		{
	//			pNode = pNode->GetRight();
	//		}
	//		else
	//		{
	//			return pNode;
	//		}
	//	}

	//	return NULL;
	//}
	//���ҽڵ�
	CIndex* FindNode(TKEY key)
	{
		CIndex* pNode = Root();
		CIndex* pParent = NULL;

		while (NULL != pNode)
		{
			pParent = pNode;
			if (pNode->GetKey() > key)
			{
				pNode = pNode->Left();
			}
			else if (pNode->GetKey() < key)
			{
				pNode = pNode->Right();
			}
			else
			{
				return pNode;
			}
		}

		return NULL;
	}

	////���Ҳ����
	//CIndex* FindInsertPostion(TKEY key)
	//{
	//	CIndex* pNode = m_pRoot;
	//	CIndex* pParent = NULL;

	//	while (NULL != pNode)
	//	{
	//		pParent = pNode;
	//		if (pNode->GetKey() > key)
	//		{
	//			pNode = pNode->GetLeft();
	//		}
	//		else
	//		{
	//			pNode = pNode->GetRight();
	//		}
	//	}

	//	return pParent;
	//}
	//���Ҳ����
	CIndex* FindInsertPostion(TKEY key)
	{
		CIndex* pNode = Root();
		CIndex* pParent = NULL;

		while (NULL != pNode)
		{
			pParent = pNode;
			if (pNode->GetKey() > key)
			{
				pNode = pNode->Left();
			}
			else
			{
				pNode = pNode->Right();
			}
		}

		return pParent;
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

	//CIndex*			m_pRoot;			//���ڵ�
	int32_t			m_nRoot;			//���ڵ�

	CIndex			m_arrIndex[CAPACITY];	//����������
};

#endif /* COMMON_RBTREE_H_ */
