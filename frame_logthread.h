/*
 * frame_logthread.h
 *
 *  Created on: 2011-11-22
 *      Author: jimm
 */

#ifndef FRAME_LOGTHREAD_H_
#define FRAME_LOGTHREAD_H_

#include "common/common_thread.h"
#include "common/common_singleton.h"
#include "frame_namespace.h"
#include "frame_def.h"

FRAME_NAMESPACE_BEGIN


class CFrameLogThread : public CThread
{
public:
	enum
	{
		enmMaxLogFileNameCount		= 1000,			//��־�ļ����������
		enmWriteLogPeriod			= 1000*60,		//��־�ļ�����ǻ�Ծʱ��(ms),��ʱ��Ϊ1����
		enmFlushPeriod				= 1000,			//�̶�ˢ�����ڣ���ʱ��Ϊ1s
		enmInvalidLogFileIndex		= 0xFFFFFFFF,	//��Ч��־�ļ����
	};
public:
	CFrameLogThread();
	virtual ~CFrameLogThread();

public:
	//��ʼ����־�߳�
	int32_t Initialize();
	//�ָ���־�߳�
	int32_t Resume();
	//ע����־�߳�
	int32_t Uninitialize();

public:
	//�߳���ں���
	virtual void Execute();

	//����־д�뵽�ļ�
	void WriteToLogFile(LogSource logSource, int32_t id1, int32_t id2, const char* szDate, const char* szLog);

	//����־д�뵽�ļ�
	void WriteToLogFile(const char *szName, int32_t id1, int32_t id2, const char* szDate, const char* szLog);

protected:
	//����־�ļ�״̬
	LogFileState		m_arrLogFileState[enmMaxLogFileNameCount];

};

#define	CREATE_FRAMELOGTHREAD_INSTANCE		CSingleton<CFrameLogThread>::CreateInstance
#define	g_FrameLogThread					CSingleton<CFrameLogThread>::GetInstance()
#define	DESTROY_FRAMELOGTHREAD_INSTANCE		CSingleton<CFrameLogThread>::DestroyInstance

FRAME_NAMESPACE_END


#endif /* FRAME_LOGTHREAD_H_ */
