/*
 * frame_mainthread.cpp
 *
 *  Created on: 2011-11-22
 *      Author: jimm
 */


#include "common/common_api.h"
#include "common/common_datetime.h"
#include "common/common_message.h"
#include "frame_errordef.h"

#include "frame_mainthread.h"
#include "frame_eventid.h"

#include "frame_netqueue.h"
#include "frame_commandqueue.h"
#include "frame_logengine.h"
#include "frame_timer_mgt.h"
#include "frame_session_mgt.h"
#include "frame_msgevent_mgt.h"
#include "frame_connection_mgt.h"
#include "frame_thread_mgt.h"
#include "frame_eventid.h"
#include "frame_crypt.h"

#include "lightframe.h"


FRAME_NAMESPACE_BEGIN


CFrameMainThread::CFrameMainThread()
{
}

CFrameMainThread::~CFrameMainThread()
{
}

//��ʼ����ҵ���߳�
int32_t CFrameMainThread::Initialize()
{
	return S_OK;
}

//�ָ���ҵ���߳�
int32_t CFrameMainThread::Resume()
{
	return Initialize();
}

//ע����ҵ���߳�
int32_t CFrameMainThread::Uninitialize()
{
	return S_OK;
}


//�߳���ں���
void CFrameMainThread::Execute()
{
	bool bHasData = false;

	int32_t nThreadID = (int32_t)gettid();
	g_FrameThreadMgt.RegistThread(nThreadID, this);

	Delay(enmThreadExecutePeriod * 10);

	WRITE_MAIN_LOG(enmLogLevel_Debug, "MainThread Start!\n");

	bool bIdle = true;

	while ((!GetTerminated()) || (!bIdle))
	{
		bIdle = true;

		//����ʱ���¼�
		bHasData = ProcessTimerEvent();
		if (bHasData)
		{
			bIdle = false;
		}

		//����SSͨ������
		if (bIdle)
		{
			bHasData = ProcessSSLogicData();
			if (bHasData)
			{
				bIdle = false;
			}
		}

		//����CSͨ������
		if (bIdle)
		{
			bHasData = ProcessCSLogicData();
			if (bHasData)
			{
				bIdle = false;
			}
		}

		if (bIdle)
		{
			bHasData = ProcessTestMessage();
			if (bHasData)
			{
				bIdle = false;
			}
		}

		//��û��������Ҫ����
		if (bIdle)
		{
			//д��ͳ������
			//GET_NETTHREADSTAT_INSTANCE().WriteCount();
			Delay(enmThreadExecutePeriod);
		}
	}

	WRITE_MAIN_LOG(enmLogLevel_Debug, "MainThread Stop!\n");
}

bool CFrameMainThread::ProcessTestMessage()
{
	NetPacket *pNetPacket = NULL;
	int32_t ret = g_FrameCommandQueue.Pop(pNetPacket);
	if ((0 > ret) || (pNetPacket == NULL))
	{
		return false;
	}

	return ProcessSSMessage(&pNetPacket->m_pNetPacket[0], pNetPacket->m_nNetPacketLen);
}

//����SSͨ������
bool CFrameMainThread::ProcessSSLogicData()
{
	//�ӽ��ն�����ȡ����Ϣ
	NetPacket *pNetPacket = NULL;
	int32_t ret = g_FrameNetQueue.PopRecvSSQueue(pNetPacket);
	if ((0 > ret) || (pNetPacket == NULL))
	{
		return false;
	}

	bool bHasData = ProcessSSMessage(&pNetPacket->m_pNetPacket[0], pNetPacket->m_nNetPacketLen);

	FREE((uint8_t *)pNetPacket);//g_FrameMemMgt.RecycleBlock((uint8_t *)pNetPacket);

	return bHasData;
}

//����CSͨ������
bool CFrameMainThread::ProcessCSLogicData()
{
	NetPacket *pNetPacket = NULL;
	//�ӽ��ն�����ȡ����Ϣ
	int32_t ret = g_FrameNetQueue.PopRecvCSQueue(pNetPacket);
	if ((0 > ret) || (pNetPacket == NULL))
	{
		return false;
	}

	bool bHasData = ProcessCSMessage(&pNetPacket->m_pNetPacket[0], pNetPacket->m_nNetPacketLen);

	FREE((uint8_t *)pNetPacket);//g_FrameMemMgt.RecycleBlock((uint8_t *)pNetPacket);

	return bHasData;
}

