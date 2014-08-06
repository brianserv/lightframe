/*
 * frame_thread_mgt.h
 *
 *  Created on: 2013-2-27
 *      Author: jimm
 */

#ifndef FRAME_THREAD_MGT_H_
#define FRAME_THREAD_MGT_H_

#include "common/common_thread.h"
#include "common/common_singleton.h"
#include "common/common_mutex.h"
#include "frame_namespace.h"

FRAME_NAMESPACE_BEGIN

#define MAX_FRAME_THREAD_COUNT	32

class CFrameThreadMgt
{
public:
	CFrameThreadMgt();

	virtual ~CFrameThreadMgt();

	void RegistThread(int32_t nThreadID, CThread *pThread);

	CThread *GetCurThread();

	CThread *GetThread(int32_t nThreadID);

protected:
//	CriticalSection	m_stThreadMgtLock;

	int32_t			m_nThreadCount;
	int32_t			m_arrThreadID[MAX_FRAME_THREAD_COUNT];
	CThread			*m_arrThread[MAX_FRAME_THREAD_COUNT];
};

#define	CREATE_FRAMETRHEADMGT_INSTANCE			CSingleton<CFrameThreadMgt>::CreateInstance
#define	g_FrameThreadMgt						CSingleton<CFrameThreadMgt>::GetInstance()
#define	DESTROY_FRAMETHREADMGT_INSTANCE			CSingleton<CFrameThreadMgt>::DestroyInstance

FRAME_NAMESPACE_END

#endif /* FRAME_THREAD_MGT_H_ */
