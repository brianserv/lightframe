/*
 * frame_session_ctl.cpp
 *
 *  Created on: 2011-11-22
 *      Author: jimm
 */

#include "frame_session_mgt.h"
#include "frame_errordef.h"
#include "frame_logengine.h"
#include "lightframe.h"

FRAME_NAMESPACE_BEGIN

//�����Ự(�����Ͻӿ�)
int32_t CFrameSessionMgt::CreateSession(const uint32_t nMsgID, const int32_t nSessionType, ISessionEvent* pHandler, int64_t nTimeOut, CFrameSession*& pSession)
{
	SessionIndex sessionindex = 0;
	int32_t nRet = CreateSession(pSession,sessionindex);
	if( 0 > nRet)
	{
		return nRet;
	}

	//Ϊ���λỰ�󶨶�ʱ��
	TimerIndex timerindex = enmInvalidTimerIndex;

	nRet = g_FrameTimerMgt.CreateTimer(sessionindex, this, NULL, nTimeOut, false, timerindex);
	if(0 > nRet)
	{
		//ɾ���Ự
		DestroySessionByIndex(sessionindex);
		pSession = NULL;
		return nRet;
	}

	uint8_t *pSessionBlock = MALLOC(8 * 1024);//g_FrameMemMgt.AllocBlock(8 * 1024);
	if(pSessionBlock == NULL)
	{
		//ɾ���Ự
		DestroySessionByIndex(sessionindex);
		pSession = NULL;
		//ɾ����ʱ��
		g_FrameTimerMgt.RemoveTimer(timerindex);
		return E_NULLPOINTER;
	}
	pSession->SetSessionMem(pSessionBlock);

	pSession->SetSessionIndex(sessionindex);
	pSession->SetSessionEventHandler(pHandler);
	pSession->SetTimerIndex(timerindex);
	pSession->SetTimeoutValue(nTimeOut);
	pSession->SetSessionType(nSessionType);

	pSession->SetSessionWaitMsg(nMsgID);

	//��ӻỰ����Ϣӳ���ϵ
	g_FrameMsgEventMgt.AddSessionBindMsg(sessionindex, nMsgID);

	return S_OK;
}

//�����Ự
int32_t CFrameSessionMgt::CreateSession(const uint32_t nMsgID, int64_t nTimeOut, ISessionData *pSessionData, CFrameSession*& pSession)
{
	SessionIndex sessionindex = 0;
	int32_t nRet = CreateSession(pSession,sessionindex);
	if( 0 > nRet)
	{
		return nRet;
	}

	//Ϊ���λỰ�󶨶�ʱ��
	TimerIndex timerindex = enmInvalidTimerIndex;

	nRet = g_FrameTimerMgt.CreateTimer(sessionindex, this, NULL, nTimeOut, false, timerindex);
	if(0 > nRet)
	{
		//ɾ���Ự
		DestroySessionByIndex(sessionindex);
		pSession = NULL;
		return nRet;
	}

	uint8_t *pSessionBlock = MALLOC(pSessionData->GetSize());//g_FrameMemMgt.AllocBlock(pSessionData->GetSize());
	if(pSessionBlock == NULL)
	{
		//ɾ���Ự
		DestroySessionByIndex(sessionindex);
		pSession = NULL;
		//ɾ����ʱ��
		g_FrameTimerMgt.RemoveTimer(timerindex);
		return E_NULLPOINTER;
	}
	pSession->SetSessionMem(pSessionBlock);

	//����Ự����
	memcpy(pSessionBlock, pSessionData, pSessionData->GetSize());

	pSession->SetSessionIndex(sessionindex);
	pSession->SetSessionEventHandler(pSessionData->GetSessionEvent());
	pSession->SetSessionCallBackProc(pSessionData->GetSessionProc(), pSessionData->GetTimeoutProc());
	pSession->SetTimerIndex(timerindex);
	pSession->SetTimeoutValue(nTimeOut);
	pSession->SetSessionType(pSessionData->GetSessionType());

	pSession->SetSessionWaitMsg(nMsgID);

	//��ӻỰ����Ϣӳ���ϵ
	g_FrameMsgEventMgt.AddSessionBindMsg(sessionindex, nMsgID);

	return S_OK;
}

