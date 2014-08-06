/*
 * lightframe_impl.h
 *
 *  Created on: 2012-10-30
 *      Author: jimm
 */

#ifndef LIGHTFRAME_IMPL_H_
#define LIGHTFRAME_IMPL_H_

#include "frame_typedef.h"
#include "frame_msg_impl.h"
#include "common/common_memory.h"
#include "frame_protocolhead.h"
#include "common/common_mysqlengine.h"
#include "common/common_tcpsocket.h"

FRAME_NAMESPACE_BEGIN

//Server���ýӿ�
class IServerConfig_Impl
{
public:
	virtual ~IServerConfig_Impl() {};

	virtual const char* GetServerName()		= 0;
	//��ȡServer����
	virtual int32_t GetZoneID()				= 0;

	//TODO(ServerType�̻���Frame�������ļ�, ����汾����)
	virtual EntityType GetServerType()			= 0;
	virtual ServerID GetServerID()			= 0;
	virtual LogLevel GetLogLevel()			= 0;
	virtual int32_t GetLogFileSize()		= 0;
};

//�������Ľӿ�
class IConfigCenter
{
public:
	virtual ~IConfigCenter() {};
	virtual int32_t Initialize(const char* szFileName = NULL, const int32_t type=0) = 0;
	virtual int32_t Reload(const char* szFileName = NULL, const int32_t type=0) = 0;
	virtual int32_t Uninitialize() = 0;
};

//�������Ľӿ�
class IDataCenter
{
public:
	virtual ~IDataCenter() {};

	virtual const char* GetName() = 0;

	//��Frame�ṩ�Լ������ڴ��С�Ĳ�ѯ�ӿ�
	virtual size_t GetSize() = 0;

	//��ʼ���ӿڵ��õ�ʱ�� �����ڴ��ѷ���
	virtual int32_t Initialize() = 0;
	virtual int32_t Resume() = 0;
	virtual int32_t Uninitialize() = 0;
};

class CFrameSession;

//�Ự�¼��ӿ�
class ISessionEvent
{
public:
	virtual ~ISessionEvent() {};

	//�Ự�¼�����
	virtual int32_t OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			CFrameSession* pSession, const uint16_t nOptionLen = 0, const void *pOptionData = NULL) = 0;

	//�Ự��ʱ
	virtual int32_t OnSessionTimeOut(CFrameSession *pSession) = 0;
};

typedef int32_t (ISessionEvent::*SessionEventCallBack)(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData);

typedef int32_t (ISessionEvent::*SessionTimeoutCallBack)(CFrameSession *pSession);

class ISessionData
{
public:
	ISessionData()
	{
		m_pSessionProc = NULL;
		m_pTimeoutProc = NULL;
		m_pSessionHandler = NULL;
		m_nRoleID = enmInvalidRoleID;
		m_nRoomID = 0;
	}

	virtual int32_t GetSize() = 0;

	virtual ~ISessionData() {};

	//virtual uint16_t GetSessionSize() = 0;

	RoleID GetRoleID()
	{
		return m_nRoleID;
	}

	uint32_t GetRoomID()
	{
		return m_nRoomID;
	}

	SessionEventCallBack GetSessionProc()
	{
		return m_pSessionProc;
	}

	SessionTimeoutCallBack GetTimeoutProc()
	{
		return m_pTimeoutProc;
	}

	ISessionEvent *GetSessionEvent()
	{
		return m_pSessionHandler;
	}

	SessionType GetSessionType()
	{
		return m_nSessionType;
	}

	virtual void SetSessionData(uint32_t nRoomID, RoleID nRoleID, SessionType nSessionType,
			ISessionEvent *pHandler, SessionEventCallBack SessionProc, SessionTimeoutCallBack TimeoutProc)
	{
		m_nRoomID = nRoomID;
		m_nRoleID = nRoleID;
		m_pSessionProc = (SessionEventCallBack)SessionProc;
		m_pTimeoutProc = (SessionTimeoutCallBack)TimeoutProc;
		m_nSessionType = nSessionType;
		m_pSessionHandler = pHandler;
	}

	virtual void SetSessionData(uint32_t nRoomID, RoleID nRoleID, SessionType nSessionType,
			ISessionEvent *pHandler)
	{
		m_nRoomID = nRoomID;
		m_nRoleID = nRoleID;
		m_pSessionHandler = pHandler;
		m_nSessionType = nSessionType;
	}
protected:
	SessionEventCallBack	m_pSessionProc;
	SessionTimeoutCallBack	m_pTimeoutProc;
	ISessionEvent			*m_pSessionHandler;
	SessionType				m_nSessionType;
	RoleID					m_nRoleID;
	uint32_t				m_nRoomID;
};

//���м̳�ISessionData�����඼Ҫ�������м�������ĺ�
#define SESSION_INTERFACE() 	\
	public:	\
	int32_t GetSize(){return sizeof(*this);}

#define SESSION_DATA_BEGIN(class_name)		\
	class class_name : public ISessionData	\
	{	\
		SESSION_INTERFACE()

#define SESSION_DATA_MEMBER(type, member)	\
	type		member;

#define SESSION_DATA_END()	};

class CFrameTimer;

//��ʱ���¼��ӿ�
class ITimerEvent
{
public:
	virtual ~ITimerEvent() {};
	//��ʱ���¼�
	virtual int32_t OnTimerEvent(CFrameTimer *pTimer) = 0;
};

typedef int32_t (ITimerEvent::*TimerProc)(CFrameTimer *pTimer);

//Message�¼��ӿ�
class IMessageEventSS
{
public:
	virtual ~IMessageEventSS() {};

	//��Ϣ�¼�
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			const uint16_t nOptionLen = 0, const void *pOptionData = NULL) = 0;
};

