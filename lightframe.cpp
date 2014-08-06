/*
 * lightframe.cpp
 *
 *  Created on: 2012-10-31
 *      Author: jimm
 */

#include <stdarg.h>
#include <memory.h>
#include <stdlib.h>
#include <time.h>

#include "common/common_version.h"
#include "frame_logthread.h"
#include "frame_mainthread.h"
#include "frame_netthread.h"
#include "frame_crypt.h"
#include "lightframe.h"


#ifndef WIN32
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#endif

#define FRAME_LOG_PATH	"./log"
#define FRAME_BLL_BASE	"./bill"
#define SERVER_NAME		"LightFrame Library"

FRAME_NAMESPACE_BEGIN

ServerCommand g_eServerCommand = enmServerCommand_None;		//����������
ServerStatus g_eServerStatus = enmServerStatus_None;		//������״̬

bool g_bTestMode = false;

//��ӡlibframe��汾��
void print_version_frame_lib()
{
	char version[128] = { 0 };
	GET_VERSION(version);

	printf("%s, %s\n", SERVER_NAME, version);
#ifdef DEBUG
	printf("Build for debug\n");
#else
	printf("Build for release\n");
#endif

#ifdef SVNVER
	printf("svn version no:%d\n", SVNVER);
#endif

#ifdef SVNDIR
	printf("source dir in svn:%s\n", SVNDIR);
#endif

	printf("\n");
}

CLightFrame::CLightFrame(const char* szServerName)
{
	memset(m_arrServerName, 0, sizeof(m_arrServerName));
	strcpy(m_arrServerName, szServerName);

	m_nAppThreadCount = 0;
	memset(m_arrAppThread, 0, sizeof(m_arrAppThread));
}

CLightFrame::~CLightFrame()
{
}

static void frame_mkdir(const char *path)
{
#ifndef WIN32
	struct stat buf;
	stat(path,&buf);

	if(S_ISDIR(buf.st_mode) == 0)
	{
		mkdir(path,0777);
	}
#endif
}

void CLightFrame::MakeDir()
{
	//��鲢����logĿ¼
	frame_mkdir(FRAME_LOG_PATH);

	//��鲢����bllĿ¼
	frame_mkdir(FRAME_BLL_BASE);

	//��鲢����bll/serverĿ¼
	char bllpath[enmMaxStringLength]={0};
	sprintf(bllpath,FRAME_BLL_BASE"/%s",m_arrServerName);
	frame_mkdir(bllpath);
}

//Frame��ʼ��
int32_t CLightFrame::Initialize()
{
	int32_t nRet = S_OK;

	//��ʼ���������
	srand((unsigned)time(NULL));

	//��鲢����Ŀ¼
	MakeDir();

	//������־����
	CREATE_FRAMELOGENGINE_INSTANCE();

	//���ɷ����������ļ���
	g_FrameConfigMgt.MakeFrameBaseConfigFilePath(m_arrServerName);
	//��ʼ�����ù�����
	nRet = g_FrameConfigMgt.Initialize();
	if(nRet < 0)
	{
		return nRet;
	}

	//��ʼ������ڴ������
	nRet = g_FrameMemMgt.Initialize();
	if(nRet < 0)
	{
		return nRet;
	}
	
	//��ʼ����������
	nRet = g_FrameDalMgt.Initialize();
	if(nRet < 0)
	{
		return nRet;
	}

	//��ʼ����ʱ��������
	nRet = g_FrameTimerMgt.Initialize();
	if(nRet < 0)
	{
		return nRet;
	}

	//��ʼ���Ự������
	nRet = g_FrameSessionMgt.Initialize();
	if(nRet < 0)
	{
		return nRet;
	}

	//��ʼ����Ϣӳ�������
	nRet = g_FrameMsgEventMgt.Initialize();
	if(nRet < 0)
	{
		return nRet;
	}

	//��ʼ�����ӹ�����
	nRet = g_FrameSocketMgt.Initialize();
	if(nRet < 0)
	{
		return nRet;
	}

	//��ʼ��������Ϣ������
	nRet = g_FrameConnectionMgt.Initialize();
	if(nRet < 0)
	{
		return nRet;
	}

	//������ڲ���ģʽ
	if(g_bTestMode)
	{
		//��ʼ��������Ϣ����
		nRet = g_FrameCommandQueue.Initialize();
		if(nRet < 0)
		{
			return nRet;
		}
	}

	//��ʼ���������
	nRet = g_FrameNetQueue.Initialize();
	if(nRet < 0)
	{
		return nRet;
	}

	//��ʼ����־�̶߳���
	nRet = g_FrameLogThread.Initialize();
	if(nRet < 0)
	{
		return nRet;
	}

	//��ʼ�������̶߳���
	nRet = g_FrameNetThread.Initialize();
	if(nRet < 0)
	{
		return nRet;
	}

	//��ʼ�����̶߳���
	int32_t nAppThreadCount = g_FrameConfigMgt.GetFrameBaseConfig().GetAppThreadCount();
	if(nAppThreadCount > enmMaxAppThreadCount)
	{
		nAppThreadCount = enmMaxAppThreadCount;
	}
	for(int32_t i = 0; i < nAppThreadCount; ++i)
	{
		CFrameMainThread *pThread = new CFrameMainThread();
		if(pThread == NULL)
		{
			return E_NULLPOINTER;
		}

		nRet = pThread->Initialize();
		if(nRet < 0)
		{
			return nRet;
		}

		pThread->SetThreadIndex(i);

		m_arrAppThread[m_nAppThreadCount] = pThread;
		++m_nAppThreadCount;
	}

	nRet = g_FrameTimerTask.Start();
	if(nRet < 0)
	{
		return nRet;
	}

	g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "lightframe init finished, %s is ready to start!\n", m_arrServerName);

	return nRet;
}