//����ʱ���¼�
bool CFrameMainThread::ProcessTimerEvent()
{
	CFrameTimer *pTimer = NULL;
	TimerIndex timerIndex = enmInvalidTimerIndex;

	//��ȡ��ʱ���б��еĽ���ʱ������Ķ�ʱ��
	int32_t ret = g_FrameTimerMgt.GetFirstTimer(pTimer, timerIndex);
	if (0 > ret)
	{
		return false;
	}

	//��ʱ������ʱ��С�ڵ�ǰʱ��
	if (pTimer->GetEndTime() > CTimeValue::CurrentTime().Microseconds())
	{
		return false;
	}

	//����TimerSeq ���ڱ���
	uint32_t nTimerSeq = pTimer->GetTimerSeq();

	//�ص���ʱ���ӿ�
//	if(enmFrameTimerType_Session == pTimer->GetTimerType())
//	{
//		//����ǻỰ��Timer
//		FrameTimerProc pProc = pTimer->GetFrameEventProc();
//		if(NULL != pProc)
//		{
//			pProc(pTimer);
//		}
//	}
//	else
	{
		//�ⲿTimerֱ�ӻص��ӿ�
		ITimerEvent *pHandle = pTimer->GetEventHandler();
		if( NULL != pHandle)
		{
			TimerProc proc = pTimer->GetEventProc();
			if(proc != NULL)
			{
				(pHandle->*proc)(pTimer);
			}
			else
			{
				pHandle->OnTimerEvent(pTimer);
			}
		}
	}

	//��ʱ���¼��Ѵ���
	g_FrameTimerMgt.TimerFired(timerIndex,nTimerSeq);

	return true;
}

bool CFrameMainThread::ProcessSSMessage(uint8_t *pBuf, uint32_t nLength)
{
	uint32_t nOffset = 0;

	bool bHasData = false;

	MessageHeadSS stMessageHead;
	int32_t ret = stMessageHead.MessageDecode(pBuf, nLength, nOffset);
	if (0 > ret)
	{
		WRITE_MAIN_LOG(enmLogLevel_Error, "Pop center net message, but decode msghead error!\n");
		return bHasData;
	}

	if (nLength < nOffset)
	{
		WRITE_MAIN_LOG(enmLogLevel_Error, "Pop center net  message, but decode msgbody error!\n");
		return bHasData;
	}

	//ע���
	if(stMessageHead.nTransType == enmTransType_Regist)
	{
		OnSystemEvent(SYS_EVENT_REGIST_SERVER, &stMessageHead);
	}
	else if(stMessageHead.nTransType == enmTransType_Disconnect)
	{
		OnSystemEvent(SYS_EVENT_DISCONNECT_SERVER, &stMessageHead);
	}
	else
	{
		//��Ϣ����
		OnMessageEvent(&stMessageHead, nOffset, pBuf + nOffset, nLength - nOffset);
	}

	return true;
}