//ɾ���Ự
int32_t CFrameSessionMgt::RemoveSession(const SessionIndex sessionindex)
{
	//���һỰ
	CFrameSession *pSession = NULL;
	int32_t nRet = GetSessionByIndex(sessionindex,pSession);
	if (0 > nRet)
	{
		WRITE_MAIN_LOG(enmLogLevel_Error, "RemoveEvent but session can not be found!{ErrorCode=0x%08x SessionIndex=%d}\n", nRet, sessionindex);
		return nRet;
	}

	//����Ự����Ϣӳ���ϵ
	if(pSession->GetSessionWaitMsg() != 0)
	{
		g_FrameMsgEventMgt.RemoveSessionBindMsg(sessionindex,pSession->GetSessionWaitMsg());
		pSession->SetSessionWaitMsg(0);
	}

	//ɾ����ʱ��
	g_FrameTimerMgt.RemoveTimer(pSession->GetTimerIndex());
	//ɾ���Ự
	DestroySessionByIndex(sessionindex);

	return S_OK;
}

//���ٻỰ����
int32_t CFrameSessionMgt::DestroySessionByIndex(const SessionIndex sessionIndex)
{
	m_stSessionLock.enter();
	SessionPool::CIndex* pIndex = m_sessionPool.GetIndex(sessionIndex);
	if (NULL == pIndex)
	{
		m_stSessionLock.leave();
		return E_LIBF_SESSIONNOTEXIST;
	}
	m_stSessionLock.leave();

	CFrameSession *pSession = pIndex->ObjectPtr();
	if(pSession != NULL)
	{
		uint8_t *pSessionMem = pSession->GetSessionData();
		if(pSessionMem != NULL)
		{
			//�����ڴ�
			FREE(pSessionMem);//g_FrameMemMgt.RecycleBlock(pSessionMem);
		}
		//���ûỰ����
		pSession->Reset();
	}

	return DestroySession(pIndex);
}

//��ȡ�Ự����Ϣ�İ���Ϣ
bool CFrameSessionMgt::IsSessionBindingMsg(const SessionIndex sessionindex, const uint32_t MsgID)
{
	return g_FrameMsgEventMgt.IsSessionBindingMsg(sessionindex,MsgID);
}

//�Ự������
int32_t CFrameSessionMgt::OnFrameSessionEvent(SessionIndex sessionindex, MessageHeadSS* msgHead, const uint8_t* buf, const uint32_t size)
{
	//���һỰ
	CFrameSession *pSession = NULL;
	int32_t nRet = GetSessionByIndex(sessionindex,pSession);
	if (0 > nRet)
	{
		WRITE_MAIN_LOG(enmLogLevel_Error, "SessionEvent Active but session can not be found!(ErrorCode=0x%08x SessionIndex=%d)\n", nRet, sessionindex);
		return nRet;
	}

	//����Ự����Ϣӳ���ϵ
	const uint32_t nMsgID = pSession->GetSessionWaitMsg();
	if(0 != nMsgID)
	{
		g_FrameMsgEventMgt.RemoveSessionBindMsg(sessionindex,pSession->GetSessionWaitMsg());
		pSession->SetSessionWaitMsg(0);
	}

	//���ûỰ�ص�ǰ���汾�λỰ��ʶ(DianlogSeq) ���ڼ��Ự�Ƿ�ɾ��
	uint32_t nSessionSeq = pSession->GetSessionSeq();

	//�ص��Ự�ӿ�
	nRet = CallBackSessionEvent(nMsgID, pSession, msgHead, buf, size);
	if(0 > nRet)
	{
		WRITE_MAIN_LOG(enmLogLevel_Error, "CallBackSessionEvent is failed!(ErrorCode=0x%08x SessionIndex=%d,nMsgID=0x%08x)\n", nRet, sessionindex,nMsgID);
	}

	//���»�ȡpSession ��Ϊ�ص������лỰ���ܱ�ɾ����
	nRet = GetSessionByIndex(sessionindex,pSession);
	if(0 > nRet)
	{
		//�Ự�Ѿ���ɾ�� �������κ�����
		return S_OK;
	}

	//�Ựû��ɾ��
	if(pSession->GetSessionSeq() == nSessionSeq)
	{
		//�Ự���°���������Ϣ
		if(0 != pSession->GetSessionWaitMsg())
		{
			return nRet;
		}
		else
		{
			//ɾ����ʱ��
			g_FrameTimerMgt.RemoveTimer(pSession->GetTimerIndex());

			//��û��ɾ���Ự��Ҳû�а���Ϣ
			DestroySessionByIndex(sessionindex);
		}
	}
	else
	{
		//���Ǹ��µĻỰ
	}

	return S_OK;
}