int32_t CLightFrame::Resume()
{
	return Reload();
}

int32_t CLightFrame::Uninitialize()
{
	return S_OK;
}

int32_t CLightFrame::Start()
{
	//������־�߳�
	g_FrameLogThread.Start();

	//�������߳�
	for(int32_t i = 0; i < m_nAppThreadCount; ++i)
	{
		if(m_arrAppThread[i] == NULL)
		{
			continue;
		}

		m_arrAppThread[i]->Start();
	}

	//���������߳�
	g_FrameNetThread.Start();

	g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "lightframe start finished, %s is start success!\n", m_arrServerName);

	return S_OK;
}

void CLightFrame::Run()
{
	ServerCommand eServerCommand = enmServerCommand_None;

	while (true)
	{
		eServerCommand = g_eServerCommand;

		if (enmServerCommand_Reload == eServerCommand)
		{
			//WriteLog(LOGPREFIX, enmLogLevel_Notice, "Activity server received reload signal!\n");
			g_eServerCommand = enmServerCommand_None;
			Reload();
		}
		else if (enmServerCommand_Terminate == eServerCommand)
		{
			//WriteLog(LOGPREFIX, enmLogLevel_Notice, "Activity server received terminate signal!\n");

			//֪ͨLibFrame��������
			Terminal();
			break;
		}

		if (g_bTestMode)
		{
			OnTestRequest();
		}

		//TODO:
		Delay(100000);
	}

	g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "%s terminated!\n", m_arrServerName);
}

int32_t CLightFrame::Reload()
{
	//��ʼ���������
	srand((unsigned)time(NULL));

	int32_t nRet = g_FrameConfigMgt.Resume();
	if(nRet < 0)
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "lightframe reload is failed!{nRet=0x%08x}\n", nRet);
	}
	else
	{
		g_FrameLogEngine.WriteBaseLog(enmLogLevel_Notice, "lightframe reload finished!\n");
	}

	return nRet;
}

int32_t CLightFrame::Terminal()
{
	//��ֹ�����߳�
	g_FrameNetThread.Terminate();

	//��ֹ���߳�
	for(int32_t i = 0; i < m_nAppThreadCount; ++i)
	{
		if(m_arrAppThread[i] == NULL)
		{
			continue;
		}

		m_arrAppThread[i]->Terminate();
	}

	//��ֹ��־�߳�
	g_FrameLogThread.Terminate();

	return S_OK;
}

//ע����������
void CLightFrame::AddConfigCenter(int32_t nType, const char *szName, IConfigCenter* pConfigCenter)
{
	g_FrameConfigMgt.AddUsrConfigCenter(nType, szName, pConfigCenter);
}

//ע����������
int32_t CLightFrame::AddDataCenter(IDataCenter *pDataCenter)
{
	g_FrameDalMgt.AddDataCenter(pDataCenter);
	return S_OK;
}

int32_t CLightFrame::GetNetConfigByDesc(const char *szDesc, NetConfig &stNetConfig) const
{
	return g_FrameConfigMgt.GetFrameBaseConfig().GetNetConfigByDesc(szDesc, stNetConfig);
}

//ע����Ϣ������
void CLightFrame::RegistMsgHandler(uint32_t nMsgID, IMessageEventSS* pHandler, SS_MSG_PROC proc/* = NULL*/)
{
	g_FrameMsgEventMgt.AddMsgEvent(nMsgID, pHandler, proc);
}

//ע����Ϣ������
void CLightFrame::RegistMsgHandler(uint32_t nMsgID, IMessageEventCS* pHandler, CS_MSG_PROC proc/* = NULL*/)
{
	g_FrameMsgEventMgt.AddMsgEvent(nMsgID, pHandler, proc);
}

//ע��Ĭ�ϵ���Ϣ������ܲ�decode body��
void CLightFrame::RegistDefEvent(uint32_t nMsgID, IDefaultMessageSS* pHandler)
{
	g_FrameMsgEventMgt.AddDefEvent(nMsgID, pHandler);
}