bool CFrameMainThread::ProcessCSMessage(uint8_t *pBuf, uint32_t nLength)
{
	uint32_t nOffset = 0;

	bool bHasData = false;

	ConnInfo *pConnInfo = (ConnInfo *)pBuf;
	nOffset += sizeof(ConnInfo);

	//���ӶϿ�
	if(pConnInfo->nErrorCode > 0)
	{
		OnSystemEvent(pConnInfo->nErrorCode, pConnInfo);
		//������Դ
		g_FrameConnectionMgt.DestroyConnection(pConnInfo->nTunnelIndex);
	}
	else
	{
		MessageHeadCS stMessageHead;
		int32_t ret = stMessageHead.MessageDecode(pBuf, nLength, nOffset);
		if (0 > ret)
		{
			WRITE_MAIN_LOG(enmLogLevel_Error, "Pop center net message, but decode msghead error!\n");
			return bHasData;
		}

		if (nLength < nOffset)
		{
			WRITE_MAIN_LOG(enmLogLevel_Error, "Pop center net  message, but decode msgbody error!\n");
			return bHasData;
		}

		//��ȡ������Ϣ
		CFrameConnection *pConnection = g_FrameConnectionMgt.GetConnection(pConnInfo->nTunnelIndex);
		if(pConnection == NULL)
		{
			pConnection = g_FrameConnectionMgt.CreateConnection(pConnInfo->nTunnelIndex, pConnInfo);
			if(pConnection == NULL)
			{
				WRITE_MAIN_LOG(enmLogLevel_Error, "create conntion info failed!{nRoleID=%d,nFd=%d}\n", stMessageHead.nRoleID,pConnInfo->nTunnelIndex);
				return bHasData;
			}
		}

		uint32_t nBodySize = nLength - nOffset;
		//����web�˵���Ϣ����Ҫ����
		if(nBodySize > 0 && pConnInfo->nServerID != enmEntityType_Webagent)
		{
			ret = CFrameCrypt::Decrypt(&stMessageHead, pBuf + nOffset, nBodySize, pConnection);
		}
		if(ret >= 0)
		{
			//��Ϣ����
			OnMessageEvent(&stMessageHead, nOffset, pBuf + nOffset, nBodySize, sizeof(ConnInfo), &pConnection->m_stConnInfo);
		}
	}

	return bHasData;
}

//ϵͳ�¼�
int32_t CFrameMainThread::OnSystemEvent(uint16_t nEventID, void *pParam)
{
	SystemEventInfo *pEventInfo = g_FrameMsgEventMgt.GetSystemEventProc(nEventID);
	if(pEventInfo == NULL)
	{
		return S_OK;
	}

	if(pEventInfo->pHandle != NULL)
	{
		return pEventInfo->pHandle->OnSystemEvent(nEventID, pParam);
	}

	WRITE_MAIN_LOG(enmLogLevel_Warning, "null pointer:sysevent hander is null!{eventid=0x%08x}\n", nEventID);
	return S_OK;
}

int32_t CFrameMainThread::OnMessageEvent(MessageHeadCS* pMsgHead, const uint16_t nMsgHeadOffset,
		const uint8_t* buf, const uint32_t size, const int32_t nOptionLen, const void *pOption)
{
	uint32_t MsgID = pMsgHead->nMessageID;

	int32_t nRet = E_UNKNOWN;
	uint32_t nBodySize = size;

	//����Ƿ���defaultevent
	int32_t nDefEventCount = 0;
	DefaultEventInfo *arrPDefaultEventInfo[MAX_DEFEVENT_COUNT] = {NULL};

	nRet = g_FrameMsgEventMgt.GetDefEvent(MsgID, arrPDefaultEventInfo, nDefEventCount);
	if (nRet < 0)
	{
		WRITE_MAIN_LOG(enmLogLevel_Error, "it's not found default event hander error!{msgid=0x%08x}\n", MsgID);
		return nRet;
	}

	bool bIsDefEvent = false;
	for(int32_t i = 0; i < nDefEventCount; i++)
	{
		if(arrPDefaultEventInfo[i]->pHandleCS != NULL)
		{
			if(!bIsDefEvent)
			{
				bIsDefEvent = true;
			}
			arrPDefaultEventInfo[i]->pHandleCS->OnMessageEvent(pMsgHead,buf,nBodySize,nOptionLen,pOption);
		}
		else
		{
			WRITE_MAIN_LOG(enmLogLevel_Warning, "null pointer:default event hander is null!{msgid=0x%08x}\n", MsgID);
			continue;
		}
	}
	if(bIsDefEvent)
	{
		return S_OK;
	}

	MsgEventInfo * arrPMsgEventInfo[MAX_MSGEVENT_COUNT] = {NULL};
	int32_t nEventCount = 0;
	nRet = g_FrameMsgEventMgt.GetMessageEvent(MsgID, arrPMsgEventInfo, nEventCount);
	if ((nRet < 0) || (nEventCount <= 0))
	{
		WRITE_MAIN_LOG(enmLogLevel_Warning, "it's not found msg hander!{msgid=0x%08x}\n", MsgID);
		return nRet;
	}

	for (int32_t i = 0; i < nEventCount; i++)
	{
		//��ȡIMsgBody
		IMsgBody *pMsgBody = g_MessageMapDecl.GetMessageBody(MsgID);
		if(NULL == pMsgBody)
		{
			WRITE_MAIN_LOG(enmLogLevel_Warning, "it's not found msg body!{msgid=0x%08x}\n", MsgID);
			continue;
		}

		uint32_t offset = 0;
		nRet = pMsgBody->MessageDecode(buf, nBodySize, offset);
		if( 0 > nRet)
		{
			WRITE_MAIN_LOG(enmLogLevel_Error, "decode msg body failed!{ret=0x%08x, msgid=0x%08x}\n", nRet, MsgID);
			continue;
		}

		CLightFrame::DumpMessage("LibFrame DumpMessage", pMsgHead, pMsgBody);

		if(arrPMsgEventInfo[i]->pHandleCS != NULL)
		{
			if(arrPMsgEventInfo[i]->CSMsgProc != NULL)
			{
				CS_MSG_PROC proc = arrPMsgEventInfo[i]->CSMsgProc;
				nRet = (arrPMsgEventInfo[i]->pHandleCS->*proc)(pMsgHead, pMsgBody, nOptionLen, pOption);
			}
			else
			{
				nRet = arrPMsgEventInfo[i]->pHandleCS->OnMessageEvent(pMsgHead, pMsgBody, nOptionLen, pOption);
			}
		}
		else
		{
			WRITE_MAIN_LOG(enmLogLevel_Warning, "null pointer:msg hander is null!{msgid=0x%08x}\n", MsgID);
			continue;
		}
	}

	return nRet;
}

