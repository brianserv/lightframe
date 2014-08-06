/*
 * lightframe.h
 *
 *  Created on: 2012-10-30
 *      Author: jimm
 */

#ifndef LIGHTFRAME_H_
#define LIGHTFRAME_H_

#include <signal.h>

#include "lightframe_impl.h"
#include "frame_session_mgt.h"
#include "frame_timer_mgt.h"
#include "frame_msgevent_mgt.h"
#include "frame_netqueue.h"
#include "frame_commandqueue.h"
#include "frame_typedef.h"
#include "frame_protocolhead.h"
#include "frame_baseconfig.h"
#include "frame_configmgt.h"
#include "frame_socket_mgt.h"
#include "frame_connection_mgt.h"
#include "frame_mem_mgt.h"
#include "frame_logengine.h"
#include "frame_namespace.h"
#include "frame_def.h"
#include "frame_dalmgt.h"
#include "frame_timertask.h"
#include "frame_mainthread.h"
#include "frame_thread_mgt.h"


FRAME_NAMESPACE_BEGIN

typedef uint8_t						ServerCommand;		//服务器命令
enum
{
	enmServerCommand_None			= 0x00,				//无效命令
	enmServerCommand_Reload			= 0x01,				//重新加载命令
	enmServerCommand_Terminate		= 0x02,				//退出命令
};


typedef uint8_t						ServerStatus;		//服务器状态
enum
{
	enmServerStatus_None			= 0x00,				//初始状态
	enmServerStatus_Initializing	= 0x01,				//正在初始化
	enmServerStatus_Running			= 0x02,				//运行
	enmServerStatus_Paused			= 0x03,				//暂停
	enmServerStatus_Terminated		= 0x04,				//已终止
};

extern ServerCommand g_eServerCommand;		//服务器命令
extern ServerStatus g_eServerStatus;		//服务器状态

extern bool g_bTestMode;					//是否进入测试模式


//打印libframe库版本号
void print_version_frame_lib();

class CLightFrame : public IFrame_Impl
{
public:
	CLightFrame(const char* szServerName);
	virtual ~CLightFrame();

	//Frame初始化
	int32_t Initialize();
	int32_t Resume();
	int32_t Uninitialize();

	int32_t Start();
	void Run();
	int32_t Reload();
	int32_t Terminal();

	//注册配置中心
	void AddConfigCenter(int32_t nType, const char *szName, IConfigCenter* pConfigCenter);

	//注册数据中心
	int32_t AddDataCenter(IDataCenter *pDataCenter);

	int32_t GetNetConfigByDesc(const char *szDesc, NetConfig &stNetConfig) const;

public:
	virtual void RegistMsgHandler(uint32_t nMsgID, IMessageEventSS* pHandle, SS_MSG_PROC proc = NULL);

	virtual void RegistMsgHandler(uint32_t nMsgID, IMessageEventCS* pHandle, CS_MSG_PROC proc = NULL);

	virtual void RegistDefEvent(uint32_t nMsgID, IDefaultMessageSS* pHandler);

	virtual void RegistDefEvent(uint32_t nMsgID, IDefaultMessageCS* pHandler);

	//注册系统事件
	virtual void RegistSysEvent(uint16_t nEventID, ISystemEvent* pHandle);

	virtual int32_t RegistSocketHandler(CSocket *pSocket, uint16_t nListenPort);

	void UnregistSocketHandler(uint16_t nListenPort);

	virtual int32_t RegistSocketHandler(CSocket *pSocket, EntityType nServerType);

	void UnregistSocketHandler(EntityType nServerType);

	virtual int32_t SendMessageRequest(uint32_t nMsgID, IMsgBody *pMsgBody, ISessionData *pSessionData,
			const char *szDumpContent = "LibFrame DumpMessage");

	virtual int32_t SendMessageResponse(uint32_t nMsgID,MessageHeadSS *pMsgHead,IMsgBody* pMsgBody,
			const char *szDumpContent = "LibFrame DumpMessage");

