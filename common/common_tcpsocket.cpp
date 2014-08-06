/***************************************************************************************************
*	file name:	common_tcpsocket.cpp
*	created:	2008-11-29
*	author:		teras xiao
*	version:	1.0
*	purpose:	��TCPЭ���Socket��װ��Ķ���
*
*	revision:
*
***************************************************************************************************/


#include <errno.h>
#include <string.h>

#include "common/common_tcpsocket.h"
#include "common/common_errordef.h"
#include "common/common_socket.h"


CTCPSocket::CTCPSocket()
{
	m_socketType = enmSocketType_Listen;
	m_socketFD = enmInvalidSocketFD;
	m_socketStatus = enmSocketStatus_Closed;
}

CTCPSocket::~CTCPSocket()
{
	if (enmInvalidSocketFD != m_socketFD)
	{
		CloseSocket();
	}
}

//��ȡ�׽���������
SocketFD CTCPSocket::GetFD() const
{
	return m_socketFD;
}

//��ȡ�׽���״̬
SocketStatus CTCPSocket::GetStatus() const
{
	return m_socketStatus;
}

//�򿪷������׽���
int32_t CTCPSocket::OpenAsServer(uint16_t nPort, const char* szLocalIP/* = NULL*/, bool nNonBlock/*= true*/)
{
	//��socket�����Ѵ������ȹر��׽���
	if (enmInvalidSocketFD != m_socketFD)
	{
		CloseSocket();
	}

#ifdef WIN32
	//����socket
	m_socketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (enmInvalidSocketFD == m_socketFD)
	{
		return E_SOCKETCREATE;
	}

	//�����׽�������
	int32_t op = 1;
	int32_t ret = setsockopt(m_socketFD, SOL_SOCKET, SO_REUSEADDR, (const char*)&op, sizeof(op));
	if (0 != ret)
	{
		CloseSocket();
		return E_SOCKETOPTION;
	}
#else
	//���׽���
	m_socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (enmInvalidSocketFD == m_socketFD)
	{
		return E_SOCKETCREATE;
	}

	//�����׽�������
	int32_t op = 1;
	int32_t ret = setsockopt(m_socketFD, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op));
	if (0 != ret)
	{
		CloseSocket();
		return E_SOCKETOPTION;
	}
#endif

	//����������ַ&�˿���Ϣ
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ((NULL != szLocalIP) ? inet_addr(szLocalIP) : htonl(INADDR_ANY));
	addr.sin_port = htons(nPort);

	ret = bind(m_socketFD, (struct sockaddr *)&addr, sizeof(addr));
	if (0 != ret)
	{
		CloseSocket();
		return E_SOCKETBIND;
	}

	//��ʼ����
	ret = listen(m_socketFD, SOMAXCONN);
	if (0 != ret)
	{
		CloseSocket();
		return E_SOCKETLISTEN;
	}

#ifndef WIN32
	//����Ϊ������
	if(nNonBlock)
	{
		set_non_block(m_socketFD);
	}

#endif


	//�����׽������ͺ�״̬
	m_socketType = enmSocketType_Listen;
	m_socketStatus = enmSocketStatus_Opened;

	return S_OK;

}

//�򿪿ͻ����׽���
int32_t CTCPSocket::OpenAsClient(const char* szLocalIP/* = NULL*/)
{
	//��socket�����Ѵ������ȹر��׽���
	if (enmInvalidSocketFD != m_socketFD)
	{
		CloseSocket();
	}

#ifdef WIN32
	//����socket
	m_socketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (enmInvalidSocketFD == m_socketFD)
	{
		return E_SOCKETCREATE;
	}
#else
	//���׽���
	m_socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (enmInvalidSocketFD == m_socketFD)
	{
		return E_SOCKETCREATE;
	}
#endif

	if (NULL != szLocalIP)
	{
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));

		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr(szLocalIP);
		int32_t ret = bind(m_socketFD, (struct sockaddr *)&addr, sizeof(addr));
		if (0 != ret)
		{
			CloseSocket();
			return E_SOCKETBIND;
		}

	}

	//�����׽������ͺ�״̬
	m_socketType = enmSocketType_Communicate;
	m_socketStatus = enmSocketStatus_Opened;

	return S_OK;
}