//ע����Ϣ������
void CLightFrame::RegistDefEvent(uint32_t nMsgID, IDefaultMessageCS* pHandler)
{
	g_FrameMsgEventMgt.AddDefEvent(nMsgID, pHandler);
}

//ע��ϵͳ�¼�
void CLightFrame::RegistSysEvent(uint16_t nEventID, ISystemEvent* pHandle)
{
	g_FrameMsgEventMgt.AddSysEvent(nEventID, pHandle);
}

int32_t CLightFrame::RegistSocketHandler(CSocket *pSocket, uint16_t nListenPort)
{
	return g_FrameSocketMgt.RegistSocketHandler(pSocket, nListenPort);
}

void CLightFrame::UnregistSocketHandler(uint16_t nListenPort)
{
	return g_FrameSocketMgt.UnregistSocketHandler(nListenPort);
}

int32_t CLightFrame::RegistSocketHandler(CSocket *pSocket, EntityType nServerType)
{
	return g_FrameSocketMgt.RegistSocketHandler(pSocket, nServerType);
}

void CLightFrame::UnregistSocketHandler(EntityType nServerType)
{
	return g_FrameSocketMgt.UnregistSocketHandler(nServerType);
}

int32_t CLightFrame::SendMessageRequest(uint32_t nMsgID, IMsgBody *pMsgBody, ISessionData *pSessionData,
		const char *szDumpContent)
{
	int32_t ret = S_OK;

	CFrameMessageConfig &stFrameMessageConfig = g_FrameConfigMgt.GetFrameMessageConfig();
	CFrameBaseConfig &stFrameBaseConfig = g_FrameConfigMgt.GetFrameBaseConfig();

	//��λ��΢��
	int64_t nTimeout = stFrameMessageConfig.GetTimeoutValue(nMsgID) * US_PER_SECOND;
	//����Ҫ�����Ự
	if((nTimeout <= 0) || (pSessionData == NULL))
	{
		return E_NULLPOINTER;
	}

	uint32_t nRespMsgID = stFrameMessageConfig.GetRespMsgID(nMsgID);
	if(nRespMsgID == 0)
	{
		return E_UNKNOWN;
	}

	CFrameSession *pSession = NULL;
	ret = CreateSession(nRespMsgID, nTimeout, pSessionData, pSession);
	if(ret < 0 || pSession == NULL)
	{
		WriteLog(enmLogLevel_Error, __func__, __LINE__, "null pointer:create session is failed!{RoleID=%d, ret=0x%08x}\n",
				pSessionData->GetRoleID(), ret);
		return ret;
	}

	SessionIndex nSessionIndex = pSession->GetSessionIndex();
	TransType nTransType = stFrameMessageConfig.GetTransType(nMsgID);

	MessageHeadSS stSendMessage;
	stSendMessage.nMessageID = nMsgID;
	stSendMessage.nMessageType = enmMessageType_Request;
	stSendMessage.nSourceType = stFrameBaseConfig.GetServerType();
	stSendMessage.nDestType = stFrameMessageConfig.GetDestType(nMsgID);
	stSendMessage.nSourceID = stFrameBaseConfig.GetServerID();
	if(nTransType == enmTransType_ByKey)
	{
		stSendMessage.nDestID = pSessionData->GetRoleID();
	}
	else
	{
		nTransType = enmTransType_Ramdom;
		stSendMessage.nDestID = 0;
	}
	stSendMessage.nRoleID = pSessionData->GetRoleID();
	stSendMessage.nSequence = 0;
	stSendMessage.nTransType = nTransType;
	stSendMessage.nSessionIndex = nSessionIndex;
	stSendMessage.nRoomID = pSessionData->GetRoomID();
	stSendMessage.nZoneID = 0;
	stSendMessage.nRouterIndex = 0;

	DumpMessage(szDumpContent, &stSendMessage, pMsgBody, 0, NULL);

	return PostMessage(&stSendMessage, pMsgBody, 0, NULL);
}

int32_t CLightFrame::SendMessageResponse(uint32_t nMsgID, MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,
		const char *szDumpContent)
{
	CFrameBaseConfig &stFrameBaseConfig = g_FrameConfigMgt.GetFrameBaseConfig();

	MessageHeadSS stSendMessage;
	stSendMessage.nMessageID = nMsgID;
	stSendMessage.nMessageType = enmMessageType_Response;
	stSendMessage.nSourceType = stFrameBaseConfig.GetServerType();
	stSendMessage.nDestType = pMsgHead->nSourceType;
	stSendMessage.nSourceID =  stFrameBaseConfig.GetServerID();
	stSendMessage.nDestID = pMsgHead->nSourceID;
	stSendMessage.nRoleID = pMsgHead->nRoleID;
	stSendMessage.nSequence = pMsgHead->nSequence;
	stSendMessage.nTransType = enmTransType_P2P;
	stSendMessage.nSessionIndex = pMsgHead->nSessionIndex;
	stSendMessage.nRoomID = pMsgHead->nRoomID;
	stSendMessage.nZoneID = pMsgHead->nZoneID;
	stSendMessage.nRouterIndex = pMsgHead->nRouterIndex;

	DumpMessage(szDumpContent, &stSendMessage, pMsgBody, 0, NULL);
	return PostMessage(&stSendMessage, pMsgBody, 0, NULL);
}

