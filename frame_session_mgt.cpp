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

//创建会话(兼容老接口)
int32_t CFrameSessionMgt::CreateSession(const uint32_t nMsgID, const int32_t nSessionType, ISessionEvent* pHandler, int64_t nTimeOut, CFrameSession*& pSession)
{
	SessionIndex sessionindex = 0;
	int32_t nRet = CreateSession(pSession,sessionindex);
	if( 0 > nRet)
	{
		return nRet;
	}

	//为本次会话绑定定时器
	TimerIndex timerindex = enmInvalidTimerIndex;

	nRet = g_FrameTimerMgt.CreateTimer(sessionindex, this, NULL, nTimeOut, false, timerindex);
	if(0 > nRet)
	{
		//删除会话
		DestroySessionByIndex(sessionindex);
		pSession = NULL;
		return nRet;
	}

	uint8_t *pSessionBlock = MALLOC(8 * 1024);//g_FrameMemMgt.AllocBlock(8 * 1024);
	if(pSessionBlock == NULL)
	{
		//删除会话
		DestroySessionByIndex(sessionindex);
		pSession = NULL;
		//删除定时器
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

	//添加会话绑定消息映射关系
	g_FrameMsgEventMgt.AddSessionBindMsg(sessionindex, nMsgID);

	return S_OK;
}

//创建会话
int32_t CFrameSessionMgt::CreateSession(const uint32_t nMsgID, int64_t nTimeOut, ISessionData *pSessionData, CFrameSession*& pSession)
{
	SessionIndex sessionindex = 0;
	int32_t nRet = CreateSession(pSession,sessionindex);
	if( 0 > nRet)
	{
		return nRet;
	}

	//为本次会话绑定定时器
	TimerIndex timerindex = enmInvalidTimerIndex;

	nRet = g_FrameTimerMgt.CreateTimer(sessionindex, this, NULL, nTimeOut, false, timerindex);
	if(0 > nRet)
	{
		//删除会话
		DestroySessionByIndex(sessionindex);
		pSession = NULL;
		return nRet;
	}

	uint8_t *pSessionBlock = MALLOC(pSessionData->GetSize());//g_FrameMemMgt.AllocBlock(pSessionData->GetSize());
	if(pSessionBlock == NULL)
	{
		//删除会话
		DestroySessionByIndex(sessionindex);
		pSession = NULL;
		//删除定时器
		g_FrameTimerMgt.RemoveTimer(timerindex);
		return E_NULLPOINTER;
	}
	pSession->SetSessionMem(pSessionBlock);

	//保存会话数据
	memcpy(pSessionBlock, pSessionData, pSessionData->GetSize());

	pSession->SetSessionIndex(sessionindex);
	pSession->SetSessionEventHandler(pSessionData->GetSessionEvent());
	pSession->SetSessionCallBackProc(pSessionData->GetSessionProc(), pSessionData->GetTimeoutProc());
	pSession->SetTimerIndex(timerindex);
	pSession->SetTimeoutValue(nTimeOut);
	pSession->SetSessionType(pSessionData->GetSessionType());

	pSession->SetSessionWaitMsg(nMsgID);

	//添加会话绑定消息映射关系
	g_FrameMsgEventMgt.AddSessionBindMsg(sessionindex, nMsgID);

	return S_OK;
}

//删除会话
int32_t CFrameSessionMgt::RemoveSession(const SessionIndex sessionindex)
{
	//查找会话
	CFrameSession *pSession = NULL;
	int32_t nRet = GetSessionByIndex(sessionindex,pSession);
	if (0 > nRet)
	{
		WRITE_MAIN_LOG(enmLogLevel_Error, "RemoveEvent but session can not be found!{ErrorCode=0x%08x SessionIndex=%d}\n", nRet, sessionindex);
		return nRet;
	}

	//解除会话绑定消息映射关系
	if(pSession->GetSessionWaitMsg() != 0)
	{
		g_FrameMsgEventMgt.RemoveSessionBindMsg(sessionindex,pSession->GetSessionWaitMsg());
		pSession->SetSessionWaitMsg(0);
	}

	//删除定时器
	g_FrameTimerMgt.RemoveTimer(pSession->GetTimerIndex());
	//删除会话
	DestroySessionByIndex(sessionindex);

	return S_OK;
}

//销毁会话对象
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
			//回收内存
			FREE(pSessionMem);//g_FrameMemMgt.RecycleBlock(pSessionMem);
		}
		//重置会话数据
		pSession->Reset();
	}

	return DestroySession(pIndex);
}

