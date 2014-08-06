/*
 * common_def.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_DEF_H_
#define COMMON_DEF_H_

#include "common_typedef.h"


/*********************************************************************
* ʱ��������س�������
*********************************************************************/
#define US_PER_MS				((int64_t)1000)
#define US_PER_SECOND			((int64_t)(1000 * US_PER_MS))
#define US_PER_MINUTE			((int64_t)(60 * US_PER_SECOND))
#define US_PER_HOUR				((int64_t)(60 * US_PER_MINUTE))
#define US_PER_DAY				((int64_t)(24 * US_PER_HOUR))

#define MS_PER_SECOND			1000
#define SECOND_PER_MINUTE		60
#define MINUTE_PER_HOUR			60
#define HOUR_PER_DAY			24
#define DAY_PER_YEAR			365

#define SECOND_PER_HOUR			(MINUTE_PER_HOUR * SECOND_PER_MINUTE)
#define SECOND_PER_DAY			(HOUR_PER_DAY * SECOND_PER_HOUR)
#define SECOND_PER_WEEK			(SECOND_PER_DAY * 7)
#define SECOND_PER_YEAR			(DAY_PER_YEAR * SECOND_PER_DAY)


/*********************************************************************
* �ַ���������ض���
*********************************************************************/
enum
{
	enmMaxDateTimeStringLength			= 32,	//����ʱ���ַ�����󳤶�
	enmMaxFileNameLength				= 256,	//�ļ�����󳤶�,
	enmMaxConfigCount                   = 32,   //���һ�����¼��ص������ļ��ĸ���
	enmMaxConfigNameLength              = 64,   //�����ļ����Ƶ���󳤶�
	enmMaxFullPathLength				= 256,	//�ļ�ȫ·����󳤶�,
	enmMaxIPAddressLength				= 20,	//IP��ַ��󳤶�
	enmMaxFieldValueLength				= 1024*10,//���ݿ��ֶ�ֵ��󳤶�
	enmMaxLogInfoLength					= 0x40000,	//������־��¼��󳤶�(256K)
	enmMaxDBUserLength					= 256,	//���ݿ��û�����󳤶�
	enmMaxDBPasswordLength				= 32,	//���ݿ�������󳤶�
	enmMaxDBTableLength					= 32,	//���ݿ��������󳤶�
	enmMaxDateLength					= 9,	//�����ַ�������󳤶�
	enmMaxStringLength					= 4096,	//��ͨ�ַ�����󳤶�
	enmMaxOptionLen                     = 64,   //��󸽼��ֶγ���
	enmDefaultStringLength				= 256,	//��ͨ�ַ���Ĭ�ϳ���
	enmIDCardLength						= 19,	//�û����֤�ų���
	enmIDCardLength2					= 16,	//�û����֤�ų���
	enmMaxLogFileNumber					= 1000,	//��־�ļ�����������
	enmMaxIPStringLength				= 20,	//IP�ַ�������󳤶�
	enmMaxIPCount						= 10,	//����IP����
	enmMaxPortStringLength				= 64,	//IP�ַ�������󳤶�
	enmMaxPortCount						= 10,
	enmMaxDBCacheCount					= 2,	//dbcache�����Ŀ
	enmMaxDBNameLength					= 64,
	enmMaxDataBaseCount					= 32,	//dmz������server�õ������ݿ������ܺ�
	enmMaxRoleCountPerQuery				= 100, //һ�β�ѯ����������Ŀ
	enmTableCountPerDB					= 4,  //ÿ��DB�ķֱ���Ŀ
	enmDefaultDBFactor					= 256,
	enmDefaultDBPort					= 3306,
	enmMaxMsgIDNumber                   = 256,  //GM͸��Э����������
	enmMaxNoticeStringLength            = 512,  //���֪ͨ�ַ�������
	enmMaxRoomThemeLength               = 128,  //��󷿼������ַ�������
	enmMaxRoomPasswordLength            = 32,   
	enmMaxFieldCount                    = 32,   //һ�ű�����ж��ٸ��ֶ�
	enmMaxAppThreadCount				= 8,	//����ҵ���߳���
};


/*********************************************************************
* �߳�ִ�����ڶ���
*********************************************************************/
enum
{
#if defined(DEBUG) || defined(_DEBUG) || defined(_DEBUG_)
	enmThreadExecutePeriod			= 10000,
#else
	enmThreadExecutePeriod			= 10000,
#endif
};


/*********************************************************************
* �������Ӳ�������
*********************************************************************/
typedef int EConnectParamCatagory;
enum
{
	enmConnectParamCatagory_DianXin = 1, //����
	enmConnectParamCatagory_WangTong = 2, //��ͨ
};
enum{
	enmMaxConnectParamCatagoryCount = 2
};
typedef struct tagConnectParam
{
	char				szRemoteIP[enmMaxIPStringLength];
	char				szRemotePorts[enmMaxPortStringLength];
	bool				bSlave;
	EConnectParamCatagory				iCatagory;
	int				iPingMSec;	//��ֵ��ʾ��IP���ɴ�
} ConnectParam;