int32_t CLightFrame::SendMessageNotify(uint32_t nMsgID, IMsgBody* pMsgBody, TransType nTransType,
		EntityType nDestType, ServerID nDestID, RoomID nRoomID, RoleID nRoleID, const char *szDumpContent)
{
	CFrameBaseConfig &stFrameBaseConfig = g_FrameConfigMgt.GetFrameBaseConfig();

	MessageHeadSS stSendMessage;
	stSendMessage.nMessageID = nMsgID;
	stSendMessage.nMessageType = enmMessageType_Notify;
	stSendMessage.nSourceType = stFrameBaseConfig.GetServerType();
	stSendMessage.nDestType = nDestType;
	stSendMessage.nDestID = nDestID;
	stSendMessage.nSourceID =  stFrameBaseConfig.GetServerID();
	if(nTransType == enmTransType_ByKey)
	{
		nTransType = enmTransType_Ramdom;
	}
	else if(nTransType == enmTransType_Broadcast)
	{
		stSendMessage.nDestID = 0;
	}

	stSendMessage.nRoleID = nRoleID;
	stSendMessage.nSequence = 0;
	stSendMessage.nTransType = nTransType;
	stSendMessage.nSessionIndex = enmInvalidSessionIndex;
	stSendMessage.nRoomID = nRoomID;
	stSendMessage.nZoneID = 0;
	stSendMessage.nRouterIndex = 0;

	DumpMessage(szDumpContent,&stSendMessage,pMsgBody, 0, NULL);
	return PostMessage(&stSendMessage, pMsgBody, 0, NULL);
}

void CLightFrame::DumpMessage(const char* szContent, MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const char *pOptionData)
{
	uint32_t offset = 0;
	char szLog[enmMaxLogInfoLength];
	szLog[0] = 0;
	sprintf(szLog + offset, szContent);
	offset = (uint32_t)strlen(szLog);

	sprintf(szLog + offset, " MessageHeadSS=");
	offset = (uint32_t)strlen(szLog);
	pMsgHead->Dump(szLog, enmMaxLogInfoLength, offset);

	if(pMsgBody)
	{
		sprintf(szLog + offset, " MessageBody=");
		offset = (uint32_t)strlen(szLog);
		pMsgBody->Dump(szLog, enmMaxLogInfoLength, offset);
	}

	g_FrameLogEngine.WriteMainLog(enmLogLevel_Notice, "%s\n", szLog);
}

void CLightFrame::DumpMessage(const char* szContent, MessageHeadCS *pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const char *pOptionData)
{
	uint32_t offset = 0;
	char szLog[enmMaxLogInfoLength];
	szLog[0] = 0;
	sprintf(szLog + offset, szContent);
	offset = (uint32_t)strlen(szLog);

	sprintf(szLog + offset, " MessageHeadCS=");
	offset = (uint32_t)strlen(szLog);
	pMsgHead->Dump(szLog, enmMaxLogInfoLength, offset);

	if(pMsgBody)
	{
		sprintf(szLog + offset, " MessageBody=");
		offset = (uint32_t)strlen(szLog);
		pMsgBody->Dump(szLog, enmMaxLogInfoLength, offset);
	}

	g_FrameLogEngine.WriteMainLog(enmLogLevel_Notice, "%s\n", szLog);
}

