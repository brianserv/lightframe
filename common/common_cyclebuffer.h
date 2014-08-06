/*
 * common_cyclebuffer.h
 *
 *  Created on: 2012-11-20
 *      Author: jimm
 */

#ifndef COMMON_CYCLEBUFFER_H_
#define COMMON_CYCLEBUFFER_H_

#include "common_mutex.h"

template<int32_t SIZE>
class CycleBuffer
{
public:
	CycleBuffer()
	{
		Reset();
	}

	virtual ~CycleBuffer()
	{

	}

	//д��д�б��ͷ���ڴ�
	int32_t WriteToHead(const uint8_t *pBuf, int32_t nDataSize)
	{
		if(nDataSize <= 0 || pBuf == NULL)
		{
			return 0;
		}

		MUTEX_GUARD(lock, m_stLock);
		//ʣ��ռ䲻��
		if(m_nDataSize + nDataSize > SIZE)
		{
			return -1;
		}

		if(m_nWriteIndex < m_nReadIndex)
		{
			memcpy(&m_arrBuf[m_nReadIndex - nDataSize], pBuf, nDataSize);
			m_nReadIndex -= nDataSize;
		}
		//�����������һ�㶼�ǳ�ʼֵ��ʱ��
		else if(m_nWriteIndex == m_nReadIndex)
		{
			return Write(pBuf, nDataSize);
		}
		else
		{
			int32_t nLeftDataSize = (m_nReadIndex - nDataSize < 0) ? m_nReadIndex : nDataSize;
			if(nLeftDataSize < nDataSize)
			{
				int32_t nTailDataSize = nDataSize - nLeftDataSize;
				int32_t nWriteIndex = SIZE - nTailDataSize;
				memcpy(&m_arrBuf[nWriteIndex], pBuf, nTailDataSize);
				memcpy(&m_arrBuf[0], pBuf + nTailDataSize, nLeftDataSize);
				m_nWriteIndex = nWriteIndex;
			}
			else
			{
				if(m_nReadIndex - nDataSize < 0)
				{
					return 0;
				}

				memcpy(&m_arrBuf[m_nReadIndex - nDataSize], pBuf, nDataSize);
				m_nReadIndex -= nDataSize;
			}
		}

		m_nDataSize += nDataSize;

		return nDataSize;
	}

	//д��д�б��β���ڴ�
	int32_t Write(const uint8_t *pBuf, int32_t nDataSize)
	{
		if(nDataSize <= 0 || pBuf == NULL)
		{
			return 0;
		}

		MUTEX_GUARD(lock, m_stLock);
		//ʣ��ռ䲻��
		if(m_nDataSize + nDataSize > SIZE)
		{
			return -1;
		}

		if(m_nWriteIndex < m_nReadIndex)
		{
			memcpy(&m_arrBuf[m_nWriteIndex], pBuf, nDataSize);
			m_nWriteIndex += nDataSize;
		}
		else
		{
			int32_t nLeftDataSize = SIZE - m_nWriteIndex;
			if(nLeftDataSize < nDataSize)
			{
				memcpy(&m_arrBuf[m_nWriteIndex], pBuf, nLeftDataSize);
				memcpy(&m_arrBuf[0], pBuf + nLeftDataSize, nDataSize - nLeftDataSize);
				m_nWriteIndex = nDataSize - nLeftDataSize;
			}
			else
			{
				memcpy(&m_arrBuf[m_nWriteIndex], pBuf, nDataSize);
				m_nWriteIndex += nDataSize;
			}
		}

		m_nDataSize += nDataSize;

		return nDataSize;
	}

