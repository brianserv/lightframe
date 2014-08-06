/*
 * common_memory.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_MEMORY_H_
#define COMMON_MEMORY_H_

#include <stdio.h>
#include <time.h>
#include <new>

#ifdef WIN32
#else
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#endif

#include "common_typedef.h"


#ifdef WIN32
typedef int key_t;
#endif


enum
{
	enmShmMode_Init			= 0x00,
	enmShmMode_Resume		= 0x01,
};


class CShareMemory
{
public:
	static uint8_t*	ms_pMemory;

public:
	CShareMemory();
	CShareMemory(key_t key, size_t nSize, uint8_t nShmMode);
	~CShareMemory();

public:
	//new����������
	void*	operator new(size_t nSize) throw();
	//delete����������
	void	operator delete(void *pShareMemory);

	//�����СΪnSize���ڴ��
	void*	CreateSegment(size_t nSize);

	//��ȡʣ��ռ��С(��λ: Byte)
	size_t	GetFreeSize();
	//��ȡ��ʹ�ÿռ��С(��λ: Byte)
	size_t	GetUsedSize();

	//���ù����ڴ�ʱ���
	void	SetTimeStamp();
	uint8_t	GetMode();

protected:
	int32_t Initialize(key_t key, size_t nSize);
	int32_t Resume(key_t key, size_t nSize);

protected:
	key_t			m_key;					//�����ڴ��
	size_t			m_nTotalSize;			//�ڴ��ܴ�С
	uint32_t		m_nCRC;					//�ڴ�ͷУ���
	time_t			m_tmCreateTime;			//�ڴ洴��ʱ��
	time_t			m_tmLastTimeStamp;		//������ʱ��
	uint8_t			m_nShmMode;				//�ڴ�ʹ��ģʽ
	uint8_t*		m_pAvailableMemory;		//�����ڴ�

};


//���������ڴ�
extern CShareMemory*	CreateShareMemory(key_t key, size_t nSize, uint8_t nShmMode = 0);
//���ٹ����ڴ�
extern int32_t			DestroyShareMemory(key_t key);


#endif /* COMMON_MEMORY_H_ */