int32_t CLightFrame::PushCommand(MessageHeadSS* pMessageHead, IMsgBody *pMsgBody, const int32_t nOptionLen,
		const char *pOptionData)
{
	int32_t nPacketLen = sizeof(MessageHeadSS) + pMsgBody->GetSize() + nOptionLen;

	uint8_t *pMem = MALLOC(nPacketLen);//g_FrameMemMgt.AllocBlock(nPacketLen);
	if(pMem == NULL)
	{
		WriteLog(enmLogLevel_Error, __func__, __LINE__, "nPacketLen is too big!{nPacketLen=%d, "
				"MsgBodySize=%d, nOptionLen=%d}\n", nPacketLen, pMsgBody->GetSize(), nOptionLen);
		return E_NULLPOINTER;
	}

	NetPacket *pNetPacket = new(pMem)NetPacket();

	uint32_t nMessageHeadBufOffset = 0;
	int32_t nRet = pMessageHead->MessageEncode(&pNetPacket->m_pNetPacket[0], nPacketLen, nMessageHeadBufOffset);
	if (0 > nRet)
	{
		FREE(pMem);
		return nRet;
	}

	if((nOptionLen > 0) && (pOptionData != NULL))
	{
		memcpy(&pNetPacket->m_pNetPacket[nMessageHeadBufOffset], pOptionData, nOptionLen);
		nMessageHeadBufOffset += nOptionLen;
	}

	uint32_t offset = nMessageHeadBufOffset;

	uint32_t len = 0;
	nRet = pMsgBody->MessageEncode(&pNetPacket->m_pNetPacket[offset], nPacketLen - offset, len);
	if(0 > nRet)
	{
		FREE(pMem);
		return nRet;
	}

	pMessageHead->nTotalSize = nMessageHeadBufOffset + len;
	nPacketLen = pMessageHead->nTotalSize;

	uint32_t nTotalSize = nPacketLen;
	uint32_t nTotalSizeOffset = offsetof(MessageHeadSS, nTotalSize);
	nRet = CCodeEngine::Encode(&pNetPacket->m_pNetPacket[0], nPacketLen, nTotalSizeOffset, nTotalSize);
	if(0 > nRet)
	{
		FREE(pMem);
		return nRet;
	}

	uint16_t nHeadSize = nMessageHeadBufOffset;
	uint32_t nHeadSizeOffset = offsetof(MessageHeadSS, nHeadSize);
	nRet = CCodeEngine::Encode(&pNetPacket->m_pNetPacket[0], nPacketLen, nHeadSizeOffset, nHeadSize);
	if(0 > nRet)
	{
		FREE(pMem);
		return nRet;
	}

	pNetPacket->m_nNetPacketLen = nPacketLen;

	return g_FrameCommandQueue.Push(pNetPacket);
}