	//��ȡ���б��β���ڴ�
	int32_t ReadFromTail(uint8_t *pBuf, const int32_t nWantSize)
	{
		if(nWantSize <= 0 || pBuf == NULL)
		{
			return 0;
		}

		MUTEX_GUARD(lock, m_stLock);

		int32_t nDataSize = ((m_nDataSize < nWantSize) ? m_nDataSize : nWantSize);
		if(m_nReadIndex < m_nWriteIndex)
		{
			if(m_nWriteIndex - nDataSize < 0)
			{
				return 0;
			}

			memcpy(pBuf, &m_arrBuf[m_nWriteIndex - nDataSize], nDataSize);
			m_nWriteIndex -= nDataSize;
		}
		else if(m_nReadIndex == m_nWriteIndex)
		{
			return 0;
		}
		else
		{
			if(m_nWriteIndex - nWantSize >= 0)
			{
				memcpy(pBuf, &m_arrBuf[m_nWriteIndex - nWantSize], nWantSize);
				m_nWriteIndex -= nWantSize;
			}
			else
			{
				int32_t nTailDataSize = nDataSize - m_nWriteIndex;
				int32_t nWriteIndex = SIZE - nTailDataSize;
				memcpy(pBuf, &m_arrBuf[nWriteIndex], nTailDataSize);
				memcpy(&pBuf[nTailDataSize], &m_arrBuf[0], m_nWriteIndex);
				m_nWriteIndex = nWriteIndex;
			}
		}

		m_nDataSize -= nDataSize;

		return nDataSize;
	}

	//��ȡ���б��ͷ���ڴ�
	int32_t Read(uint8_t *pBuf, const int32_t nWantSize)
	{
		if(nWantSize <= 0 || pBuf == NULL)
		{
			return 0;
		}

		MUTEX_GUARD(lock, m_stLock);

		int32_t nDataSize = ((m_nDataSize < nWantSize) ? m_nDataSize : nWantSize);
		if(nDataSize <= 0)
		{
			return 0;
		}

		if(m_nReadIndex < m_nWriteIndex)
		{
			memcpy(pBuf, &m_arrBuf[m_nReadIndex], nDataSize);
			m_nReadIndex += nDataSize;
		}
		else
		{
			int32_t nLeftDataSize = SIZE - m_nReadIndex;
			if(nLeftDataSize < nDataSize)
			{
				memcpy(pBuf, &m_arrBuf[m_nReadIndex], nLeftDataSize);
				memcpy(pBuf + nLeftDataSize, &m_arrBuf[0], nDataSize - nLeftDataSize);
				m_nReadIndex = nDataSize - nLeftDataSize;
			}
			else
			{
				memcpy(pBuf, &m_arrBuf[m_nReadIndex], nDataSize);
				m_nReadIndex += nDataSize;
			}
		}

		m_nDataSize -= nDataSize;

		return nDataSize;
	}

	//��ȡ���ݵ��ǲ��޸Ķ�����
	int32_t PeekRead(uint8_t *pBuf, const int32_t nWantSize)
	{
		if(nWantSize <= 0 || pBuf == NULL)
		{
			return 0;
		}

		MUTEX_GUARD(lock, m_stLock);

		int32_t nDataSize = ((m_nDataSize < nWantSize) ? m_nDataSize : nWantSize);
		if(m_nReadIndex < m_nWriteIndex)
		{
			memcpy(pBuf, &m_arrBuf[m_nReadIndex], nDataSize);
		}
		else if(m_nReadIndex == m_nWriteIndex)
		{
			return 0;
		}
		else
		{
			int32_t nLeftDataSize = SIZE - m_nReadIndex;
			if(nLeftDataSize < nDataSize)
			{
				memcpy(pBuf, &m_arrBuf[m_nReadIndex], nLeftDataSize);
				memcpy(pBuf + nLeftDataSize, &m_arrBuf[0], nDataSize - nLeftDataSize);
			}
			else
			{
				memcpy(pBuf, &m_arrBuf[m_nReadIndex], nDataSize);
			}
		}

		return nDataSize;
	}

	void Reset()
	{
		MUTEX_GUARD(lock, m_stLock);
		m_nReadIndex = 0;
		m_nWriteIndex = 0;
		m_nDataSize = 0;
		memset(m_arrBuf, 0, sizeof(m_arrBuf));
	}

	int32_t Size()
	{
		MUTEX_GUARD(lock, m_stLock);
		return m_nDataSize;
	}

	int32_t Capatity()
	{
		return SIZE;
	}

	int32_t Space()
	{
		MUTEX_GUARD(lock, m_stLock);
		return SIZE - m_nDataSize;
	}

protected:
	CriticalSection			m_stLock;
	volatile int32_t		m_nReadIndex;
	volatile int32_t		m_nWriteIndex;
	volatile int32_t		m_nDataSize;
	uint8_t					m_arrBuf[SIZE];
};

#endif /* COMMON_CYCLEBUFFER_H_ */
