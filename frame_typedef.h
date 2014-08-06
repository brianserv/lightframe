/*
 * frame_typedef.h
 *
 *  Created on: 2011-11-22
 *      Author: jimm
 */

#ifndef FRAME_TYPEDEF_H_
#define FRAME_TYPEDEF_H_

#include "common/common_typedef.h"
#include "common/common_def.h"
#include "common/common_version.h"
#include "frame_namespace.h"
//
//typedef uint8_t SessionIndexLocType;					//�ỰID����Ϣ�д��λ��
//enum
//{
//	enumSessionIndexLocType_Opt			=1,			//�����opt��
//	enumSessionIndexLocType_Body			=2			//�����MsgBody
//};
//
//typedef uint8_t MessageEventType;					//��Ϣ�¼��ص���ʽ
//enum
//{
//	enumMessageEventType_Comp,						//�����м���ģʽ(buf,size)
//	enumMessageEventType_New,						//��ģʽ(IMsgBody)
//};

typedef uint8_t ConfigType;
enum {
	enmConfigType_XML,								//�����ļ���xml����
	enmConfigType_DB								//�����ļ���db����
};

typedef char SubConfigName[32];

enum
{
	enmMaxRowCountPreKey = 1024						//����key����ӵ�е�������
};

/*********************************************************************
* �������Ͷ���
*********************************************************************/

typedef int32_t						ErrorCode;			//������

typedef int32_t						UIN;				//���UIN

typedef int32_t						ServerID;			//������ID
typedef int32_t						RoleID;				//��ɫID

typedef int32_t						TimerIndex;			//��ʱ��������
typedef TimerIndex					TimerID;			//��ʱ��ID

typedef uint16_t					SessionType;			//�Ự����
typedef int32_t						SessionIndex;		//�Ự������

typedef int32_t						PlayerIndex;

typedef uint8_t						TimerType;			//��ʱ������

typedef uint16_t					ConnIndex;			//��������

//��������
typedef uint8_t							NetType;
enum
{
	enmNetType_Tel						= 0x00,			//����
	enmNetType_Cnc						= 0x01,			//��ͨ
	enmNetType_Count					= 0x02,
};


/*********************************************************************
* ����������ƶ���
*********************************************************************/

enum
{
	enmMaxOptionalDataLength			= 256,
	enmMaxZoneCount						= 16,			//����������
	enmMaxConnActiveTime				= 60,			//������������Ծʱ��
	enmMinReconnectTime					= 1,			//�����������
};

//����Ĭ�ϴ�����
enum
{
	enmDefault_ErrorCode  = -1,                  
};

/*********************************************************************
* ��ЧID����
*********************************************************************/

enum
{
	enmInvalidRoleID				= 0,				//��Ч��ɫID
	enmInvalidPlayerIndex 			= -1,
	enmInvalidTimerIndex			= -1,				//��Ч��ʱ������
	enmInvalidTimerID				= -1,				//��Ч��ʱ��ID
	enmInvalidSessionIndex			= -1,				//��Ч�ĻỰ����
	enmInvalidMessageID             = 0,                //��Ч����ϢID
	enmInvalidMessageType           = 0xFF,                //��Ч����Ϣ����
	enmInvalidEntityType            = 0,                //��Ч��ʵ������
	enmInvalidServerID             = -1,               //��Ч�ķ�����ID
	enmInvalidSequence             = 0,                 //��Ч�����к�
	enmInvalidOptionLen            = -1,                //��Ч�ĸ����ֶγ���   
	enmInvalidLevel                = -1,                //��Ч�ĵȼ�ֵ
	enmInvalidExpVal               = -1,                //��Ч�ľ���ֵ
	//enmInvalidItemID               = -1,                  //��Ч����ƷID
	enmInvalidPriceVal             = -1,                 //��Ч�ļ۸�
	enmInvalidBenefitVal           = -1,                  //��Ч������ֵ
	enmInvalidItemCount            = -1,                   //��Ч����Ʒ����
	enmInvalidTimeVal              = -1,                   //��Ч��ʱ��ֵ
	enmInvalidVersion              = 0,                    //��Ч�İ汾��
	enmInvalid179ID                = 0,                    //��Ч��179�˺�
	enmInvalidVipLevel             = 0,                    //��Ч��vip�ȼ�
	enmInvalidOnlineTime           = 0,                    //��Ч������ʱ��
	enmInvalidLeftTime             = 0,                    //��Ч�ı���ʣ��ʱ��
	enmInvalidLeftMoney            = 0,                    //��Ч��ʣ���ʲ�
};

