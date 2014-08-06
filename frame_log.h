/*
 * frame_log.h
 *
 *  Created on: 2013-1-2
 *      Author: jimm
 */

#ifndef FRAME_LOG_H_
#define FRAME_LOG_H_

#include "common/common_object.h"
#include "common/common_logqueue.h"
#include "common/common_singleton.h"
#include "frame_namespace.h"
#include "frame_def.h"
#include "frame_logqueue.h"


FRAME_NAMESPACE_BEGIN

typedef CLogQueue<enmMaxLogQueueSize>		UserLogQueue;

class CFrameLog : public CObject
{
public:
	CFrameLog();
	virtual ~CFrameLog();

public:
	//��ʼ����־����
	virtual int32_t Initialize();
	//�ָ���־����
	virtual int32_t Resume();
	//ע����־����
	virtual int32_t Uninitialize();

public:
	//�Ӷ���β��׷��һ����־
	virtual int32_t Push(const char *szLog, const char *szName, const int32_t nID1, const int32_t nID2, const char* szDate);
	//�Ӷ���ͷ����ȡһ����־
	virtual int32_t Pop(char* szLog, uint32_t nLogSize, char *szName, int32_t& nID1, int32_t& nID2, char* szDate);
	//�ж϶����Ƿ�����
	virtual bool IsFull();
	//�ж������Ƿ�Ϊ��
	virtual bool IsEmpty();

protected:
	UserLogQueue		m_stUserLogQueue;
};

FRAME_NAMESPACE_END

#endif /* FRAME_LOG_H_ */
