/*
 * frame_msgevent_mgt.h
 *
 *  Created on: 2011-11-22
 *      Author: jimm
 */

#ifndef FRAME_MSGEVENT_MGT_H_
#define FRAME_MSGEVENT_MGT_H_

#include "common/common_object.h"
#include "common/common_pool.h"
#include "common/common_hashlist.h"
#include "common/common_singleton.h"
#include "common/common_mutex.h"
#include "frame_namespace.h"
#include "frame_def.h"
#include "lightframe_impl.h"


FRAME_NAMESPACE_BEGIN

//�����Ϣע������
#ifndef MAX_MSGEVENT_COUNT
#define MAX_MSGEVENT_COUNT			0x0000040		//64
#endif

//���ϵͳ�¼�ע������
#ifndef MAX_SYSEVENT_COUNT
#define MAX_SYSEVENT_COUNT			0x0000010		//16
#endif

//���ϵͳ�¼�ע������
#ifndef MAX_DEFEVENT_COUNT
#define MAX_DEFEVENT_COUNT			0x0000100		//256
#endif

typedef CPool<int32_t, MAX_BINDING_SESSION_COUNT> SessionBindingList;

//Message���¼��ӿ�ע����Ϣ
typedef struct tagMsgEventInfo
{
	uint32_t			MsgID;
	IMessageEventSS*	pHandleSS;
	SS_MSG_PROC			SSMsgProc;
	IMessageEventCS*	pHandleCS;
	CS_MSG_PROC			CSMsgProc;
}MsgEventInfo;

//�Ự��MsgID�İ󶨹�ϵ
typedef struct tagBindingMsg
{
	uint32_t			MsgID;
	SessionBindingList	bindingList;
}BindingMsg;

//Message���¼��ӿ�ע����Ϣ
typedef struct tagDefaultEventInfo
{
	uint32_t			MsgID;
	IDefaultMessageSS*	pHandleSS;
	IDefaultMessageCS*	pHandleCS;
}DefaultEventInfo;

typedef struct tagSystemEventInfo
{
	uint16_t			EventID;
	SYS_EVENT_PROC		SysEventProc;
	ISystemEvent* 		pHandle;
}SystemEventInfo;

class CFrameMsgEventMgt : public CObject
{
public:
	CFrameMsgEventMgt()
	{
	}
	virtual ~CFrameMsgEventMgt()
	{

	}

public:
	//��ʼ��MsgEvent������
	virtual int32_t Initialize()
	{
		m_nMsgEventCount = 0;
		memset(m_arrMsgEventInfo, 0, sizeof(m_arrMsgEventInfo));

		m_nDefaultEventCount = 0;
		memset(m_arrDefaultEventInfo, 0, sizeof(m_arrDefaultEventInfo));

		MUTEX_GUARD(Lock, m_stSessionBindMsgLock);
		m_nSessionBingMsgCount = 0;
		for(int32_t i = 0; i < MAX_MSGEVENT_COUNT; i++)
		{
			m_arrSessionBingMsg[i].MsgID = 0;
			m_arrSessionBingMsg[i].bindingList.Initialize();
		}

		return S_OK;
	}
	//�ָ�MsgEvent������
	virtual int32_t Resume()
	{
		return Initialize();
	}
	//ע��MsgEvent������
	virtual int32_t Uninitialize()
	{
		Clear();
		return S_OK;
	}

	//��ջỰ����Ϣ������
	int32_t Clear()
	{
		MUTEX_GUARD(Lock, m_stSessionBindMsgLock);
		m_nSessionBingMsgCount = 0;
		for(int32_t i = 0; i < MAX_MSGEVENT_COUNT; i++)
		{
			m_arrSessionBingMsg[i].MsgID = 0;
			m_arrSessionBingMsg[i].bindingList.Clear();
		}
		return S_OK;
	}

	//ע����Ϣӳ��
	void AddMsgEvent(uint32_t MsgID, IMessageEventSS* pHandle, SS_MSG_PROC proc)
	{
		if(NULL != pHandle && m_nMsgEventCount < MAX_MSGEVENT_COUNT)
		{
			m_arrMsgEventInfo[m_nMsgEventCount].MsgID = MsgID;
			m_arrMsgEventInfo[m_nMsgEventCount].pHandleSS = pHandle;
			m_arrMsgEventInfo[m_nMsgEventCount].pHandleCS = NULL;
			m_arrMsgEventInfo[m_nMsgEventCount].SSMsgProc = proc;
			m_arrMsgEventInfo[m_nMsgEventCount].CSMsgProc = NULL;
			m_nMsgEventCount++;
		}
	}