//�ر��׽���
void CTCPSocket::CloseSocket()
{
	if (enmInvalidSocketFD != m_socketFD)
	{
#ifdef WIN32
		closesocket(m_socketFD);
#else
		close(m_socketFD);
#endif
		m_socketFD = enmInvalidSocketFD;
	}

	//�����׽���״̬
	m_socketStatus = enmSocketStatus_Closed;
}

//�������׽���
int32_t CTCPSocket::Attach(SocketFD socketFD)
{
	//��socket�����Ѵ������ȹر��׽���
	if (enmInvalidSocketFD != m_socketFD)
	{
		CloseSocket();
	}

	m_socketFD = socketFD;
	m_socketType = enmSocketType_Communicate;
	m_socketStatus = enmSocketStatus_Connected;

	return S_OK;
}
//�����˽�������
int32_t CTCPSocket::Connect_With_Timeout(const char* szRemoteIP, uint16_t nPort, uint16_t secs, bool bNonBlock/* = true*/)
{
	if (NULL == szRemoteIP)
	{
		return E_REMOTEIP;
	}

	//�ж��׽�������
	if (enmSocketType_Communicate != m_socketType)
	{
		return E_SOCKETTYPE;
	}

	//�׽����Ƿ��
	if ((enmInvalidSocketFD == m_socketFD) || (enmSocketStatus_Opened != m_socketStatus))
	{
		return E_SOCKETNOTCREATED;
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(szRemoteIP);
	addr.sin_port = htons(nPort);
	int32_t ret; 
#ifdef WIN32
	if(bNonBlock)
	{
		unsigned long ul = 1;
		ret = ioctlsocket(m_socketFD,FIONBIO,(unsigned long*)&ul);
		if(ret == SOCKET_ERROR)
		{
			return SOCKET_ERROR;
		}
	}
#else
	if (bNonBlock)
	{
		set_non_block(m_socketFD);
	}
#endif

	//��������˽�������
	ret = connect(m_socketFD, (const struct sockaddr*)&addr, sizeof(addr));
	if (0 != ret)
	{
#ifdef WIN32
		if(WSAGetLastError() != WSAEWOULDBLOCK)
		{
			CloseSocket();
			return E_SOCKETCONNECT;
		}
#else
		if(errno != EINPROGRESS)
		{
			CloseSocket();
			return E_SOCKETCONNECT;
		}
#endif

	}
	struct timeval to;
	fd_set rSet,wSet,eSet;
	FD_ZERO(&rSet);
	FD_ZERO(&wSet);
	FD_ZERO(&eSet);
	FD_SET(m_socketFD,&rSet);
	FD_SET(m_socketFD,&wSet);
	FD_SET(m_socketFD,&eSet);
	to.tv_sec = secs;
	to.tv_usec= 0;
#ifdef WIN32
	ret = select(0,&rSet,&wSet,&eSet,&to);
#else
	ret = select(m_socketFD+1,&rSet,&wSet,0,&to);
#endif
	if(ret <= 0)
	{
		//select error or timeout
		CloseSocket();
		return E_SOCKETCONNECT;
	}
#ifdef WIN32
	if (FD_ISSET(m_socketFD,&eSet))
	{
		//error happened
		CloseSocket();
		return E_SOCKETCONNECT;
	}
#else
	if (FD_ISSET(m_socketFD,&rSet) && FD_ISSET(m_socketFD,&wSet))
	{
		//error happened
		CloseSocket();
		return E_SOCKETCONNECT;
	}
#endif

	//�����׽���״̬
	m_socketStatus = enmSocketStatus_Connected;

	return S_OK;
}

//�����˽�������
int32_t CTCPSocket::Connect(const char* szRemoteIP, uint16_t nPort, bool bNonBlock/* = true*/, int32_t nTimeout/* = 0*/)
{
	if (NULL == szRemoteIP)
	{
		return E_REMOTEIP;
	}

	//�ж��׽�������
	if (enmSocketType_Communicate != m_socketType)
	{
		return E_SOCKETTYPE;
	}

	//�׽����Ƿ��
	if ((enmInvalidSocketFD == m_socketFD) || (enmSocketStatus_Opened != m_socketStatus))
	{
		return E_SOCKETNOTCREATED;
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(szRemoteIP);
	addr.sin_port = htons(nPort);

	if (bNonBlock)
	{
#ifndef WIN32
		set_non_block(m_socketFD);
		//int flags = fcntl(m_socketFD, F_GETFL, 0);
		//fcntl(m_socketFD, F_SETFL, flags | O_NONBLOCK);
#else
		unsigned long ul = 1;
		int32_t ret = ioctlsocket(m_socketFD, FIONBIO, (unsigned long*)&ul);
		if (ret == SOCKET_ERROR)
		{
			return E_SOCKETCONTROL;
		}
#endif
	}

	//��������˽�������
	int32_t ret = connect(m_socketFD, (const struct sockaddr*)&addr, sizeof(addr));
	if (0 != ret)
	{
#ifdef WIN32
		DWORD error = WSAGetLastError();
		if (error != WSAEINPROGRESS && error != WSAEWOULDBLOCK)
		{
			CloseSocket();
			return E_SOCKETCONNECT;
		}
#else
		if (errno != EINPROGRESS)
		{
			CloseSocket();
			return E_SOCKETCONNECT;
		}
		else if (bNonBlock)
		{
			m_socketStatus = enmSocketStatus_Connecting;
			return E_SOCKET_CONNECTING;
		}
#endif
	}

	//���������ȴ���ʱ
	if (bNonBlock && (0 < nTimeout))
	{
		timeval tv;
		tv.tv_sec = nTimeout / MS_PER_SECOND;
		tv.tv_usec = (nTimeout % MS_PER_SECOND) * US_PER_MS;

		fd_set rset;
		fd_set wset;
		FD_ZERO(&rset);
		FD_SET(m_socketFD, &rset);
		wset = rset;

		ret = select((int)m_socketFD + 1, &rset, &wset, NULL, &tv);
		if (0 >= ret)
		{
			CloseSocket();
			return E_SOCKETCONNECT;
		}
#ifndef WIN32
		if ((FD_ISSET(m_socketFD, &rset)) && (FD_ISSET(m_socketFD, &wset)))
		{
			CloseSocket();
			return E_SOCKETCONNECT;
		}
#endif
	}

	//�����׽���״̬
	m_socketStatus = enmSocketStatus_Connected;

	return S_OK;
}

//�����˽�������
int32_t CTCPSocket::Connect(uint32_t nRemoteIP, uint16_t nPort, bool bNonBlock/* = true*/, int32_t nTimeout/* = 0*/)
{
	//�ж��׽�������
	if (enmSocketType_Communicate != m_socketType)
	{
		return E_SOCKETTYPE;
	}

	//�׽����Ƿ��
	if ((enmInvalidSocketFD == m_socketFD) || (enmSocketStatus_Opened != m_socketStatus))
	{
		return E_SOCKETNOTCREATED;
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = nRemoteIP;
	addr.sin_port = htons(nPort);

	if (bNonBlock)
	{
#ifndef WIN32
		set_non_block(m_socketFD);
#else
		unsigned long ul = 1;
		int32_t ret = ioctlsocket(m_socketFD, FIONBIO, (unsigned long*)&ul);
		if (ret == SOCKET_ERROR)
		{
			return E_SOCKETCONTROL;
		}
#endif
	}

	//��������˽�������
	int32_t ret = connect(m_socketFD, (const struct sockaddr*)&addr, sizeof(addr));
	if (0 != ret)
	{
		CloseSocket();
		return E_SOCKETCONNECT;
	}

	//���������ȴ���ʱ
	if (bNonBlock && (0 < nTimeout))
	{
		timeval tv;
		tv.tv_sec = nTimeout / MS_PER_SECOND;
		tv.tv_usec = (nTimeout % MS_PER_SECOND) * US_PER_MS;

		fd_set rset;
		fd_set wset;
		FD_ZERO(&rset);
		FD_SET(m_socketFD, &rset);
		wset = rset;

		ret = select((int)m_socketFD + 1, &rset, &wset, NULL, &tv);
		if (0 >= ret)
		{
			CloseSocket();
			return E_SOCKETCONNECT;
		}
#ifndef WIN32
		if ((FD_ISSET(m_socketFD, &rset)) && (FD_ISSET(m_socketFD, &wset)))
		{
			CloseSocket();
			return E_SOCKETCONNECT;
		}
#endif
	}

	//�����׽���״̬
	m_socketStatus = enmSocketStatus_Connected;

	return S_OK;
}

//�Ͽ�����
int32_t CTCPSocket::Disconnect()
{
	CloseSocket();
	return S_OK;
}

//
int32_t CTCPSocket::Accept(SocketFD& socketFD)
{
	socketFD = accept(m_socketFD, NULL, NULL);
	if (0 > socketFD)
	{
		return E_SOCKETACCEPT;
	}

	return S_OK;
}

//��������
int32_t CTCPSocket::ReceiveData(uint8_t *pBuffer, uint32_t nSize, uint32_t& nReceived)
{
	//�׽����Ƿ��
	if ((enmInvalidSocketFD == m_socketFD) || ((enmSocketStatus_Available != m_socketStatus)&&(enmSocketStatus_Connected != m_socketStatus)))
	{
		return E_SOCKETNOTCREATED;
	}

	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	//���ɶ�״̬
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(m_socketFD, &fds);
	int ret = select((int)m_socketFD + 1, &fds, NULL, NULL, &tv);
	if (0 >= ret)
	{
		nReceived = 0;
		return S_NODATA;
	}

	//��������
#ifdef WIN32
	int32_t n = recv(m_socketFD, (char*)pBuffer, nSize, 0);
#else
	int32_t n = recv(m_socketFD, pBuffer, nSize, 0);
#endif
	if (0 < n)
	{
		nReceived = (uint32_t)n;
		return S_OK;
	}
	else if (0 == n)
	{	
		//�����ѶϿ�
		CloseSocket();
		return E_SOCKETDISCONNECTED;
	}
#ifdef WIN32
	else if (WSAGetLastError() != ERROR_IO_PENDING)
	{
		//���ճ���
		CloseSocket();
		return E_SOCKETERROR;
	}
#else
	else if (EAGAIN != errno)
	{
		//���ճ���
		CloseSocket();
		return E_SOCKETERROR;
	}
#endif

	//�������ڽ���
	nReceived = 0;

	return S_FALSE;
}

//��������
int32_t CTCPSocket::SendData(const uint8_t *pBuffer, const uint32_t nLength, uint32_t& nSent)
{
	if ((NULL == pBuffer) || (0 == nLength))
	{
		return E_INVALIDARGUMENT;
	}

	//�׽����Ƿ��
	if ((enmInvalidSocketFD == m_socketFD) || ((enmSocketStatus_Connected != m_socketStatus)&&(enmSocketStatus_Available != m_socketStatus)))
	{
		return E_SOCKETNOTCREATED;
	}

#ifdef WIN32
	int32_t n = send(m_socketFD, (const char*)pBuffer, nLength, 0);
#else
	int32_t n = send(m_socketFD, pBuffer, nLength, 0);
#endif
	if (n > 0)
	{
		nSent = n;
		return S_OK;
	}
#ifdef WIN32
	else if ((WSAGetLastError() != ERROR_IO_PENDING) && (WSAGetLastError() != WSAENOBUFS))
	{
		//���ͳ���
		CloseSocket();
		return E_SOCKETERROR;
	}
#else
	else if (EAGAIN != errno)
	{
		//���ͳ���
		CloseSocket();
		return E_SOCKETERROR;
	}
#endif

	//��������
	nSent = 0;

	return S_SENDNOTCOMPLETED;

}
