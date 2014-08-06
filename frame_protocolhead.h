/*
 * frame_protocolhead.h
 *
 *  Created on: 2011-11-23
 *      Author: jimm
 */

#ifndef FRAME_PROTOCOLHEAD_H_
#define FRAME_PROTOCOLHEAD_H_

#include "common/common_typedef.h"
#include "common/common_codeengine.h"
#include "common/common_string.h"
#include "frame_namespace.h"
#include "frame_typedef.h"
#include "frame_msg_impl.h"
#include <string.h>
#include <stdio.h>

FRAME_NAMESPACE_BEGIN

typedef uint8_t						MessageType;		//��Ϣ����
enum
{
	enmMessageType_None				= 0x00,				//��Чֵ
	enmMessageType_Request			= 0x01,				//����
	enmMessageType_Response			= 0x02,				//��Ӧ
	enmMessageType_Notify			= 0x03,				//֪ͨ
	enmMessageType_Other			= 0x04,				//����
};

typedef uint8_t						TransType;			//��������
enum
{
	enmTransType_None				= 0xFF,				//��Чֵ
	enmTransType_P2P				= 0,				//��Ե�
	enmTransType_P2G				= 1,				//�鲥
	enmTransType_Broadcast			= 2,   				//�㲥
	enmTransType_ByKey				= 3,				//���ݹؼ���ת��
	enmTransType_Regist				= 4,				//Serverע��
	enmTransType_Ramdom				= 5,				//��ָ����Ŀ��servertype�����ѡ��һ������
	enmTransType_Disconnect         = 6,				//server���ӶϿ�
};

enum
{
	enmMaxOptionalCount				= 128,				//����ѡ�ֶδ�С
	enmMaxDestinationCout			= 126,				//�鲥ʱ���Ŀ�ĵ�����
};

enum
{
	enmMinCSHeadMessageLength		= 28,			//CS��Ϣͷ����С����
};

/*********************************************************************
* ͨ��Э��������ݽṹ����
*********************************************************************/

struct ControlInfo
{
	uint8_t					nControlType;		//�������ͣ�
	uint32_t				nControlParam;		//���ݿ������;�������ĺ���
};

struct ClientInfo
{
	uint32_t				nClientAddress;
	uint16_t				nClientPort;
};

//Connector��Ϣͷ
class ConnHead// : public IMsgHead
{
public:
	RoleID					roleID;
	ConnIndex				connIndex;
	ControlInfo				controlInfo;		//������Ϣ����ʾ�������ʾ�Ĳ���
	ClientInfo				clientInfo;			//�ͻ��˵���Ϣ

	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
	{
		int32_t ret = S_OK;

		ret = CCodeEngine::Encode(buf, size, offset, roleID);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, connIndex);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, controlInfo.nControlType);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, controlInfo.nControlParam);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, clientInfo.nClientAddress);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, clientInfo.nClientPort);
		if (0 > ret)
		{
			return ret;
		}

		return S_OK;
	}

	int32_t MessageDecode(const uint8_t* buf, const uint32_t size, uint32_t& offset)
	{
		int32_t ret = S_OK;

		ret = CCodeEngine::Decode(buf, size, offset, roleID);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, connIndex);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, controlInfo.nControlType);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, controlInfo.nControlParam);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, clientInfo.nClientAddress);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, clientInfo.nClientPort);
		if (0 > ret)
		{
			return ret;
		}

		return S_OK;
	}

	void Dump(char* buf, const uint32_t size, uint32_t& offset)
	{
	    sprintf(buf + offset, "{roleID=%d, connIndex=%d, ControlInfo={nControlType=%d, nControlParam=%d, "
	    		"nClientAddress=%u, nClientPort=%d}}", roleID, connIndex, controlInfo.nControlType,
	    		controlInfo.nControlParam, clientInfo.nClientAddress, clientInfo.nClientPort);
	    offset = (uint32_t)strlen(buf);
	}
};

typedef ConnHead WebControlHead;


//CS��Ϣͷ��
class MessageHeadCS
{
public:
	uint16_t				nTotalSize;
	uint16_t				nVersion;
	uint32_t				nMessageID;
	RoleID					nRoleID;
	uint32_t				nRoomID;
	uint32_t				nSequence;
	uint32_t				nOption;

