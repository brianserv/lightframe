/*
 * frame_def.h
 *
 *  Created on: 2011-11-22
 *      Author: jimm
 */

#ifndef FRAME_DEF_H_
#define FRAME_DEF_H_

#include "frame_namespace.h"
#include "frame_typedef.h"
#include "common/common_def.h"
#include <stdio.h>

FRAME_NAMESPACE_BEGIN

//�ڴ�����С����
#if defined(DEBUG) || defined(_DEBUG) || defined(_DEBUG_)
enum
{
	enmMaxCenterNetQueueSize		= 0x00800000/32,				//256K
	enmMaxCSChannelSize				= 0x01000000/64,				//256K
	enmMaxSCChannelSize				= 0x01000000/64,				//256K
	enmMaxLogicNetQueueSize			= 0x02000000/*/32*/,				//1M
	enmMaxCmdQueueSize				= 0x00010000,					//64K
};
#define MAX_FRAMESESSION_COUNT		0x00001000		//4096
#define MAX_FRAMETIMER_COUNT		0x00001000		//4096
#define MAX_FRAMEONLINEPLAYER_COUNT	0x00001000		//4096
//#define MAX_BINDING_SESSION_COUNT	0x00001000		//4096
#define MAX_BINDING_SESSION_COUNT	0x00000100		//256
#else
enum
{
	enmMaxCenterNetQueueSize		= 0x00800000,				//8M
	enmMaxCSChannelSize				= 0x01000000,				//16M
	enmMaxSCChannelSize				= 0x01000000,				//16M
	enmMaxLogicNetQueueSize			= 0x02000000,				//32M
	enmMaxCmdQueueSize				= 0x00010000,				//64K
};
#define MAX_FRAMESESSION_COUNT		0x00010000		//64K
#define MAX_FRAMETIMER_COUNT		0x00010000		//64K
#define MAX_FRAMEONLINEPLAYER_COUNT	0x00010000		//64K
#define MAX_BINDING_SESSION_COUNT	0x00008000		//32K
#endif

#define FRAME_DATACENTER			"FrameDataCenter"
#define LOGQUEUE_DATACENTER			"LOGQUEUEDataCenter"
#define CENTERQUEUE_DATACENTER		"CENTERQUEUEDataCenter"
#define LOGICQUEUE_DATACENTER		"LOGICQUEUEDataCenter"
#define WEBCSQUEUE_DATACENTER		"WEBCSQUEUEDataCenter"
#define WEBSCQUEUE_DATACENTER		"WEBSCQUEUEDataCenter"
#define COMMANDQUEUE_DATACENTER		"COMMANDQUEUEDataCenter"

enum
{
	enmMaxDataCenterCount			= 64,
	enmMaxConfigCenterCount			= 32,
	enmMaxSubConfigCount			= 32,
	enmMaxConfigCenterNameLength	= 256,
	enmMaxDataCenterNameLength		= 256,
	enmMaxMsgBodyInstanceSize		= 256,

//	enmMaxQueueIndex				= 0x08,
};

////�������ȼ�����
//typedef  int8_t		PriorityType;
//enum
//{
//	enmHightPRI = 0,
//	enmLowPRI	= 1,
//	enmMaxPRI	= 2
//};

//��ʱ������
//enum
//{
//	enmFrameTimerType_Session,
//	enmFrameTimerType_Timer,
//};

enum
{
	enmRouterType_Default			= 0x00,				//Ĭ��
	enmRouterType_Sync				= 0x01,				//ͬ��
};

// ��־������Ͷ���
enum
{
	enmMaxLogFileSize						= 0x2000000,	//�����־�ļ���С
};

//��־�ļ�״̬
typedef struct tagLogFileState
{
	FILE*				pFile;								//�ļ�������
	int64_t				nActiveTime;						//�ļ��ϴλ�Ծʱ��
	int64_t				nLastFlushTime;						//�ϴ�ˢ��ʱ��
	char				szFileName[enmMaxFullPathLength];	//�ļ���
} LogFileState;

#ifdef WIN32
#define snprintf _snprintf
#endif

FRAME_NAMESPACE_END

#include "frame_timer_mgt.h"
#include "frame_session_mgt.h"
#include "frame_msgevent_mgt.h"

#endif /* FRAME_DEF_H_ */