typedef int32_t (IMessageEventSS::*SS_MSG_PROC)(MessageHeadSS *pMsgHead, IMsgBody *pMsgBody,
			const uint16_t nOptionLen, const void *pOptionData);

//Message�¼��ӿ�
class IMessageEventCS
{
public:
	virtual ~IMessageEventCS() {};

	//��Ϣ�¼�
	virtual int32_t OnMessageEvent(MessageHeadCS * pMsgHead, IMsgBody* pMsgBody,
			const uint16_t nOptionLen = 0, const void *pOptionData = NULL) = 0;

};

typedef int32_t (IMessageEventCS::*CS_MSG_PROC)(MessageHeadCS *pMsgHead, IMsgBody *pMsgBody,
			const uint16_t nOptionLen, const void *pOptionData);


//���Է���������Ϣ�ӿ�
class IDefaultMessageSS
{
public:
	virtual ~IDefaultMessageSS() {};

	//��Ϣ�¼�
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, const uint8_t* pBuf,const uint32_t nBufLen,
			const uint16_t nOptionLen = 0, const void *pOptionData = NULL) = 0;
};

//���Կͻ��˵���Ϣ�ӿ�
class IDefaultMessageCS
{
public:
	virtual ~IDefaultMessageCS() {};

	//��Ϣ�¼�
	virtual int32_t OnMessageEvent(MessageHeadCS * pMsgHead, const uint8_t* pBuf,const uint32_t nBufLen,
			const uint16_t nOptionLen = 0, const void *pOptionData = NULL) = 0;
};

class IIOHandler
{
public:
	virtual ~IIOHandler(){};

	virtual int32_t ReadEvent() = 0;

	virtual int32_t WriteEvent() = 0;

	virtual int32_t ErrorEvent() = 0;
};

class ISystemEvent
{
public:
	virtual ~ISystemEvent() {};

	virtual int32_t OnSystemEvent(uint16_t nEventID, void *pParam) = 0;
};

typedef int32_t (*SYS_EVENT_PROC)(uint16_t nEventID, void *pParam);


class ITimerData
{
public:
	virtual ~ITimerData(){};

	virtual int32_t GetSize() = 0;
};
//���м̳�ITimerData�����඼Ҫ�������м�������ĺ�
#define TIMER_INTERFACE() 	\
	public:	\
	int32_t GetSize(){return sizeof(*this);}

#define TIMER_DATA_BEGIN(class_name)	\
	class class_name : public ITimerData	\
	{	\
		TIMER_INTERFACE()

#define TIMER_DATA_END()	};


class CSocket;

//Frame�߼����ܽӿ�
class IFrame_Impl
{
public:
	virtual ~IFrame_Impl() {};

	virtual void RegistMsgHandler(uint32_t nMsgID, IMessageEventSS* pHandle, SS_MSG_PROC proc = NULL) = 0;

	virtual void RegistMsgHandler(uint32_t nMsgID, IMessageEventCS* pHandle, CS_MSG_PROC proc = NULL) = 0;

	//ע��ϵͳ�¼�
	virtual void RegistSysEvent(uint16_t nEventID, ISystemEvent* pHandle) = 0;

	virtual int32_t RegistSocketHandler(CSocket *pSocket, uint16_t nListenPort) = 0;

	virtual int32_t RegistSocketHandler(CSocket *pSocket, EntityType nServerType) = 0;

	//������Ϣ
	virtual int32_t PostMessage(MessageHeadSS* pMessageHead, IMsgBody *pMsgBody,
			const int32_t nOptionLen = 0, const char *pOptionData = NULL) = 0;

	//������Ϣ
	virtual int32_t PostMessage(MessageHeadCS* pMessageHead, IMsgBody *pMsgBody,SocketFD nSocketFD) = 0;

	//������Ϣ
	virtual int32_t PushCommand(MessageHeadSS* pMessageHead, IMsgBody *pMsgBody, const int32_t nOptionLen = 0,
			const char *pOptionData = NULL) = 0;

	//д��־(DEBUGģʽֱ��д�ļ�)
	virtual int32_t WriteLog(LogLevel loglevel, const char *szFunc, const int32_t lineno,
			const char *szFormat, ...) = 0;

	//�����Ự
	virtual int32_t CreateSession(const uint32_t nMsgID, int64_t nTimeOut, ISessionData *pSessionData,
			CFrameSession*& pSession) = 0;

	//ɾ���Ự
	virtual int32_t RemoveSession(const SessionIndex sessionindex) = 0;

	//���ö�ʱ��
	virtual int32_t CreateTimer(int32_t nTimerID, ITimerEvent* pHandler, ITimerData *pTimerData,
			int64_t nCycleTime, bool bLoop, TimerIndex& timerIndex) = 0;

	//ɾ����ʱ��
	virtual int32_t RemoveTimer(const TimerIndex timerIndex) = 0;

	virtual void SetDaemon() = 0;
};

FRAME_NAMESPACE_END

#endif /* LIGHTFRAME_IMPL_H_ */