	MessageHeadCS()
	{
		nTotalSize = 0;
		nVersion = 0;
		nMessageID = 0;
		nRoleID = enmInvalidRoleID;
		nRoomID = 0;
		nSequence = 0;
		nOption = 0;
	}

	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
	{
		int32_t ret = S_OK;

		ret = CCodeEngine::Encode(buf, size, offset, nTotalSize);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, nVersion);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, nMessageID);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, nRoleID);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, nRoomID);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, nSequence);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, nOption);
		if (0 > ret)
		{
			return ret;
		}

		return ret;
	}

	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
	{
		int32_t ret = S_OK;

		ret = CCodeEngine::Decode(buf, size, offset, nTotalSize);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, nVersion);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, nMessageID);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, nRoleID);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, nRoomID);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, nSequence);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, nOption);
		if (0 > ret)
		{
			return ret;
		}

		return ret;
	}

	void Dump(char* buf, const uint32_t size, uint32_t& offset)
	{
	    sprintf(buf + offset, "{nVersion=%d, nMessageID=0x%08x, nRoleID=%d, "
	    		"nRoomID=%u, nSequence=%u, nOption=%u",
	    		nVersion, nMessageID, nRoleID, nRoomID, nSequence, nOption);
	    offset = (uint32_t)strlen(buf);
	}
};

//SS��Ϣͷ����С
#define MESSAGEHEADSS_SIZE	44

//SS��Ϣͷ��
class MessageHeadSS// : public IMsgHead
{
public:
	uint32_t				nTotalSize;			//���ֵ�ڿ���ڲ���䣬Ӧ�ò㹹���ͷʱֱ����0����
	uint32_t				nMessageID;
	uint16_t				nHeadSize;			//���ֵ�ڿ���ڲ���䣬Ӧ�ò㹹���ͷʱֱ����0����
	MessageType				nMessageType;      //uint8_t
	EntityType				nSourceType;       //uint8_t
	EntityType				nDestType;         //uint8_t
	int32_t					nSourceID;
	int32_t					nDestID;
	RoleID					nRoleID;            //int32_t
	uint32_t				nSequence;
	TransType				nTransType;				//��������, uint8_t
	SessionIndex			nSessionIndex;			//�Ự����, int32_t
	uint32_t                nRoomID;
	int32_t					nZoneID;
	uint16_t				nRouterIndex;

	MessageHeadSS()
	{
		nTotalSize  = 0;
		nHeadSize	= 0;
		nMessageID  = 0;
		nMessageType = enmMessageType_None;
		nSourceType  = 0;
		nDestType	= 0;
		nSourceID	= enmInvalidServerID;
		nDestID		= enmInvalidServerID;
		nRoleID		= enmInvalidRoleID;
		nSequence	= 0;
		nTransType	= enmTransType_None;
		nSessionIndex= enmInvalidSessionIndex;
		nRoomID		= 0;
		nZoneID		= 0;
		nRouterIndex = 0;
	}

	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
	{
		int32_t ret = S_OK;

		ret = CCodeEngine::Encode(buf, size, offset, nTotalSize);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, nMessageID);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, nHeadSize);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, nMessageType);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, nSourceType);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, nDestType);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, nSourceID);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, nDestID);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, nRoleID);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, nSequence);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, nTransType);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, nSessionIndex);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, nRoomID);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, nZoneID);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, nRouterIndex);
		if (0 > ret)
		{
			return ret;
		}

		return S_OK;
	}

	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
	{
		int32_t ret = S_OK;

		ret = CCodeEngine::Decode(buf, size, offset, nTotalSize);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, nMessageID);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, nHeadSize);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, nMessageType);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, nSourceType);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, nDestType);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, nSourceID);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, nDestID);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, nRoleID);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, nSequence);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, nTransType);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, nSessionIndex);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, nRoomID);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, nZoneID);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, nRouterIndex);
		if (0 > ret)
		{
			return ret;
		}

		return S_OK;
	}

	void Dump(char* buf, const uint32_t size, uint32_t& offset)
	{
	    sprintf(buf + offset, "{nMessageID=0x%08x, nMessageType=%d, nSourceType=%d, nDestType=%d,"
	    		"nSourceID=%d, nDestID=%d, nRoleID=%d, nSequence=%d, nTransType=%d, nSessionIndex=%d, "
	    		"nRoomID=%d, nZoneID=%d, nRouterIndex=%d}",
	    		nMessageID, nMessageType, nSourceType, nDestType, nSourceID, nDestID, nRoleID,
	    		nSequence, nTransType, nSessionIndex, nRoomID, nZoneID, nRouterIndex);
	    offset = (uint32_t)strlen(buf);
	}
};

