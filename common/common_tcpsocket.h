/*
 * common_tcpsocket.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_TCPSOCKET_H_
#define COMMON_TCPSOCKET_H_


#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#include "common_typedef.h"
#include "common_def.h"


#ifdef WIN32
typedef SOCKET		SocketFD;
enum
{
	enmInvalidSocketFD		= INVALID_SOCKET,		//��Ч�׽���
};
#else
typedef int32_t			SocketFD;
enum
{
	enmInvalidSocketFD		= -1,					//��Ч�׽���
};
#endif

//�׽�������
typedef enum enmSocketType
{
	enmSocketType_Listen				= 0,		//�����׽���
	enmSocketType_Communicate			= 1,		//ͨ���׽���
} SocketType;

//�׽�������
typedef enum enmSocketAttr
{
	enmSocketAttr_Unknown				= 0,		//δ֪�׽���
	enmSocketAttr_Outside				= 1,		//�ⲿ�׽���
	enmSocketAttr_Inside				= 2,		//�ڲ��׽���
} SocketAttr;

//�׽�������״̬
typedef enum enmSocketStatus
{
	enmSocketStatus_Closed				= 0,		//
	enmSocketStatus_Opened				= 1,
	enmSocketStatus_Connecting			= 2,
	enmSocketStatus_Connected			= 3,
	enmSocketStatus_Error				= 4,
	enmSocketStatus_Available			= 5,
} SocketStatus;

enum
{
	enmRecvBufferSize				= 0x00040000,		//���ջ�������С
	enmSendBufferSize				= 0x00040000,		//���ͻ�������С
};


enum
{
	enmMaxConnectCount					= 500,			//����������
	enmDefaultConnectionTimeout			= 1000,			//Ĭ�����ӳ�ʱʱ��
};


class CTCPSocket
{
public:
	CTCPSocket();
	virtual ~CTCPSocket();

private:
	//��ֹ�������캯���͸�ֵ������
	CTCPSocket(const CTCPSocket& soc);
	CTCPSocket& operator = (CTCPSocket& soc);

public:
	//��ȡ�׽���������
	SocketFD GetFD() const;
	//��ȡ�׽���״̬
	SocketStatus GetStatus() const;

	//�򿪷������׽���
	int32_t OpenAsServer(uint16_t nPort, const char* szLocalIP = NULL, bool bNonBlock = true);
	//�򿪿ͻ����׽���
	int32_t OpenAsClient(const char* szLocalIP = NULL);
	//�ر��׽���
	void CloseSocket();
	//�������׽���
	int32_t Attach(SocketFD socketFD);

	//�����˽�������
	int32_t Connect_With_Timeout(const char* szRemoteIP, uint16_t nPort, uint16_t secs, bool bNonBlock = true);

	//�����˽�������
	int32_t Connect(const char* szRemoteIP, uint16_t nPort, bool bNonBlock = true, int32_t nTimeout = 0);

	//�����˽�������
	int32_t Connect(uint32_t nRemoteIP, uint16_t nPort, bool bNonBlock = true, int32_t nTimeout = 0);
	//�Ͽ�����
	int32_t Disconnect();

	//
	int32_t Accept(SocketFD& socketFD);

	//��������
	int32_t ReceiveData(uint8_t *pBuffer, uint32_t nSize, uint32_t& nReceived);
	//��������
	int32_t SendData(const uint8_t *pBuffer, const uint32_t nLength, uint32_t& nSent);

protected:
	SocketType			m_socketType;
	SocketFD			m_socketFD;
	SocketStatus		m_socketStatus;
};

#endif /* COMMON_TCPSOCKET_H_ */