//������Ϣ
int32_t CLightFrame::PostMessage(MessageHeadSS* pMessageHead, IMsgBody *pMsgBody, const int32_t nOptionLen,
		const char *pOptionData)
{
	int32_t nPacketLen = sizeof(MessageHeadSS) + pMsgBody->GetSize() + nOptionLen;

	uint8_t *pMem = MALLOC(nPacketLen);//g_FrameMemMgt.AllocBlock(nPacketLen);
	if(pMem == NULL)
	{
		WriteLog(enmLogLevel_Error, __func__, __LINE__, "nPacketLen is too big!{nPacketLen=%d, "
				"MsgBodySize=%d, nOptionLen=%d}\n", nPacketLen, pMsgBody->GetSize(), nOptionLen);
		return E_NULLPOINTER;
	}

	NetPacket *pNetPacket = new(pMem)NetPacket();

	uint32_t nMessageHeadBufOffset = 0;
	int32_t nRet = pMessageHead->MessageEncode(&pNetPacket->m_pNetPacket[0], nPacketLen, nMessageHeadBufOffset);
	if (0 > nRet)
	{
		FREE(pMem);
		return nRet;
	}

	if((nOptionLen > 0) && (pOptionData != NULL))
	{
		memcpy(&pNetPacket->m_pNetPacket[nMessageHeadBufOffset], pOptionData, nOptionLen);
		nMessageHeadBufOffset += nOptionLen;
	}

	uint32_t offset = nMessageHeadBufOffset;

	uint32_t len = 0;
	nRet = pMsgBody->MessageEncode(&pNetPacket->m_pNetPacket[offset], nPacketLen - offset, len);
	if(0 > nRet)
	{
		FREE(pMem);
		return nRet;
	}

	pMessageHead->nTotalSize = nMessageHeadBufOffset + len;
	nPacketLen = pMessageHead->nTotalSize;

	uint32_t nTotalSize = nPacketLen;
	uint32_t nTotalSizeOffset = offsetof(MessageHeadSS, nTotalSize);
	nRet = CCodeEngine::Encode(&pNetPacket->m_pNetPacket[0], nPacketLen, nTotalSizeOffset, nTotalSize);
	if(0 > nRet)
	{
		FREE(pMem);
		return nRet;
	}

	uint16_t nHeadSize = nMessageHeadBufOffset;
	uint32_t nHeadSizeOffset = offsetof(MessageHeadSS, nHeadSize);
	nRet = CCodeEngine::Encode(&pNetPacket->m_pNetPacket[0], nPacketLen, nHeadSizeOffset, nHeadSize);
	if(0 > nRet)
	{
		FREE(pMem);
		return nRet;
	}

	pNetPacket->m_nNetPacketLen = nPacketLen;

	if(!g_FrameNetQueue.IsSendSSQueueFull())
	{
		return g_FrameNetQueue.PushSendSSQueue(pNetPacket);
	}
	else
	{
		WriteLog(enmLogLevel_Error, __func__, __LINE__, "send queue is full!\n");
	}
	FREE(pMem);

	return S_OK;
}
//������Ϣ
int32_t CLightFrame::PostMessage(MessageHeadCS* pMessageHead, IMsgBody *pMsgBody, SocketFD nSocketFD)
{
	//ENCRYPT_BLOCK_LENGTH_IN_BYTE�Ǽ��ܺ���������𲽰���������ݴ�С
	int32_t nPacketLen = sizeof(MessageHeadCS) + sizeof(ConnInfo) + pMsgBody->GetSize() + ENCRYPT_BLOCK_LENGTH_IN_BYTE;

	uint8_t *pMem = MALLOC(nPacketLen);//g_FrameMemMgt.AllocBlock(nPacketLen);
	if(pMem == NULL)
	{
		WriteLog(enmLogLevel_Error, __func__, __LINE__, "nPacketLen is too big!{nPacketLen=%d, "
				"MsgBodySize=%d}\n", nPacketLen, pMsgBody->GetSize());
		return E_NULLPOINTER;
	}

	NetPacket *pNetPacket = new(pMem)NetPacket();

	CFrameConnection *pConnection = g_FrameConnectionMgt.GetConnection(nSocketFD);
	if(pConnection == NULL)
	{
		FREE(pMem);
		return E_UNKNOWN;
	}

	memcpy(&pNetPacket->m_pNetPacket[0], &pConnection->m_stConnInfo, sizeof(ConnInfo));

	//ȥ��conninfo���ڴ��׵�ַ�����ݴ�С
	uint8_t *pBufCS = &pNetPacket->m_pNetPacket[sizeof(ConnInfo)];
	uint32_t nBufSizeCS = nPacketLen - sizeof(ConnInfo);

	uint32_t nMessageHeadBufOffset = 0;
	int32_t nRet = pMessageHead->MessageEncode(pBufCS, nBufSizeCS, nMessageHeadBufOffset);
	if (0 > nRet)
	{
		FREE(pMem);
		return nRet;
	}

	uint32_t offset = nMessageHeadBufOffset;

	uint32_t len = 0;
	nRet = pMsgBody->MessageEncode(pBufCS + offset, nBufSizeCS - offset, len);
	if(0 > nRet)
	{
		FREE(pMem);
		return nRet;
	}

	int32_t nOutSize = 0;
	if(len > 0)
	{
		uint32_t nEncryptBodySize = len;
		//������Ϣ��
		nOutSize = CFrameCrypt::Encrypt(pMessageHead, pBufCS + offset, nEncryptBodySize, pConnection);
		if(nOutSize <= 0)
		{
			FREE(pMem);
			return E_UNKNOWN;
		}
	}
	pMessageHead->nTotalSize = nMessageHeadBufOffset + nOutSize;
	nPacketLen = pMessageHead->nTotalSize + sizeof(ConnInfo);

	uint16_t nTotalSize = pMessageHead->nTotalSize;
	uint32_t nTotalSizeOffset = offsetof(MessageHeadCS, nTotalSize);
	nRet = CCodeEngine::Encode(pBufCS, nBufSizeCS, nTotalSizeOffset, nTotalSize);
	if(0 > nRet)
	{
		FREE(pMem);
		return nRet;
	}

	pNetPacket->m_nNetPacketLen = nPacketLen;

	if(!g_FrameNetQueue.IsSendCSQueueFull())
	{
		return g_FrameNetQueue.PushSendCSQueue(pNetPacket);
	}
	else
	{
		WriteLog(enmLogLevel_Error, __func__, __LINE__, "send queue is full!\n");
	}
	FREE(pMem);

	return S_OK;
}
//������Ϣ
int32_t CLightFrame::PostMessage(MessageHeadSS* pMessageHead, const uint8_t *pMsgBodyBuf, const uint32_t nMsgBodyLen,const int32_t nOptionLen,
		const char *pOptionData)
{
	int32_t nPacketLen = sizeof(MessageHeadSS) + nMsgBodyLen + nOptionLen;

	uint8_t *pMem = MALLOC(nPacketLen);//g_FrameMemMgt.AllocBlock(nPacketLen);
	if(pMem == NULL)
	{
		WriteLog(enmLogLevel_Error, __func__, __LINE__, "nPacketLen is too big!{nPacketLen=%d, "
				"MsgBodySize=%d, nOptionLen=%d}\n", nPacketLen, nMsgBodyLen, nOptionLen);
		return E_NULLPOINTER;
	}

	NetPacket *pNetPacket = new(pMem)NetPacket();

	uint32_t nMessageHeadBufOffset = 0;
	int32_t nRet = pMessageHead->MessageEncode(&pNetPacket->m_pNetPacket[0], nPacketLen, nMessageHeadBufOffset);
	if (0 > nRet)
	{
		FREE(pMem);
		return nRet;
	}

	if((nOptionLen > 0) && (pOptionData != NULL))
	{
		memcpy(&pNetPacket->m_pNetPacket[nMessageHeadBufOffset], pOptionData, nOptionLen);
		nMessageHeadBufOffset += nOptionLen;
	}

	uint32_t offset = nMessageHeadBufOffset;

	memcpy(&pNetPacket->m_pNetPacket[offset], pMsgBodyBuf, nMsgBodyLen);
	pMessageHead->nTotalSize = nMessageHeadBufOffset + nMsgBodyLen;
	nPacketLen = pMessageHead->nTotalSize;

	uint32_t nTotalSize = pMessageHead->nTotalSize;
	uint32_t nTotalSizeOffset = offsetof(MessageHeadSS, nTotalSize);
	nRet = CCodeEngine::Encode(&pNetPacket->m_pNetPacket[0], nPacketLen, nTotalSizeOffset, nTotalSize);
	if(0 > nRet)
	{
		FREE(pMem);
		return nRet;
	}

	uint16_t nHeadSize = nMessageHeadBufOffset;
	uint32_t nHeadSizeOffset = offsetof(MessageHeadSS, nHeadSize);
	nRet = CCodeEngine::Encode(&pNetPacket->m_pNetPacket[0], nPacketLen, nHeadSizeOffset, nHeadSize);
	if(0 > nRet)
	{
		FREE(pMem);
		return nRet;
	}

	pNetPacket->m_nNetPacketLen = nPacketLen;

	if(!g_FrameNetQueue.IsSendSSQueueFull())
	{
		return g_FrameNetQueue.PushSendSSQueue(pNetPacket);
	}
	else
	{
		WriteLog(enmLogLevel_Error, __func__, __LINE__, "send queue is full!\n");
	}
	FREE(pMem);

	return S_OK;
}