/*********************************************************************
* ��ȡ��һ���£�����ڴ��������·ݣ�
*********************************************************************/
#define GET_PREV_MONTH(month)	(((month) % 100) == 1) ? ((month) - 89) : ((month) - 1)



/*********************************************************************
* ��־������Ͷ���
*********************************************************************/

typedef uint8_t						LogLevel;			//��־����
enum
{
	enmLogLevel_Debug				= 0x00,				//������Ϣ
	enmLogLevel_Info				= 0x01,				//һ����־��Ϣ
	enmLogLevel_Notice				= 0x02,				//֪ͨ
	enmLogLevel_Warning				= 0x03,				//����
	enmLogLevel_Error				= 0x04,				//����
	enmLogLevel_Critical			= 0x05,				//���ش���
};

#define DECLARE_LOGLEVELLIST	\
	const char * arrLogLevel[] = {\
	"[Debug]   ",\
	"[Info]    ",\
	"[Notice]  ",\
	"[Warning] ",\
	"[Error]   ",\
	"[Critical]"}

//�ڴ�����С����
#if defined(DEBUG) || defined(_DEBUG) || defined(_DEBUG_)
enum
{
	enmMaxCSQueueSize				= 0x02000000/16,			//32M
	enmMaxCommandQueueSize			= 0x02000000/16,			//32M
	enmMaxReturnQueueSize			= 0x02000000/16,			//32M
	enmMaxLogQueueSize				= 0x02000000/16,			//32M

	enmMaxPlayerCount				= 0x00010000/16,			//64K
	enmMaxTimerCount				= 0x00010000/16,			//64K
	enmMaxSessionCount				= 0x00010000/16,			//64K
};
#else
enum
{
	enmMaxCSQueueSize				= 0x02000000,			//16M
	enmMaxCommandQueueSize			= 0x02000000,			//16M
	enmMaxReturnQueueSize			= 0x02000000,			//16M
	enmMaxLogQueueSize				= 0x02000000,			//16M

	enmMaxPlayerCount				= 0x00010000,			//32K
	enmMaxTimerCount				= 0x00010000,			//32K
	enmMaxSessionCount				= 0x00010000,			//32K
};
#endif


enum
{
	enmDBServiceGroupCount			= 1,					//���ݿ�ҵ��������
	enmDBIOThreadCountPerGroup		= 4,					//ÿ�����ݿ��д�߳�����
};


//Ͱ��С����
enum
{
	enmPlayerBucketSize				= (enmMaxPlayerCount * 256),		//��������б�Ͱ��С
	enmTimerBucketSize				= (enmMaxTimerCount * 256),			//���ж�ʱ���б�Ͱ��С
	enmSessionBucketSize				= (enmMaxSessionCount * 256),		//���лỰ�б�Ͱ��С
	enmMaxCommandSize					= 1024 * 256,		//256k
};

//�Ự���Ͷ���
enum
{
	enmSessionType_None				= 0x0000,		//��Ч����
	enmSessionType_CreateRole		= 0x0001,		//������ɫ�Ự
	enmSessionType_DeleteRole		= 0x0002,		//ɾ����ɫ�Ự
	enmSessionType_UpdateBaseInfo	= 0x0003,		//���½�ɫ������Ϣ�Ự
	enmSessionType_UpdateAsset		= 0x0004,		//���½�ɫ�ʲ��Ự
	enmSessionType_UpdateAvatar		= 0x0005,		//���½�ɫ����Ự
	enmSessionType_UpdateIDCard		= 0x0006,		//���½�ɫ���֤��Ϣ�Ự
	enmSessionType_UpdateLoginInfo	= 0x0007,		//���½�ɫ��������Ϣ�Ự
	enmSessionType_UpdatePlayerDBState	= 0x0008,	//���½�ɫ״̬�Ự
	enmSessionType_UpdatePunish		= 0x0009,		//���½�ɫ������Ϣ�Ự
	enmSessionType_UpdateLover		= 0x000A,		//���½�ɫ������Ϣ�Ự
	enmSessionType_UpdateAchievement	= 0x000B,		//���½�ɫ�ɾ����ݻỰ
	enmSessionType_UpdateGroup		= 0x000C,		//���½�ɫ������Ϣ�Ự
	enmSessionType_GetRoleInfo		= 0x0011,		//��ȡ��ɫ���ݻỰ
	enmSessionType_GetRoleGeneral	= 0x0012,		//��ȡ��ɫ��Ҫ��Ϣ�Ự
	enmSessionType_GetRolePublicInfo	= 0x0013,		//��ȡ��ɫ��Ҫ��Ϣ�Ự
	enmSessionType_UpdateAvatarItem	= 0x0014,		//����avatar��Ʒ��Ϣ
	enmSessionType_UpdateRoleRank	= 0x0015,		//���½�ɫ����ȼ�
};


/*********************************************************************
* ��־������Ͷ���
*********************************************************************/

typedef uint8_t						LogSource;		//��־
enum
{
	enmLogSource_Netio		= 0x00,			//ͨ���߳�
	enmLogSource_Main				= 0x01,			//��ҵ���߳�
	enmLogSource_DBAccess			= 0x02,			//���ݿ�����߳�
};


#endif /* COMMON_DEF_H_ */