int32_t CFrameMainThread::OnMessageEvent(MessageHeadSS* pMsgHead, const uint16_t nMsgHeadOffset, const uint8_t* buf, const uint32_t size)
{
	uint32_t MsgID = pMsgHead->nMessageID;

	int32_t nRet = E_UNKNOWN;

	uint32_t nBodySize = pMsgHead->nTotalSize - pMsgHead->nHeadSize;
	uint16_t nOptionLen = pMsgHead->nHeadSize - nMsgHeadOffset;
	char *pOptiondata = (nOptionLen == 0 ? NULL : (char *)buf);

	//����Ƿ���defaultevent
	int32_t nDefEventCount = 0;
	DefaultEventInfo *arrPDefaultEventInfo[MAX_DEFEVENT_COUNT] = {NULL};

	nRet = g_FrameMsgEventMgt.GetDefEvent(MsgID, arrPDefaultEventInfo, nDefEventCount);
	if (nRet < 0)
	{
		WRITE_MAIN_LOG(enmLogLevel_Error, "it's not found default event hander error!{msgid=0x%08x}\n", MsgID);
		return nRet;
	}

	for(int32_t i = 0; i < nDefEventCount; i++)
	{
		if(arrPDefaultEventInfo[i]->pHandleSS != NULL)
		{
			arrPDefaultEventInfo[i]->pHandleSS->OnMessageEvent(pMsgHead,&buf[nOptionLen],nBodySize,nOptionLen,pOptiondata);
		}
		else
		{
			WRITE_MAIN_LOG(enmLogLevel_Warning, "null pointer:default event hander is null!{msgid=0x%08x}\n", MsgID);
			continue;
		}
	}


	//����Ƿ��лỰ�ڵȴ������Ϣ
	if(pMsgHead->nSessionIndex != enmInvalidSessionIndex && pMsgHead->nMessageType == enmMessageType_Response)
	{
		if(g_FrameSessionMgt.IsSessionBindingMsg(pMsgHead->nSessionIndex, MsgID))
		{
			//ִ�лỰ�¼��ص�
			nRet = g_FrameSessionMgt.OnFrameSessionEvent(pMsgHead->nSessionIndex, pMsgHead, &buf[nOptionLen], nBodySize);
		}
	}
	else
	{
		//�Ƿ�Ϊ�����Ϣע�������Event�ӿ�
		MsgEventInfo * arrPMsgEventInfo[MAX_MSGEVENT_COUNT] = {NULL};
		int32_t nEventCount = 0;
		nRet = g_FrameMsgEventMgt.GetMessageEvent(MsgID, arrPMsgEventInfo, nEventCount);
		if ((nRet < 0) || (nEventCount <= 0))
		{
			WRITE_MAIN_LOG(enmLogLevel_Warning, "it's not found msg hander!{msgid=0x%08x}\n", MsgID);
			return nRet;
		}

		for (int32_t i = 0; i < nEventCount; i++)
		{
			if(MsgID == CLOSE_NOTIFY_EVENT) //����֪ͨ
			{
				nRet = arrPMsgEventInfo[i]->pHandleSS->OnMessageEvent(pMsgHead, NULL, nOptionLen, pOptiondata);
			}
			else
			{
				//����msgbody����
				IMsgBody *pMsgBody = CreateMsgBody(MsgID);
				if(pMsgBody == NULL)
				{
					continue;
				}

				uint32_t offset = 0;
				nRet = pMsgBody->MessageDecode(&buf[nOptionLen], nBodySize, offset);
				if( 0 > nRet)
				{
					WRITE_MAIN_LOG(enmLogLevel_Error, "decode msg body failed!{ret=0x%08x, MsgID=0x%08x}\n", nRet, MsgID);
					//����msgbody
					DestroyMsgBody(pMsgBody);
					continue;
				}

				CLightFrame::DumpMessage("LibFrame DumpMessage", pMsgHead, pMsgBody);

				if(arrPMsgEventInfo[i]->pHandleSS != NULL)
				{
					if(arrPMsgEventInfo[i]->SSMsgProc != NULL)
					{
						SS_MSG_PROC proc = arrPMsgEventInfo[i]->SSMsgProc;
						nRet = (arrPMsgEventInfo[i]->pHandleSS->*proc)(pMsgHead, pMsgBody, nOptionLen, pOptiondata);
					}
					else
					{
						nRet = arrPMsgEventInfo[i]->pHandleSS->OnMessageEvent(pMsgHead, pMsgBody, nOptionLen, pOptiondata);
					}
				}
				else
				{
					WRITE_MAIN_LOG(enmLogLevel_Warning, "null pointer:msg hander is null!{MsgID=0x%08x}\n", MsgID);
					//continue;
				}

				//����msgbody
				DestroyMsgBody(pMsgBody);
			}
		}
	}

	return nRet;
}