//������Ϣ
int32_t CLightFrame::PostMessage(MessageHeadCS* pMessageHead, const uint8_t *pMsgBodyBuf,const uint32_t nMsgBodyLen, SocketFD nSocketFD)
{
	//ENCRYPT_BLOCK_LENGTH_IN_BYTE�Ǽ��ܺ���������𲽰���������ݴ�С
	int32_t nPacketLen = sizeof(MessageHeadCS) + sizeof(ConnInfo) + nMsgBodyLen + ENCRYPT_BLOCK_LENGTH_IN_BYTE;

	uint8_t *pMem = MALLOC(nPacketLen);//g_FrameMemMgt.AllocBlock(nPacketLen);
	if(pMem == NULL)
	{
		WriteLog(enmLogLevel_Error, __func__, __LINE__, "nPacketLen is too big!{nPacketLen=%d, "
				"MsgBodySize=%d}\n", nPacketLen, nMsgBodyLen);
		return E_NULLPOINTER;
	}

	NetPacket *pNetPacket = new(pMem)NetPacket();

	CFrameConnection *pConnection = g_FrameConnectionMgt.GetConnection(nSocketFD);
	if(pConnection == NULL)
	{
		FREE(pMem);
		return E_UNKNOWN;
	}

	memcpy(&pNetPacket->m_pNetPacket[0], &pConnection->m_stConnInfo, sizeof(ConnInfo));

	//ȥ��conninfo���ڴ��׵�ַ�����ݴ�С
	uint8_t *pBufCS = &pNetPacket->m_pNetPacket[sizeof(ConnInfo)];
	uint32_t nBufSizeCS = nPacketLen - sizeof(ConnInfo);

	uint32_t nMessageHeadBufSize = sizeof(MessageHeadCS);
	uint32_t nMessageHeadBufOffset = 0;
	uint8_t arrMessageHeadBuf[nMessageHeadBufSize];
	int32_t nRet = pMessageHead->MessageEncode(arrMessageHeadBuf, nMessageHeadBufSize, nMessageHeadBufOffset);
	if (0 > nRet)
	{
		FREE(pMem);
		return nRet;
	}

	uint32_t offset = 0;
	memcpy(pBufCS , arrMessageHeadBuf, nMessageHeadBufOffset);
	offset += nMessageHeadBufOffset;

	memcpy(pBufCS + offset, pMsgBodyBuf, nMsgBodyLen);
//	offset += nMsgBodyLen;

	int32_t nOutSize = 0;
	if(nMsgBodyLen > 0)
	{
		uint32_t nEncryptBodySize = nMsgBodyLen;
		//������Ϣ��
		nOutSize = CFrameCrypt::Encrypt(pMessageHead, pBufCS + offset, nEncryptBodySize, pConnection);
		if(nOutSize <= 0)
		{
			FREE(pMem);
			return E_UNKNOWN;
		}
	}
	pMessageHead->nTotalSize = nMessageHeadBufOffset + nOutSize;
	nPacketLen = pMessageHead->nTotalSize + sizeof(ConnInfo);

	uint16_t nTotalSize = pMessageHead->nTotalSize;
	uint32_t nTotalSizeOffset = offsetof(MessageHeadCS, nTotalSize);
	nRet = CCodeEngine::Encode(pBufCS, nBufSizeCS, nTotalSizeOffset, nTotalSize);
	if(0 > nRet)
	{
		FREE(pMem);
		return nRet;
	}

	pNetPacket->m_nNetPacketLen = nPacketLen;

	if(!g_FrameNetQueue.IsSendCSQueueFull())
	{
		return g_FrameNetQueue.PushSendCSQueue(pNetPacket);
	}
	else
	{
		WriteLog(enmLogLevel_Error, __func__, __LINE__, "send queue is full!\n");
	}
	FREE(pMem);

	return S_OK;
}