	//ע����Ϣӳ��
	void AddMsgEvent(uint32_t MsgID, IMessageEventCS* pHandle, CS_MSG_PROC proc)
	{
		if(NULL != pHandle && m_nMsgEventCount < MAX_MSGEVENT_COUNT)
		{
			m_arrMsgEventInfo[m_nMsgEventCount].MsgID = MsgID;
			m_arrMsgEventInfo[m_nMsgEventCount].pHandleSS = NULL;
			m_arrMsgEventInfo[m_nMsgEventCount].pHandleCS = pHandle;
			m_arrMsgEventInfo[m_nMsgEventCount].SSMsgProc = NULL;
			m_arrMsgEventInfo[m_nMsgEventCount].CSMsgProc = proc;
			m_nMsgEventCount++;
		}
	}

	void AddSysEvent(uint16_t EventID, ISystemEvent* pHandle)
	{
		if(pHandle != NULL && m_nSysEventCount < MAX_SYSEVENT_COUNT)
		{
			m_arrSysEventInfo[m_nSysEventCount].EventID = EventID;
			m_arrSysEventInfo[m_nSysEventCount].SysEventProc = NULL;
			m_arrSysEventInfo[m_nSysEventCount].pHandle = pHandle;
			++m_nSysEventCount;
		}
	}

	//������Ϣӳ��
	MsgEventInfo * GetMessageEvent(uint32_t MsgID)
	{
		for(int32_t i = 0; i < m_nMsgEventCount; i++)
		{
			if(MsgID == m_arrMsgEventInfo[i].MsgID)
			{
				return &m_arrMsgEventInfo[i];
			}
		}

		return NULL;
	}

	//���Ҹ�MsgID�󶨵Ķ��ӳ��
	int32_t GetMessageEvent(const uint32_t MsgID, MsgEventInfo * arrPMsgEventInfo[MAX_MSGEVENT_COUNT], int32_t& nEventCount)
	{
		int32_t nRet = S_OK;
		if (NULL == arrPMsgEventInfo)
		{
			return E_UNKNOWN;
		}

		nEventCount = 0;
		for(int32_t i = 0; i < m_nMsgEventCount; i++)
		{
			if(MsgID == m_arrMsgEventInfo[i].MsgID)
			{
				arrPMsgEventInfo[nEventCount] = &m_arrMsgEventInfo[i];
				nEventCount ++;
			}
		}

		return nRet;
	}

	SystemEventInfo *GetSystemEventProc(uint16_t nEventID)
	{
		for(uint16_t i = 0; i < m_nSysEventCount; ++i)
		{
			if(m_arrSysEventInfo[i].EventID == nEventID)
			{
				return &m_arrSysEventInfo[i];
			}
		}

		return NULL;
	}

	//ע��Ự��Ϣ��
	void AddSessionBindMsg(const int32_t sessionindex, const uint32_t MsgID)
	{
		MUTEX_GUARD(Lock, m_stSessionBindMsgLock);

		SessionBindingList *pBindingList = NULL;
		for(int32_t i = 0; i < m_nSessionBingMsgCount; i++)
		{
			if(m_arrSessionBingMsg[i].MsgID == MsgID)
			{
				pBindingList = &m_arrSessionBingMsg[i].bindingList;
				break;
			}
		}

		if((NULL == pBindingList) && m_nSessionBingMsgCount < MAX_MSGEVENT_COUNT)
		{
			pBindingList = &m_arrSessionBingMsg[m_nSessionBingMsgCount].bindingList;
			m_arrSessionBingMsg[m_nSessionBingMsgCount].MsgID = MsgID;

			m_nSessionBingMsgCount++;
		}

#if defined(DEBUG) || defined(_DEBUG) || defined(_DEBUG_)
		//����Ƿ��ظ�ע��
#endif

		if(NULL != pBindingList)
		{
			SessionBindingList::CIndex *pIndex = pBindingList->CreateObject();
			if(NULL != pIndex)
			{
				pIndex->Object() = sessionindex;
			}
		}
	}

	//�����Ϣ��Ự�İ󶨹�ϵ
	void RemoveSessionBindMsg(const int32_t sessionindex, const uint32_t MsgID)
	{
		MUTEX_GUARD(Lock, m_stSessionBindMsgLock);

		for(int32_t i = 0; i < m_nSessionBingMsgCount; i++)
		{
			if(m_arrSessionBingMsg[i].MsgID == MsgID)
			{
				SessionBindingList &BindingList = m_arrSessionBingMsg[i].bindingList;

				SessionBindingList::CIndex *pIndex = NULL;
				for(pIndex = BindingList.First(); NULL != pIndex; pIndex = pIndex->Next())
				{
					if(pIndex->Object() == sessionindex)
					{
						break;
					}
				}

				if(NULL != pIndex)
				{
					BindingList.DestroyObject(pIndex->Index());
				}

				break;
			}
		}
	}