//����ʱ���Լ�keep alive���ʱ�䶨��
#define INVALID_RECONNECT_TIME  (-1)
#define RECONNECT_SPAN			(3)
#define KEEP_ALIVE_SPAN			(60)

enum
{
	enmMaxConnectingTime = 2,//2s
};


typedef uint8_t						Gender;				//����Ա�
enum
{
	enmGender_Unknown				= 0xff,				//δ֪�Ա�
	enmGender_Female				= 0x00,				//Ů��
	enmGender_Male					= 0x01,				//����
	enmGender_Unisex				= 0x02,				//��Ůͨ��
};

typedef enum tagThreadType
{
	enmThreadType_NetioThread			= 0,
	enmThreadType_MainThread			= 1,
	enmThreadType_DBAccessThread		= 2,
} ThreadType;

typedef uint8_t						EntityType;			//ʵ������

/*********************************************************************
* ��ʱ��������ݽṹ����
*********************************************************************/

//��ʱ����Ϣ
typedef struct tagTimer
{
	TimerType				timerType;					//��ʱ������
	int32_t					nTimerID;					//��ʱ����ʶ
	void					*pData;						//��ʱ������������
	int64_t					nStartTime;					//��ʱ����ʼʱ��
	int64_t					nCycleTime;					//��ʱ������
	int64_t					nEndTime;					//��ʱ������ʱ��
	bool					bLoop;						//�Ƿ�ѭ������
	uint32_t				nFiredCount;				//�Ѵ�������
	uint32_t                attachedDataA;               //��������
	uint32_t                attachedDataB;               //��������
} Timer;

/*********************************************************************
* �ַ������ȶ���
*********************************************************************/

enum
{
	enmMaxAccountIDLength			= 32,				//�û��ʺ���󳤶�
	enmMaxUrlLength					= 256,				//����������󳤶�

	/**
	����Length���ַ���UTF���뻺��������󳤶�
	����UnicodeLength��ָ�ؼ��е����ֳ���
	����RealLength��ָ�����ַ������Ƶ��ֽ�����
	һ��Ӣ����ĸ1���ֽڣ�һ�����������ֽ�
	*/
	enmMaxRoleNameUnicodeLength = 10, //��ɫ����Unicode��ʽ����󳤶�
	enmMaxRoleNameRealLength = 12,  //��ɫ���Ƶ��ֽ�����

	enmMaxRoleTitleLength			= 32,				//��ɫ������󳤶�
	enmMaxRoleTitleUnicodeLength = 10, //��ɫ����Unicode��ʽ����󳤶�
	enmMaxRoleTitleRealLength = 12,  //��ɫ���Ƶ��ֽ�����

	enmMaxGroupNameLength			= 32,				//����������󳤶�
	enmMaxGroupNameUnicodeLength			= 10,				//��������Unicode��ʽ��󳤶�
	enmMaxGroupNameRealLength = 20, //���������ֽ�����

	enmMaxFamilyNameLength			= 32,				//����������󳤶�
	enmMaxFamilyTitleLength			= 32,				//����ͷ����󳤶�
	enmMaxFamilyPasswordLength		= 32,				//�������������󳤶�
	enmMaxCustomRankLength			= 512,				//����ְλ�б�
	enmMaxFamilyNameUnicodeLength			= 10,				//��������Unicode��ʽ��󳤶�
	enmMaxFamilyNameRealLength = 20, //���������ֽ�����
	enmMaxFamilyIconFontLength 		= 16,				//����������ֵ���󳤶�

