/*
 * common_codequeue.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_CODEQUEUE_H_
#define COMMON_CODEQUEUE_H_

#include <stdio.h>
#include <memory.h>

#include "common_typedef.h"
#include "common_object.h"
#include "common_errordef.h"


template<uint32_t CAPACITY>
class CCodeQueue
{
public:
	enum
	{
		enmReservedCodeLength		= 8,	//Ԥ���ռ䳤��
	};
public:
	CCodeQueue()
	{
	}
	~CCodeQueue()
	{
	}

public:
	//��ʼ������
	int32_t Initialize()
	{
		m_nSize = CAPACITY;
		m_nHead = 0;
		m_nTail = 0;
		m_nOffset = sizeof(uint32_t) * 4;
		memset(m_arrCodeBuffer, 0, sizeof(m_arrCodeBuffer));

		return S_OK;
	}
	//�ָ�����
	int32_t Resume()
	{
		return S_OK;
	}
	//ע������
	int32_t Uninitialize()
	{
		return S_OK;
	}

public:
	//�Ӷ���β��׷��һ�����ݰ�
	int32_t Push(const uint8_t* pBuffer, uint32_t nLength)
	{
		if ((NULL == pBuffer) || (0 >= nLength))
		{
			return E_INVALIDARGUMENT;
		}

		//��ȡ�����׵�ַ�Ͷ��пռ��ܴ�С
		uint8_t* pCodeBuffer = GetCodeBuffer();
		uint32_t nCapacity = GetCapacity();

		if ((NULL == pCodeBuffer) || (nCapacity <= 0))
		{
			return E_QUEUENOTINITIALIZED;
		}
		if (IsFull())
		{
			return E_FULLQUEUE;
		}

		//��ȡ������βλ��
		uint32_t nHead = 0;
		uint32_t nTail = 0;
		uint32_t nFreeSize = 0;
		GetFreeInfo(nHead, nTail, nFreeSize);

		//�����βλ���Ƿ���Ч
		if ((nHead >= nCapacity) || (nTail >= nCapacity))
		{
			SetBoundary(0, 0);
			return E_QUEUESHATTERED;
		}

		if ((uint32_t)sizeof(uint32_t) + nLength > nFreeSize)
		{
			//���пռ䲻��
			return E_SPACENOTENOUGH;
		}

		uint8_t* p = (uint8_t*)&nLength;

		//�����ݰ����ȱ��浽������
		for (size_t i = 0; i < sizeof(uint32_t); ++i)
		{
			pCodeBuffer[nTail] = p[i];
			nTail = (nTail + 1) % nCapacity;
		}

		//�����ݰ����浽������
		if (nHead > nTail)
		{
			memcpy(&pCodeBuffer[nTail], pBuffer, nLength);
		}
		else
		{
			if (nLength > (nCapacity - nTail))
			{
				//��Ҫ�ֶο���
				memcpy(pCodeBuffer + nTail, pBuffer, nCapacity - nTail);
				memcpy(pCodeBuffer, pBuffer + (nCapacity - nTail), nLength - (nCapacity - nTail));
			}
			else
			{
				memcpy(pCodeBuffer + nTail, pBuffer, nLength);
			}
		}

		nTail = (nTail + nLength) % nCapacity;

		//���Ķ���β��λ��
		return SetTail(nTail);
	}

	//�Ӷ���ͷ����ȡһ�����ݰ�,�������α����ڻ�������,����ֵΪ�µ�ͷָ��λ��
	int32_t Front(uint8_t* pBuffer, uint32_t nSize, uint32_t& nLength)
	{
		if ((NULL == pBuffer) || (0 >= nSize))
		{
			return E_INVALIDARGUMENT;
		}

		//��ȡ�����׵�ַ�Ͷ��пռ��ܴ�С
		uint8_t* pCodeBuffer = GetCodeBuffer();
		uint32_t nCapacity = GetCapacity();

		if ((NULL == pCodeBuffer) || (nCapacity <= 0))
		{
			return E_QUEUENOTINITIALIZED;
		}
		if (IsEmpty())
		{
			return E_EMPTYQUEUE;
		}

		//��ȡ������βλ��
		uint32_t nHead = 0;
		uint32_t nTail = 0;
		uint32_t nUsedSize = 0;
		GetUsedInfo(nHead, nTail, nUsedSize);

		//�����βλ���Ƿ���Ч
		if ((nHead >= nCapacity) || (nTail >= nCapacity))
		{
			SetBoundary(0, 0);
			return E_QUEUESHATTERED;
		}
		//������ʹ�ÿռ�С��4���ֽ�
		if (nUsedSize < sizeof(uint32_t))
		{
			SetBoundary(0, 0);
			return E_QUEUESHATTERED;
		}

		//ʹ��nRealSize�������ݰ�����
		uint32_t nRealSize = 0;
		uint8_t* p = (uint8_t*)&nRealSize;

		//��ȡ���ݰ�����
		for (size_t i = 0; i < sizeof(uint32_t); ++i)
		{
			p[i] = pCodeBuffer[nHead];
			nHead = (nHead + 1) % nCapacity;
		}

		//�Ӷ����ж�ȡ�����ݰ����ȳ��Ȳ�����Ҫ��
		if ((nRealSize <= 0) || (nRealSize > (nUsedSize - sizeof(uint32_t))))
		{
			SetBoundary(0, 0);
			return E_INVALIDPACKAGE;
		}

		//������������Ȳ���
		if (nSize < nRealSize)
		{
			//���������ݰ�
			nHead = (nHead + nRealSize) % nCapacity;
			SetHead(nHead);
			return E_INVALIDPACKAGE;
		}

		//�Ӷ�����ȡ�����ݰ�
		nLength = nRealSize;

		if (nTail > nHead)
		{
			memcpy(pBuffer, pCodeBuffer + nHead, nRealSize);
		}
		else
		{
			if (nRealSize > (nCapacity - nHead))
			{
				//��Ҫ�ֶο���
				memcpy(pBuffer, pCodeBuffer + nHead, nCapacity - nHead);
				memcpy(pBuffer + (nCapacity - nHead), pCodeBuffer, nRealSize - (nCapacity - nHead));
			}
			else
			{
				memcpy(pBuffer, pCodeBuffer + nHead, nRealSize);
			}

		}

		nHead = (nHead + nRealSize) % nCapacity;

		//���Ķ���ͷ��λ��
		return (int32_t)nHead;
	}

	//��ǰlen���ֽڶ���
	int32_t Pop(uint32_t len)
	{
		uint32_t nHead = GetHead();
		//len���ϲ��û����ݵĳ��ȣ�sizeof(uint32_t)������ʱ�䳤���ֽ��ڻ������еĳ���
		nHead = (nHead + len + sizeof(uint32_t))%GetCapacity();

		return SetHead(nHead);
	}


	//�Ӷ���ͷ����ȡһ�����ݰ�
	int32_t Pop(uint8_t* pBuffer, uint32_t nSize, uint32_t& nLength)
	{
		if ((NULL == pBuffer) || (0 >= nSize))
		{
			return E_INVALIDARGUMENT;
		}

		//��ȡ�����׵�ַ�Ͷ��пռ��ܴ�С
		uint8_t* pCodeBuffer = GetCodeBuffer();
		uint32_t nCapacity = GetCapacity();

		if ((NULL == pCodeBuffer) || (nCapacity <= 0))
		{
			return E_QUEUENOTINITIALIZED;
		}
		if (IsEmpty())
		{
			return E_EMPTYQUEUE;
		}

		//��ȡ������βλ��
		uint32_t nHead = 0;
		uint32_t nTail = 0;
		uint32_t nUsedSize = 0;
		GetUsedInfo(nHead, nTail, nUsedSize);

		//�����βλ���Ƿ���Ч
		if ((nHead >= nCapacity) || (nTail >= nCapacity))
		{
			SetBoundary(0, 0);
			return E_QUEUESHATTERED;
		}
		//������ʹ�ÿռ�С��4���ֽ�
		if (nUsedSize < sizeof(uint32_t))
		{
			SetBoundary(0, 0);
			return E_QUEUESHATTERED;
		}

		//ʹ��nRealSize�������ݰ�����
		uint32_t nRealSize = 0;
		uint8_t* p = (uint8_t*)&nRealSize;

		//��ȡ���ݰ�����
		for (size_t i = 0; i < sizeof(uint32_t); ++i)
		{
			p[i] = pCodeBuffer[nHead];
			nHead = (nHead + 1) % nCapacity;
		}

		//�Ӷ����ж�ȡ�����ݰ����ȳ��Ȳ�����Ҫ��
		if ((nRealSize <= 0) || (nRealSize > (nUsedSize - sizeof(uint32_t))))
		{
			SetBoundary(0, 0);
			return E_INVALIDPACKAGE;
		}

		//������������Ȳ���
		if (nSize < nRealSize)
		{
			//���������ݰ�
			nHead = (nHead + nRealSize) % nCapacity;
			SetHead(nHead);
			return E_INVALIDPACKAGE;
		}

		//�Ӷ�����ȡ�����ݰ�
		nLength = nRealSize;

		if (nTail > nHead)
		{
			memcpy(pBuffer, pCodeBuffer + nHead, nRealSize);
		}
		else
		{
			if (nRealSize > (nCapacity - nHead))
			{
				//��Ҫ�ֶο���
				memcpy(pBuffer, pCodeBuffer + nHead, nCapacity - nHead);
				memcpy(pBuffer + (nCapacity - nHead), pCodeBuffer, nRealSize - (nCapacity - nHead));
			}
			else
			{
				memcpy(pBuffer, pCodeBuffer + nHead, nRealSize);
			}

		}

		nHead = (nHead + nRealSize) % nCapacity;

		//���Ķ���ͷ��λ��
		return SetHead(nHead);
	}

	//�ж϶����Ƿ�����
	bool IsFull()
	{
		return (GetFreeSize() <= 0);
	}
	//�ж������Ƿ�Ϊ��
	bool IsEmpty()
	{
		return (GetUsedSize() <= 0);
	}

protected:
	//���ö���ͷ��λ��
	int32_t SetHead(uint32_t nHead)
	{
		uint32_t nCapacity = GetCapacity();

		if (nHead > nCapacity)
		{
			return E_INVALIDHEAD;
		}

		m_nHead = nHead;
		return S_OK;
	}
	//���ö���β��λ��
	int32_t SetTail(uint32_t nTail)
	{
		uint32_t nCapacity = GetCapacity();

		if (nTail > nCapacity)
		{
			return E_INVALIDTAIL;
		}

		m_nTail = nTail;
		return S_OK;
	}
	//���ö�����βλ��
	int32_t SetBoundary(uint32_t nHead, uint32_t nTail)
	{
		uint32_t nCapacity = GetCapacity();

		if (nHead > nCapacity)
		{
			return E_INVALIDHEAD;
		}
		if (nTail > nCapacity)
		{
			return E_INVALIDTAIL;
		}

		m_nHead = nHead;
		m_nTail = nTail;
		return S_OK;
	}
	//��ȡ����ͷ��λ��
	uint32_t GetHead() const
	{
		return m_nHead;
	}
	//��ȡ����β��λ��
	uint32_t GetTail() const
	{
		return m_nTail;
	}
	//��ȡ������βλ��
	void GetBoundary(uint32_t& nHead, uint32_t& nTail) const
	{
		nHead = m_nHead;
		nTail = m_nTail;
	}

	void GetFreeInfo(uint32_t& nHead, uint32_t& nTail, uint32_t& nFreeSize)
	{
		uint32_t nCapacity = GetCapacity();
		if (nCapacity <= enmReservedCodeLength)
		{
			return;
		}
		nHead = GetHead();
		nTail = GetTail();
		nFreeSize = ((nHead == nTail) ? nCapacity : ((nHead + nCapacity - nTail) % nCapacity));

		//ȥ��Ԥ���Ŀռ�
		if (nFreeSize < (uint32_t)enmReservedCodeLength)
		{
			nFreeSize = 0;
		}
		else
		{
			nFreeSize = nFreeSize - (uint32_t)enmReservedCodeLength;
		}
	}

	//���ض��п��ÿռ��С
	uint32_t GetFreeSize() const
	{
		uint32_t nHead = GetHead();
		uint32_t nTail = GetTail();
		uint32_t nCapacity = GetCapacity();

		if (nCapacity <= enmReservedCodeLength)
		{
			return 0;
		}

		uint32_t nFreeSize = ((nHead == nTail) ? nCapacity : ((nHead + nCapacity - nTail) % nCapacity));

		//ȥ��Ԥ���Ŀռ�
		if (nFreeSize < (uint32_t)enmReservedCodeLength)
		{
			return 0;
		}
		return nFreeSize - (uint32_t)enmReservedCodeLength;
	}
	void GetUsedInfo(uint32_t& nHead, uint32_t& nTail, uint32_t& nUsedSize)
	{
		uint32_t nCapacity = GetCapacity();

		if (nCapacity <= enmReservedCodeLength)
		{
			return;
		}

		nHead = GetHead();
		nTail = GetTail();

		nUsedSize = (nTail + nCapacity - nHead) % nCapacity;
	}
	//���ض�����ʹ�ÿռ��С
	uint32_t GetUsedSize()
	{
		uint32_t nHead = GetHead();
		uint32_t nTail = GetTail();
		uint32_t nCapacity = GetCapacity();

		if (nCapacity <= enmReservedCodeLength)
		{
			return 0;
		}

		uint32_t nUsedSize = (nTail + nCapacity - nHead) % nCapacity;

		return nUsedSize;
	}
	//���ض��пռ��ܴ�С
	uint32_t GetCapacity() const
	{
		return CAPACITY;
	}
	//�����������׵�ַ
	uint8_t* GetCodeBuffer() const
	{
		return (uint8_t*)m_arrCodeBuffer;
	}


protected:
	//�������ܳ���
	uint32_t				m_nSize;
	//����������ʼλ��
	uint32_t				m_nHead;
	//�������ݽ���λ��
	uint32_t				m_nTail;
	//ƫ����
	uint32_t				m_nOffset;
	//������
	uint8_t					m_arrCodeBuffer[CAPACITY];

};

#endif /* COMMON_CODEQUEUE_H_ */