	virtual int32_t SendMessageNotify(uint32_t nMsgID, IMsgBody* pMsgBody, TransType nTransType,
			EntityType nDestType, ServerID nDestID = enmInvalidServerID, RoomID nRoomID = enmInvalidRoomID,
			RoleID nRoleID = enmInvalidRoleID, const char *szDumpContent = "LibFrame DumpMessage");

	static	void DumpMessage(const char* szContent, MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,
			const uint16_t nOptionLen = 0, const char *pOptionData = NULL);

	static	void DumpMessage(const char* szContent, MessageHeadCS *pMsgHead, IMsgBody* pMsgBody,
			const uint16_t nOptionLen = 0, const char *pOptionData = NULL);

	//发送消息
	virtual int32_t PostMessage(MessageHeadSS* pMessageHead, IMsgBody *pMsgBody,
			const int32_t nOptionLen = 0, const char *pOptionData = NULL);

	//发送消息
	virtual int32_t PostMessage(MessageHeadCS* pMessageHead, IMsgBody *pMsgBody,SocketFD nSocketFD);

	//发送消息
	virtual int32_t PostMessage(MessageHeadSS* pMessageHead, const uint8_t *pMsgBodyBuf, const uint32_t nMsgBodyLen,
			const int32_t nOptionLen = 0, const char *pOptionData = NULL);

	//发送消息
	virtual int32_t PostMessage(MessageHeadCS* pMessageHead, const uint8_t *pMsgBodyBuf, const uint32_t nMsgBodyLen,SocketFD nSocketFD);

	//测试消息
	virtual int32_t PushCommand(MessageHeadSS* pMessageHead, IMsgBody *pMsgBody, const int32_t nOptionLen = 0,
			const char *pOptionData = NULL);

	//写日志
	virtual int32_t WriteLog(LogLevel loglevel, const char *szFunc, const int32_t nLineNO,
			const char *szFormat, ...);

	//写应用日志
	virtual int32_t WriteUserLog(char * szName, LogLevel logLevel, const char *szFileName, const int32_t nLineNO,const char* szFormat, ...);

	//创建会话(兼容老接口)
	virtual int32_t CreateSession(const uint32_t nMsgID, const int32_t nSessionType, ISessionEvent* pHandler, int64_t nTimeOut, CFrameSession*& pSession);

	//创建会话
	virtual int32_t CreateSession(const uint32_t nMsgID, int64_t nTimeOut, ISessionData *pSessionData, CFrameSession*& pSession);

	//删除会话
	virtual int32_t RemoveSession(const SessionIndex sessionindex);

	//设置定时器
	virtual int32_t CreateTimer(int32_t nTimerID, ITimerEvent *pHandle, ITimerData *pTimerData, int64_t nCycleTime, bool bLoop, TimerIndex& timerIndex);
	//设置定时器---proc要用MEMBER_FUNC(timer_class, timer_func)来获取
	virtual int32_t CreateTimer(TimerProc Proc, ITimerEvent *pTimer, ITimerData *pTimerData, int64_t nCycleTime, bool bLoop, TimerIndex& timerIndex);

	//删除定时器
	virtual int32_t RemoveTimer(const TimerIndex timerIndex);

	virtual int32_t GetZoneID(uint8_t nRouterType);

	virtual uint16_t GetRouterIndex(uint32_t nRouterAddress, uint16_t nRouterPort);

	virtual void SetDaemon();

	virtual void OnTestRequest();

protected:
	//检查并创建目录结构
	void MakeDir();

	template<typename FROM, typename TO>
	void GetMemberFuncAddr(FROM from, TO &to)
	{
		union
		{
			FROM f;
			TO t;
		}ut;

		ut.f = from;
		to = ut.t;
	}

protected:
	char						m_arrServerName[enmMaxStringLength];

	int32_t						m_nAppThreadCount;
	CFrameMainThread			*m_arrAppThread[enmMaxAppThreadCount];
};

FRAME_NAMESPACE_END



#endif /* LIGHTFRAME_H_ */