//д��־
int32_t CLightFrame::WriteLog(LogLevel loglevel, const char *szFunc, const int32_t nLineNO, const char *szFormat, ...)
{
	//�жϵȼ��Ƿ��㹻
	if (loglevel < g_FrameConfigMgt.GetFrameBaseConfig().GetLogLevel())
	{
		return E_LIBF_LOGLEVEL;
	}

	char szLog[enmMaxLogInfoLength];

	//������־��Ϣ
	va_list ap;
	va_start(ap, szFormat);
	vsprintf(szLog, szFormat, ap);
	va_end(ap);

	return g_FrameLogEngine.WriteMainLog(loglevel, szFunc, nLineNO, szLog);
};

int32_t CLightFrame::WriteUserLog(char *szName, LogLevel logLevel, const char *szFileName, const int32_t nLineNO,const char* szFormat, ...)
{
	//�жϵȼ��Ƿ��㹻
	if (logLevel < g_FrameConfigMgt.GetFrameBaseConfig().GetLogLevel())
	{
		return E_LIBF_LOGLEVEL;
	}

	char szLog[enmMaxLogInfoLength];

	//������־��Ϣ
	va_list ap;
	va_start(ap, szFormat);
	vsprintf(szLog, szFormat, ap);
	va_end(ap);

	return g_FrameLogEngine.WriteUserLog(logLevel, szName, szFileName, nLineNO, szLog);
}

//�����Ự(�����Ͻӿ�)
int32_t CLightFrame::CreateSession(const uint32_t nMsgID, const int32_t nSessionType, ISessionEvent* pHandler, int64_t nTimeOut, CFrameSession*& pSession)
{
	return g_FrameSessionMgt.CreateSession(nMsgID, nSessionType, pHandler, nTimeOut, pSession);
}

//�����Ự
int32_t CLightFrame::CreateSession(const uint32_t nMsgID, int64_t nTimeOut, ISessionData *pSessionData, CFrameSession*& pSession)
{
	return g_FrameSessionMgt.CreateSession(nMsgID, nTimeOut, pSessionData, pSession);
}

//ɾ���Ự
int32_t CLightFrame::RemoveSession(const SessionIndex sessionIndex)
{
	return g_FrameSessionMgt.RemoveSession(sessionIndex);
}

//���ö�ʱ��
int32_t CLightFrame::CreateTimer(int32_t nTimerID, ITimerEvent *pHandle, ITimerData *pTimerData, int64_t nCycleTime, bool bLoop, TimerIndex& timerIndex)
{
	return g_FrameTimerMgt.CreateTimer(nTimerID, pHandle, pTimerData, nCycleTime, bLoop, timerIndex);
}

//���ö�ʱ��
int32_t CLightFrame::CreateTimer(TimerProc Proc, ITimerEvent *pTimer, ITimerData *pTimerData, int64_t nCycleTime, bool bLoop, TimerIndex& timerIndex)
{
	return g_FrameTimerMgt.CreateTimer(Proc, pTimer, pTimerData, nCycleTime, bLoop, timerIndex);
}

//ȡ����ʱ��
int32_t CLightFrame::RemoveTimer(const TimerIndex timerIndex)
{
	return g_FrameTimerMgt.RemoveTimer(timerIndex);
}

int32_t CLightFrame::GetZoneID(uint8_t nRouterType)
{
	return g_FrameNetThread.GetZoneID(nRouterType);
}

uint16_t CLightFrame::GetRouterIndex(uint32_t nRouterAddress, uint16_t nRouterPort)
{
	return g_FrameNetThread.GetRouterIndex(nRouterAddress, nRouterPort);
}

//��ʼ��Ϊ�ػ�����
void CLightFrame::SetDaemon(void)
{
	pid_t pid = 0;

	if ((pid = fork()) != 0)
	{
		exit(0);
	}

	setsid();

	signal(SIGINT,  SIG_IGN);
	signal(SIGHUP,  SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	signal(SIGHUP,  SIG_IGN);

	struct sigaction sig;
	sig.sa_handler = SIG_IGN;
	sig.sa_flags = 0;
	sigemptyset(&sig.sa_mask);
	sigaction(SIGPIPE, &sig, NULL);

	if ((pid = fork()) != 0)
	{
		exit(0);
	}

	umask(0);
}

void CLightFrame::OnTestRequest()
{

}

FRAME_NAMESPACE_END

