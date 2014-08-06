/*
 * frame_mainthread.h
 *
 *  Created on: 2011-11-22
 *      Author: jimm
 */

#ifndef FRAME_MAINTHREAD_H_
#define FRAME_MAINTHREAD_H_

#include "common/common_singleton.h"
#include "common/common_thread.h"
#include "frame_namespace.h"
#include "frame_def.h"
#include "common/common_mutex.h"

FRAME_NAMESPACE_BEGIN

class CFrameMainThread : public CThread
{
public:
	CFrameMainThread();
	virtual ~CFrameMainThread();

public:
	//��ʼ�����ݿ�����߳�
	int32_t Initialize();
	//�ָ����ݿ�����߳�
	int32_t Resume();
	//ע�����ݿ�����߳�
	int32_t Uninitialize();

public:
	//�߳���ں���
	virtual void Execute();

protected:
	//Logicģʽ�µ�����ͨѶ����
	bool ProcessSSLogicData();

	bool ProcessCSLogicData();

	bool ProcessTestMessage();

	//����ʱ���¼�
	bool ProcessTimerEvent();
	//�����������ͨѶ����
	bool ProcessSSMessage(uint8_t *pBuf, uint32_t nLength);
	//����ͻ��˺ͷ�����֮��ͨѶ����
	bool ProcessCSMessage(uint8_t *pBuf, uint32_t nLength);
	//ϵͳ�¼�
	int32_t OnSystemEvent(uint16_t nEventID, void *pParam);
	//��������Ϣ�¼�
	int32_t OnMessageEvent(MessageHeadSS* pMsgHead, const uint16_t nMsgHeadOffset,
			const uint8_t* buf, const uint32_t size);
	//�ͻ�����Ϣ�¼�
	int32_t OnMessageEvent(MessageHeadCS* pMsgHead, const uint16_t nMsgHeadOffset,
			const uint8_t* buf, const uint32_t size, const int32_t nOptionLen, const void *pOption);

	IMsgBody *CreateMsgBody(uint32_t nMsgID);

	void DestroyMsgBody(IMsgBody *pMsgBody);

protected:
//	CriticalSection			m_stMsgBodyLock;
};

//#define	CREATE_FRAMEMAINTHREAD_INSTANCE		CSingleton<CFrameMainThread>::CreateInstance
//#define	g_FrameMainThread					CSingleton<CFrameMainThread>::GetInstance()
//#define	DESTROY_FRAMEMAINTHREAD_INSTANCE	CSingleton<CFrameMainThread>::DestroyInstance

FRAME_NAMESPACE_END


#endif /* FRAME_MAINTHREAD_H_ */