IMsgBody *CFrameMainThread::CreateMsgBody(uint32_t nMsgID)
{
	//��ȡIMsgBody
	IMsgBody *pMsgBody = g_MessageMapDecl.GetMessageBody(nMsgID);
	if(NULL == pMsgBody)
	{
		WRITE_MAIN_LOG(enmLogLevel_Warning, "it's not found msg body!{nMsgID=0x%08x}\n", nMsgID);
		return NULL;
	}

	IMsgBody *pBodyInstance = pMsgBody;

	//ҵ���߳��Ƿ��Ƕ��߳�
	if(g_FrameConfigMgt.GetFrameBaseConfig().GetAppThreadCount() > 1)
	{
		pBodyInstance = (IMsgBody *)MALLOC(pMsgBody->GetSize());
		if(pBodyInstance == NULL)
		{
			WRITE_MAIN_LOG(enmLogLevel_Error, "malloc msgbody failed!{nMsgID=0x%08x, Size=%d}\n",
					nMsgID, pMsgBody->GetSize());

			return NULL;
		}

//		MUTEX_GUARD(MsgBodyLock, m_stMsgBodyLock);
		//�������ݵ��µ�msgbody����
		memcpy(pBodyInstance, pMsgBody, pMsgBody->GetSize());
	}

	return pBodyInstance;
}

void CFrameMainThread::DestroyMsgBody(IMsgBody *pMsgBody)
{
	//ҵ���߳��Ƿ��Ƕ��߳�
	if(g_FrameConfigMgt.GetFrameBaseConfig().GetAppThreadCount() > 1)
	{
//		MUTEX_GUARD(MsgBodyLock, m_stMsgBodyLock);

		FREE((uint8_t *)pMsgBody);
	}
}

FRAME_NAMESPACE_END

