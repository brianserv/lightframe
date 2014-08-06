/*
 * common_thread.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_THREAD_H_
#define COMMON_THREAD_H_

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif

#include "common_typedef.h"


#define THREAD_TERMINATE_TIMEOUT	2000

enum
{
	enmThread_ThreadNameLength = 1024,
	enmThread_ThreadLogNameLength = 1024,
};

class CThread
{
public:
	CThread();
	virtual ~CThread();

public:
	//�����߳�
	virtual int32_t	Start();
	//�����߳�
	virtual int32_t	Terminate();

	//�߳���ں���
	virtual void	Execute();

	//�߳��Ƿ����
	bool		GetTerminated();
	void		SetTerminated(bool bTerminated);

	bool		SetPriority(int nPriority);
	int			GetPriority();

	void		SetThreadIndex(int32_t nThreadIndex)
	{
		m_nThreadIndex = nThreadIndex;
	}

	int32_t		GetThreadIndex()
	{
		return m_nThreadIndex;
	}

	int32_t	SetThreadData(const void *pData);

	void *GetThreadData();

	const char* GetName();
	void SetName(const char* name);

	const char* GetLogName();
	void SetLogName(const char* log_name);

	static bool		IsSetPrivateKeyLog();
	static void		SetPrivateKeyLog();
	static int32_t InitPrivateKeyLog();
	static CThread* GetLogCtxByPrivateKey();

protected:
#ifdef WIN32
	uint32_t		m_nTimeOut;
	HANDLE			m_hThread;
#else
	pthread_t		m_thread;
	pthread_mutex_t m_stMutex;
#endif

	bool			m_bTerminated;
	int32_t			m_nThreadIndex;

	/**
	���߳���־�й�
	*/
	static bool ms_bIsSetPrivateKeyLog;
	char	m_szName[enmThread_ThreadNameLength];
	char m_szLogName[enmThread_ThreadLogNameLength];

#ifdef WIN32

#else
	static pthread_key_t		ms_private_key_log;
#endif


	pthread_key_t			m_stThreadDataKey;
};

#endif /* COMMON_THREAD_H_ */
