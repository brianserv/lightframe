/*
 * frame_logqueue.h
 *
 *  Created on: 2011-11-22
 *      Author: jimm
 */

#ifndef FRAME_LOGQUEUE_H_
#define FRAME_LOGQUEUE_H_

#include "common/common_object.h"
#include "common/common_logqueue.h"
#include "common/common_memory.h"
#include "common/common_singleton.h"
#include "common/common_mutex.h"
#include "frame_namespace.h"
#include "frame_def.h"
#include "lightframe_impl.h"


FRAME_NAMESPACE_BEGIN


typedef CLogQueue<enmMaxLogQueueSize>		LogQueue;

class CFrameLogQueue
{
public:
	CFrameLogQueue();
	virtual ~CFrameLogQueue();

	virtual size_t GetSize();
	virtual const char* GetName();

public:
	//��ʼ����־����
	virtual int32_t Initialize();
	//�ָ���־����
	virtual int32_t Resume();
	//ע����־����
	virtual int32_t Uninitialize();

public:
	//�Ӷ���β��׷��һ����־
	int32_t Push(ThreadType threadType, const char *szLog, const LogSource logSource, const int32_t nID1, const int32_t nID2, const char* szDate);
	//�Ӷ���ͷ����ȡһ����־
	int32_t Pop(ThreadType threadType, char* szLog, uint32_t nLogSize, LogSource& logSource, int32_t& nID1, int32_t& nID2, char* szDate);
	//�ж϶����Ƿ�����
	bool IsFull(ThreadType threadType);
	//�ж������Ƿ�Ϊ��
	bool IsEmpty(ThreadType threadType);

protected:
	LogQueue				m_stLogQueueNetioThread;
	CriticalSection			m_stMainLogLock;
	LogQueue				m_stLogQueueMainThread;
	LogQueue				m_stLogQueueDBAccessThread;
};


#define	CREATE_FRAMELOGQUEUE_INSTANCE		CSingleton<CFrameLogQueue>::CreateInstance
#define	g_FrameLogQueue						CSingleton<CFrameLogQueue>::GetInstance()
#define	DESTROY_FRAMELOGQUEUE_INSTANCE		CSingleton<CFrameLogQueue>::DestroyInstance



FRAME_NAMESPACE_END


#endif /* FRAME_LOGQUEUE_H_ */