	enmMaxReservedStringLength		= 32,				//�����ַ�����󳤶�

	enmMaxStageNameLength			= 64,				//��̨������󳤶�
	enmMaxStageNameUnicodeLength			= 10,				//��̨����Unicode��ʽ��󳤶�
	enmMaxStageNameRealLength = 20, //��̨�����ֽ�����

	enmMaxStagePasswordLength		= 32, 				//��̨������󳤶�
	enmMaxPasswordRealLength		= 6, 				//��̨������󳤶�

	enmMaxRoomNameLength			= 128,				//����������󳤶�
	enmMaxRoomNameUnicodeLength = 10,       //��������Unicode��ʽ��󳤶�

	enmMaxRoleNameLength			= 32,				//��ɫ������󳤶�
	enmMaxDescriptionLength			= 256,				//������Ϣ��󳤶�

	enmMaxDescriptionUnicodeLength			= 64,				//������ϢUnicode��ʽ��󳤶�

	enmMaxFamilyNoticeLength			= 256,				//���幫����󳤶�
	enmMaxFamilyNoticeUnicodeLength			= 64,				//���幫��Unicode��ʽ��󳤶�

	enmMaxGoodsNameLength			= 256,				//��Ʒ������󳤶�
	enmMaxGoodsNameUnicodeLength			= 64,				//��Ʒ����Unicode��ʽ��󳤶�

	enmMaxIconNameLength			= 256,				//��Ʒͼ��������󳤶�
	enmMaxIconNameUnicodeLength			= 64,				//��Ʒͼ������Unicode��ʽ��󳤶�

	enmMaxItemNameLength			= 32,				//��Ʒ������󳤶�
	enmMaxItemNameUnicodeLength			= 10,				//��Ʒ����Unicode��ʽ��󳤶�

	enmMaxMusicNameLength			= 32,				//����������󳤶�
	enmMaxMusicNameUnicodeLength			= 10,				//��������Unicode��ʽ��󳤶�

	enmMaxSingerNameLength			= 32,				//����������󳤶�
	enmMaxSingerNameUnicodeLength			= 10,				//��������Unicode��ʽ��󳤶�

	enmMaxLanguageLength			= 32,				//������󳤶�
	enmMaxLanguageUnicodeLength			= 10,				//����Unicode��ʽ��󳤶�

	enmMaxTerritoryLength			= 32,				//������󳤶�
	enmMaxTerritoryUnicodeLength			= 10,				//����Unicode��ʽ��󳤶�

	enmMaxCompanyLength				= 64,				//��˾��󳤶�
	enmMaxCompanyUnicodeLength				= 20,				//��˾Unicode��ʽ��󳤶�

	enmMaxSpecialNameLength			= 32,				//ר��������󳤶�
	enmMaxSpecialNameUnicodeLength			= 10,				//ר������Unicode��ʽ��󳤶�

	enmMaxSceneNameLength			= 32,				//����������󳤶�
	enmMaxSceneNameUnicodeLength			= 10,				//��������Unicode��ʽ��󳤶�

	enmMaxSqlStringLength			= 32768,				//SQL�����󳤶�

	enmMaxMailSubjectLength		= 256,					//�ʼ��������󳤶�(utf8��ʽ)
	enmMaxMailSubjectUnicodeLength = 10,            //�ʼ�����Unicode��ʽ����󳤶�
	enmMaxMailSubjectRealLength = 20,					//�ʼ������ֽ�����

	enmMaxMailContentLength			= 1024,				//�ʼ����ݵ���󳤶�
	enmMaxMailContentUnicodeLength = 200,        //�ʼ�����Unicode��ʽ����󳤶�
	enmMaxMailContentRealLength = 400,        //�ʼ������ֽ�����

