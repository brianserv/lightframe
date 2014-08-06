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

typedef uint8_t						MessageType;		//消息类型
enum
{
	enmMessageType_None				= 0x00,				//无效值
	enmMessageType_Request			= 0x01,				//请求
	enmMessageType_Response			= 0x02,				//响应
	enmMessageType_Notify			= 0x03,				//通知
	enmMessageType_Other			= 0x04,				//其他
};

typedef uint8_t						TransType;			//传输类型
enum
{
	enmTransType_None				= 0xFF,				//无效值
	enmTransType_P2P				= 0,				//点对点
	enmTransType_P2G				= 1,				//组播
	enmTransType_Broadcast			= 2,   				//广播
	enmTransType_ByKey				= 3,				//根据关键字转发
	enmTransType_Regist				= 4,				//Server注册
	enmTransType_Ramdom				= 5,				//在指定的目的servertype中随机选择一个发送
	enmTransType_Disconnect         = 6,				//server链接断开
};

enum
{
	enmMaxOptionalCount				= 128,				//最大可选字段大小
	enmMaxDestinationCout			= 126,				//组播时最大目的地数量
};

enum
{
	enmMinCSHeadMessageLength		= 28,			//CS消息头部最小长度
};

/*********************************************************************
* 通信协议相关数据结构定义
*********************************************************************/

struct ControlInfo
{
	uint8_t					nControlType;		//控制类型，
	uint32_t				nControlParam;		//根据控制类型决定代表的含义
};

struct ClientInfo
{
	uint32_t				nClientAddress;
	uint16_t				nClientPort;
};

//Connector消息头
class ConnHead// : public IMsgHead
{
public:
	RoleID					roleID;
	ConnIndex				connIndex;
	ControlInfo				controlInfo;		//控制信息，表示这个包表示的操作
	ClientInfo				clientInfo;			//客户端的信息

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


//CS消息头部
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

//SS消息头部大小
#define MESSAGEHEADSS_SIZE	44

//SS消息头部
class MessageHeadSS// : public IMsgHead
{
public:
	uint32_t				nTotalSize;			//这个值在框架内部填充，应用层构造包头时直接填0即可
	uint32_t				nMessageID;
	uint16_t				nHeadSize;			//这个值在框架内部填充，应用层构造包头时直接填0即可
	MessageType				nMessageType;      //uint8_t
	EntityType				nSourceType;       //uint8_t
	EntityType				nDestType;         //uint8_t
	int32_t					nSourceID;
	int32_t					nDestID;
	RoleID					nRoleID;            //int32_t
	uint32_t				nSequence;
	TransType				nTransType;				//传输类型, uint8_t
	SessionIndex			nSessionIndex;			//会话索引, int32_t
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
	uint8_t					nVersion;				//版本
	uint8_t					nServerID;				//tunnel server的id
	uint16_t				nTunnelIndex;			//tunnel server的内部索引
	RoleID					nRoleID;				//用户id
	int32_t					nCreateTime;			//连接创建时间
	uint32_t				nIsn;					//序列号,由tunnel server分配
	uint32_t				nAddress;				//ip
	uint16_t				nPort;					//端口
	NetType					nNetType;				//标示客户端的网络类型
	uint8_t					nAccountType;			//账号类型
	uint32_t				nRouterAddress;			//tunnel所连的router地址
	uint16_t				nRouterPort;			//tunnel所连的router端口
	uint8_t					nErrorCode;				//错误码

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
//客户端/服务器通信协议头
typedef struct tagCSHead
{
	int32_t nPackageLength;							//头部长度 + Body长度
	int32_t roleID;
	int16_t nFlag;
	int16_t nOptionalLen;							//signature 加密串
	char arrOptional[MAX_CS_HEAD_OPTION_LENGTH];
	int16_t nHeadLength;
	int16_t nMessageID;
	int16_t nMessageType;
	int16_t nVersion;
	int32_t nPlayerID;
	int32_t nSequence;	//sequence 使用加密传输
}CSHead;

//与ROUTER断开的通知
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

	int32_t 	m_nNetPacketLen;	//不包含自身4字节的长度
	uint8_t		m_pNetPacket[0];
};

FRAME_NAMESPACE_END

#endif /* FRAME_PROTOCOLHEAD_H_ */