int32_t CFrameSessionMgt::CallBackSessionEvent(const uint32_t nMsgID, CFrameSession *pSession, MessageHeadSS *pMsgHeadSS, const uint8_t* buf, const uint32_t size)
{
	int32_t nRet = S_OK;

	//��ȡIMsgBody
	IMsgBody *pMsgBody = g_MessageMapDecl.GetMessageBody(nMsgID);
	if(NULL == pMsgBody)
	{
		WRITE_MAIN_LOG(enmLogLevel_Warning, "it's not found msg body in callback session event!{msgid=0x%08x, sessionindex=%d}\n", nMsgID, pMsgHeadSS->nSessionIndex);
		return E_LIBF_MSGIDNOMSGBODY;
	}

	uint32_t offset = 0;
	nRet = pMsgBody->MessageDecode(buf, size, offset);
	if( 0 > nRet)
	{
		WRITE_MAIN_LOG(enmLogLevel_Warning, "decode session msg failed!{msgid=0x%08x, sessionindex=%d}", nMsgID, pMsgHeadSS->nSessionIndex);
		return nRet;
	}

	CLightFrame::DumpMessage("LibFrame DumpMessage", pMsgHeadSS, pMsgBody);

	ISessionEvent * pSessionEvent = pSession->GetSessionEventHandler();
	if(pSessionEvent != NULL)
	{
		nRet = pSessionEvent->OnSessionEvent(pMsgHeadSS, pMsgBody, pSession);
	}
	else
	{
		SessionEventCallBack SessionProc = NULL;
		SessionTimeoutCallBack TimeoutProc = NULL;
		pSession->GetSessionCallBackProc(SessionProc, TimeoutProc);
		ISessionEvent *pSessionEvent = pSession->GetSessionEventHandler();
		if(NULL != SessionProc && NULL != pSessionEvent)
		{
			nRet = (pSessionEvent->*SessionProc)(pMsgHeadSS, pMsgBody, pSession, 0, NULL);
		}
	}

	return nRet;
}

int32_t CFrameSessionMgt::OnTimerEvent(CFrameTimer *pFrameTimer)
{
//	if(pFrameTimer->GetTimerType() != enmFrameTimerType_Session)
//	{
//		return E_LIBF_UNEXCEPTTIMERTYPE;
//	}

	//���һỰ
	SessionIndex sessionindex = (SessionIndex)pFrameTimer->GetTimerID();

	CFrameSession *pSession = NULL;
	int32_t nRet = g_FrameSessionMgt.GetSessionByIndex(sessionindex,pSession);
	if (0 > nRet)
	{
		return nRet;
	}

	//����Ự����Ϣӳ���ϵ
	if(pSession->GetSessionWaitMsg())
	{
		g_FrameMsgEventMgt.RemoveSessionBindMsg(sessionindex,pSession->GetSessionWaitMsg());
		pSession->SetSessionWaitMsg(0);
	}

	//�Ự��ʱ�ص�
	uint32_t nSessionSeq = pSession->GetSessionSeq();

	ISessionEvent *pSessionEvent = pSession->GetSessionEventHandler();
	if(NULL != pSessionEvent)
	{
		nRet = pSessionEvent->OnSessionTimeOut(pSession);
	}
	else
	{
		SessionEventCallBack SessionProc = NULL;
		SessionTimeoutCallBack TimeoutProc = NULL;
		pSession->GetSessionCallBackProc(SessionProc, TimeoutProc);
		ISessionEvent *pSessionEvent = pSession->GetSessionEventHandler();
		if(NULL != TimeoutProc && NULL != pSessionEvent)
		{
			nRet = (pSessionEvent->*TimeoutProc)(pSession);
		}
	}

	//�ٴΰ��ݴ��� �������ٴΰ���Ϣ
	nRet = g_FrameSessionMgt.GetSessionByIndex(sessionindex,pSession);
	if (0 > nRet)
	{
		//�Ự�ѱ�ɾ��������
		return S_OK;
	}

	//�Ựû��ɾ��
	if(pSession->GetSessionSeq() == nSessionSeq)
	{
		//����Ự����Ϣӳ���ϵ
		if(pSession->GetSessionWaitMsg())
		{
			g_FrameMsgEventMgt.RemoveSessionBindMsg(sessionindex,pSession->GetSessionWaitMsg());
			pSession->SetSessionWaitMsg(0);
		}

		//ɾ���Ự
		g_FrameSessionMgt.DestroySessionByIndex(sessionindex);
	}
	else
	{
		//���Ǹ��µĻỰ
	}

	return nRet;
}


FRAME_NAMESPACE_END