	enmMaxAddFriendInteractiveMsgLength	= 256,			//����Ӻ��ѽ����и�������Ϣ�ĳ���
	enmMaxAddFriendInteractiveMsgUnicodeLength = 30, //����Ӻ��ѽ����и�������Ϣ��Unicode���볤��
	enmMaxAddFriendInteractiveMsgRealLength = 60,	//����Ӻ��ѽ����и�������Ϣ���ֽ�����

	enmMaxChannelNameLength			= 32,

	enmMaxTitleNameLength			= 32,				//���ͷ����󳤶�
	enmMaxTitleNameUnicodeLength = 10,		//���ͷ��Unicode�������󳤶�

	enmMaxBigspkerLength			= 512,				//�����ȵ����ݳ���
	enmMaxBigspkerUncodeLength = 30,             //�����ȵ����ݵ�Unicode���볤��
	enmMaxBigspkerRealLength = 60,             //�����ȵ����ݵ��ֽ�����

	enmMaxSmallSpeakerLength		= 256,				//С���ȵ����ݳ���
	enmMaxSmallSpeakerUnicodeLength = 30, //С���ȵ����ݵ�Unicode��ʽ�ı��볤��
	enmMaxSmallSpeakerRealLength = 60, //С���ȵ����ݵ��ֽ�����

	enmMaxStyleLength				= 256,		//���������ʽ����
	enmMaxStyleRealLength			= 32,		//���������ʽʵ�ʳ���

	enmMaxChatMessageLength			= 512,				//�������ʱ�ܷ�������ֽ���
	enmMaxChatMessageUnicodeLength = 50,		//���������Ϣ��Unicode��ʽ����󳤶�
	enmMaxChatMessageRealLength = 100,		//���������Ϣ���ֽ�����

	enmMaxSendMessageLength			= 60*3,	//������Ϣ����

	enmSettingBufferLength			= 1024, //�ͻ���˽��buffer����

	enmMaxErrorMessageLength = 512,	//���ظ��ͻ��˵Ĵ�����Ϣ����
	enmMaxErrorMessageUnicodeLength = 128,
	enmMaxErrorMessageRealLength = 256,
	enmMaxNotifyMessageLength		= 512,
	enmMaxNotifyParamLength			= 32,

	enmMaxQuestionStringLength		= 128,
	enmMaxClueStringLength			= 256,
	enmMaxOptionStringLength		= 32,
	enmMaxMusicInfoLength			= 1024,
	enmMaxPersonalSignatureLength	= 128, //����ǩ����󳤶�(�ֽ�)
	enmMaxLogPrefixLength			= 128,
	enmMaxLogPrefixCount			= 5,
	enmMaxActionParaLength			= 256, //ue
	enmMaxActionResultDesc			= 128,
	enmMaxEchoStringLength			= 1024,
	enmMaxShowMessageLength			= 256,
	enmMaxItemPeriodCount			= 256,//���ļ������ţ���ʱʹ�õ�������

	enmMaxDescStringLength			= 256,		//���������Ϣ����
	enmMaxUpdateAssetDescLength		= 128,	//�����ʲ�����������󳤶�

	enmMaxRelativesNameLength		=32,
	enmMaxRelativesGroupNameLength	=32,
	enmMaxSpeakerPostfixLength		=32,

	enmMaxNoticeLength				= 256,	//֪ͨ�ַ�������
	enmPrizeItemNum					= 16,	//�������
	enmNewPrizeLevelNum				= 20,	//�����������
	enmAllPrizeItemNum				= enmPrizeItemNum*enmNewPrizeLevelNum,	//ÿ��������ܸ���
	enmMaxItemClassCount            = 128,   //������ӵ�е���Ʒ��������
	enmMaxPunishReasonLength        = 512,   //���ͷ�ԭ�򳤶�
};

/*********************************************************************
* ������������ݽṹ����
*********************************************************************/


#define MaxOptionDataSize			(1 * 1024)

#endif /* FRAME_TYPEDEF_H_ */