	bool IsSessionBindingMsg(const int32_t sessionindex, const uint32_t MsgID)
	{
		MUTEX_GUARD(Lock, m_stSessionBindMsgLock);

		for(int32_t i = 0; i < m_nSessionBingMsgCount; i++)
		{
			if(m_arrSessionBingMsg[i].MsgID == MsgID)
			{
				SessionBindingList &BindingList = m_arrSessionBingMsg[i].bindingList;

				for(SessionBindingList::CIndex *pIndex = BindingList.First(); NULL != pIndex; pIndex = pIndex->Next())
				{
					if(pIndex->Object() == sessionindex)
					{
						return true;
					}
				}
				break;
			}
		}

		return false;
	}

	//ע����Ϣӳ��
	void AddDefEvent(uint32_t MsgID, IDefaultMessageSS* pHandle)
	{
		if(NULL != pHandle && m_nDefaultEventCount < MAX_DEFEVENT_COUNT)
		{
			m_arrDefaultEventInfo[m_nDefaultEventCount].MsgID = MsgID;
			m_arrDefaultEventInfo[m_nDefaultEventCount].pHandleSS = pHandle;
			m_arrDefaultEventInfo[m_nDefaultEventCount].pHandleCS = NULL;
			m_nDefaultEventCount++;
		}
	}

	//ע����Ϣӳ��
	void AddDefEvent(uint32_t MsgID, IDefaultMessageCS* pHandle)
	{
		if(NULL != pHandle && m_nDefaultEventCount < MAX_DEFEVENT_COUNT)
		{
			m_arrDefaultEventInfo[m_nDefaultEventCount].MsgID = MsgID;
			m_arrDefaultEventInfo[m_nDefaultEventCount].pHandleSS = NULL;
			m_arrDefaultEventInfo[m_nDefaultEventCount].pHandleCS = pHandle;
			m_nDefaultEventCount++;
		}
	}

	//������Ϣӳ��
	DefaultEventInfo * GetDefaultEvent(uint32_t MsgID)
	{
		for(int32_t i = 0; i < m_nDefaultEventCount; i++)
		{
			if(MsgID == m_arrDefaultEventInfo[i].MsgID)
			{
				return &m_arrDefaultEventInfo[i];
			}
		}

		return NULL;
	}

	//���Ҹ�MsgID�󶨵Ķ��ӳ��
	int32_t GetDefEvent(const uint32_t MsgID, DefaultEventInfo * arrPDefaultEventInfo[MAX_DEFEVENT_COUNT], int32_t& nEventCount)
	{
		int32_t nRet = S_OK;
		if (NULL == arrPDefaultEventInfo)
		{
			return E_UNKNOWN;
		}

		nEventCount = 0;
		for(int32_t i = 0; i < m_nDefaultEventCount; i++)
		{
			if(MsgID == m_arrDefaultEventInfo[i].MsgID)
			{
				arrPDefaultEventInfo[nEventCount] = &m_arrDefaultEventInfo[i];
				nEventCount ++;
			}
		}

		return nRet;
	}

protected:
	//MsgID -> IMessageEvent ӳ��
	int32_t					m_nMsgEventCount;
	MsgEventInfo			m_arrMsgEventInfo[MAX_MSGEVENT_COUNT];

	CriticalSection			m_stSessionBindMsgLock;
	//MsgID -> SessionIndex ӳ��
	int32_t					m_nSessionBingMsgCount;
	BindingMsg				m_arrSessionBingMsg[MAX_MSGEVENT_COUNT];

	//EventID -> SYS_EVENT_PROC ӳ��
	int32_t					m_nSysEventCount;
	SystemEventInfo			m_arrSysEventInfo[MAX_SYSEVENT_COUNT];

	//MsgID -> DefaultEvent ӳ��
	int32_t					m_nDefaultEventCount;
	DefaultEventInfo		m_arrDefaultEventInfo[MAX_DEFEVENT_COUNT];

};


#define	CREATE_FRAMEMSGEVENTMGT_INSTANCE		CSingleton<CFrameMsgEventMgt>::CreateInstance
#define	g_FrameMsgEventMgt						CSingleton<CFrameMsgEventMgt>::GetInstance()
#define	DESTROY_FRAMEMSGEVENTMGT_INSTANCE		CSingleton<CFrameMsgEventMgt>::DestroyInstance

FRAME_NAMESPACE_END


#endif /* FRAME_MSGEVENT_MGT_H_ */