//获取会话与消息的绑定信息
bool CFrameSessionMgt::IsSessionBindingMsg(const SessionIndex sessionindex, const uint32_t MsgID)
{
	return g_FrameMsgEventMgt.IsSessionBindingMsg(sessionindex,MsgID);
}

//会话被激活
int32_t CFrameSessionMgt::OnFrameSessionEvent(SessionIndex sessionindex, MessageHeadSS* msgHead, const uint8_t* buf, const uint32_t size)
{
	//查找会话
	CFrameSession *pSession = NULL;
	int32_t nRet = GetSessionByIndex(sessionindex,pSession);
	if (0 > nRet)
	{
		WRITE_MAIN_LOG(enmLogLevel_Error, "SessionEvent Active but session can not be found!(ErrorCode=0x%08x SessionIndex=%d)\n", nRet, sessionindex);
		return nRet;
	}

	//解除会话绑定消息映射关系
	const uint32_t nMsgID = pSession->GetSessionWaitMsg();
	if(0 != nMsgID)
	{
		g_FrameMsgEventMgt.RemoveSessionBindMsg(sessionindex,pSession->GetSessionWaitMsg());
		pSession->SetSessionWaitMsg(0);
	}

	//调用会话回调前保存本次会话标识(DianlogSeq) 用于检测会话是否被删除
	uint32_t nSessionSeq = pSession->GetSessionSeq();

	//回调会话接口
	nRet = CallBackSessionEvent(nMsgID, pSession, msgHead, buf, size);
	if(0 > nRet)
	{
		WRITE_MAIN_LOG(enmLogLevel_Error, "CallBackSessionEvent is failed!(ErrorCode=0x%08x SessionIndex=%d,nMsgID=0x%08x)\n", nRet, sessionindex,nMsgID);
	}

	//重新获取pSession 因为回调过程中会话可能被删除了
	nRet = GetSessionByIndex(sessionindex,pSession);
	if(0 > nRet)
	{
		//会话已经被删除 不用做任何事情
		return S_OK;
	}

	//会话没有删除
	if(pSession->GetSessionSeq() == nSessionSeq)
	{
		//会话重新绑定了其他消息
		if(0 != pSession->GetSessionWaitMsg())
		{
			return nRet;
		}
		else
		{
			//删除定时器
			g_FrameTimerMgt.RemoveTimer(pSession->GetTimerIndex());

			//即没有删除会话，也没有绑定消息
			DestroySessionByIndex(sessionindex);
		}
	}
	else
	{
		//这是个新的会话
	}

	return S_OK;
}

int32_t CFrameSessionMgt::CallBackSessionEvent(const uint32_t nMsgID, CFrameSession *pSession, MessageHeadSS *pMsgHeadSS, const uint8_t* buf, const uint32_t size)
{
	int32_t nRet = S_OK;

	//获取IMsgBody
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

	//查找会话
	SessionIndex sessionindex = (SessionIndex)pFrameTimer->GetTimerID();

	CFrameSession *pSession = NULL;
	int32_t nRet = g_FrameSessionMgt.GetSessionByIndex(sessionindex,pSession);
	if (0 > nRet)
	{
		return nRet;
	}

	//解除会话绑定消息映射关系
	if(pSession->GetSessionWaitMsg())
	{
		g_FrameMsgEventMgt.RemoveSessionBindMsg(sessionindex,pSession->GetSessionWaitMsg());
		pSession->SetSessionWaitMsg(0);
	}

	//会话超时回调
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

	//再次绑定容错处理 不允许再次绑定消息
	nRet = g_FrameSessionMgt.GetSessionByIndex(sessionindex,pSession);
	if (0 > nRet)
	{
		//会话已被删除，正常
		return S_OK;
	}

	//会话没有删除
	if(pSession->GetSessionSeq() == nSessionSeq)
	{
		//解除会话绑定消息映射关系
		if(pSession->GetSessionWaitMsg())
		{
			g_FrameMsgEventMgt.RemoveSessionBindMsg(sessionindex,pSession->GetSessionWaitMsg());
			pSession->SetSessionWaitMsg(0);
		}

		//删除会话
		g_FrameSessionMgt.DestroySessionByIndex(sessionindex);
	}
	else
	{
		//这是个新的会话
	}

	return nRet;
}


FRAME_NAMESPACE_END