class ConnUin// : public IMsgHead
{
public:
	uint8_t					nVersion;				//�汾
	uint8_t					nServerID;				//tunnel server��id
	uint16_t				nTunnelIndex;			//tunnel server���ڲ�����
	RoleID					nRoleID;				//�û�id
	int32_t					nCreateTime;			//���Ӵ���ʱ��
	uint32_t				nIsn;					//���к�,��tunnel server����
	uint32_t				nAddress;				//ip
	uint16_t				nPort;					//�˿�
	NetType					nNetType;				//��ʾ�ͻ��˵���������
	uint8_t					nAccountType;			//�˺�����
	uint32_t				nRouterAddress;			//tunnel������router��ַ
	uint16_t				nRouterPort;			//tunnel������router�˿�
	uint8_t					nErrorCode;				//������

	ConnUin()
	{
		Reset();
	}

	void Reset()
	{
		nVersion		= 0;
		nServerID		= 0;
		nTunnelIndex	= 0;
		nRoleID			= enmInvalidRoleID;
		nCreateTime		= 0;
		nIsn			= 0;
		nAddress		= 0;
		nPort			= 0;
		nNetType		= enmNetType_Tel;
		nAccountType	= 0;
		nRouterAddress	= 0;
		nRouterPort		= 0;
		nErrorCode		= 0;
	}

	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
	{
		int32_t ret = S_OK;

		ret = CCodeEngine::Encode(buf, size, offset, nVersion);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, nServerID);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, nTunnelIndex);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, nRoleID);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, nCreateTime);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, nIsn);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, nAddress);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, nPort);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, nNetType);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, nAccountType);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, nRouterAddress);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Encode(buf, size, offset, nRouterPort);
		if (0 > ret)
		{
			return ret;
		}
//		ret = CCodeEngine::Encode(buf, size, offset, nErrorCode);
//		if (0 > ret)
//		{
//			return ret;
//		}


		return S_OK;
	}

	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
	{
		int32_t ret = S_OK;

		ret = CCodeEngine::Decode(buf, size, offset, nVersion);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, nServerID);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, nTunnelIndex);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, nRoleID);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, nCreateTime);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, nIsn);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, nAddress);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, nPort);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, nNetType);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, nAccountType);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, nRouterAddress);
		if (0 > ret)
		{
			return ret;
		}
		ret = CCodeEngine::Decode(buf, size, offset, nRouterPort);
		if (0 > ret)
		{
			return ret;
		}
//		ret = CCodeEngine::Decode(buf, size, offset, nErrorCode);
//		if (0 > ret)
//		{
//			return ret;
//		}

		return S_OK;
	}

	void Dump(char* buf, const uint32_t size, uint32_t& offset)
	{
	    sprintf(buf + offset, "{nVersion=%d, nServerID=%d, nTunnelIndex=%d, nRoleID=%d, "
	    		"nCreateTime=%d, nIsn=%d, nAddress=%u, nPort=%d, nNetType=%d, nAccountType=%d, "
	    		"nRouterAddress=%d, nRouterPort=%d, nErrorCode=%d}",
	    		nVersion, nServerID, nTunnelIndex, nRoleID, nCreateTime, nIsn, nAddress,
	    		nPort, nNetType, nAccountType, nRouterAddress, nRouterPort, nErrorCode);
	    offset = (uint32_t)strlen(buf);
	}

	bool Compare(ConnUin *pConnUin)
	{
		if(pConnUin == NULL)
		{
			return false;
		}

		return (pConnUin->nAddress == nAddress)&& (pConnUin->nPort == nPort) && (pConnUin->nRoleID == nRoleID)
				&& (pConnUin->nCreateTime == nCreateTime) && (pConnUin->nTunnelIndex == nTunnelIndex);
	}
};

typedef ConnUin		ConnInfo;

#define MAX_CS_HEAD_OPTION_LENGTH 128
//�ͻ���/������ͨ��Э��ͷ
typedef struct tagCSHead
{
	int32_t nPackageLength;							//ͷ������ + Body����
	int32_t roleID;
	int16_t nFlag;
	int16_t nOptionalLen;							//signature ���ܴ�
	char arrOptional[MAX_CS_HEAD_OPTION_LENGTH];
	int16_t nHeadLength;
	int16_t nMessageID;
	int16_t nMessageType;
	int16_t nVersion;
	int32_t nPlayerID;
	int32_t nSequence;	//sequence ʹ�ü��ܴ���
}CSHead;

//��ROUTER�Ͽ���֪ͨ
#define CLOSE_NOTIFY_EVENT		0xFFFFFFFF


class NetPacket
{
public:
	NetPacket()
	{
		m_nNetPacketLen = 0;
	}

	virtual ~NetPacket()
	{
	}

	int32_t 	m_nNetPacketLen;	//����������4�ֽڵĳ���
	uint8_t		m_pNetPacket[0];
};

FRAME_NAMESPACE_END

#endif /* FRAME_PROTOCOLHEAD_H_ */
