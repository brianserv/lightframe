/*
 * frame_socket.cpp
 *
 *  Created on: 2012-11-20
 *      Author: jimm
 */

#include "common/common_api.h"
#include "common/common_codeengine.h"
#include "frame_netqueue.h"
#include "frame_socket.h"
#include "frame_eventid.h"
#include "frame_logengine.h"
#include "frame_socket_mgt.h"

#include <time.h>

FRAME_NAMESPACE_BEGIN

CSocket::CSocket()
{
	Reset();
}

CSocket::~CSocket()
{
	if (enmInvalidSocketFD != m_nSocketFD)
	{
		CloseSocket(SYS_EVENT_CONN_CONFLICT);
	}
}

//设置套接字描述符
void CSocket::SetSocketFD(SocketFD nSocketFD)
{
	m_nSocketFD = nSocketFD;
}

//获取套接字描述符
SocketFD CSocket::GetSocketFD() const
{
	return m_nSocketFD;
}

//设置套接字状态
void CSocket::SetSocketStatus(SocketStatus nSocketStatus)
{
	m_nSocketStatus = nSocketStatus;
}

//获取套接字状态
SocketStatus CSocket::GetStatus() const
{
	return m_nSocketStatus;
}

void CSocket::SetSocketType(SocketType nSocketType)
{
	m_nSocketType = nSocketType;
}

SocketType CSocket::GetSocketType()
{
	return m_nSocketType;
}

void CSocket::SetSocketAttr(SocketAttr nSocketAttr)
{
	m_nSocketAttr = nSocketAttr;
}

SocketAttr CSocket::GetSocketAttr()
{
	return m_nSocketAttr;
}

void CSocket::SetPeerID(ServerID nPeerID)
{
	m_nPeerID = nPeerID;
}

ServerID CSocket::GetPeerID()
{
	return m_nPeerID;
}

void CSocket::SetPeerType(EntityType nPeerType)
{
	m_nPeerType = nPeerType;
}

EntityType CSocket::GetPeerType()
{
	return m_nPeerType;
}

char *CSocket::GetPeerAddressStr()
{
	return m_szClientAddress;
}

void CSocket::SetPeerAddress(uint32_t nAddress)
{
	m_nPeerAddress = nAddress;

	char *pAddress = inet_ntoa_f(m_nPeerAddress);
	strncpy(m_szClientAddress, pAddress, sizeof(m_szClientAddress));
}

uint32_t CSocket::GetPeerAddress()
{
	return m_nPeerAddress;
}

void CSocket::SetPeerPort(uint16_t nPort)
{
	m_nPeerPort = nPort;
}

uint16_t CSocket::GetPeerPort()
{
	return m_nPeerPort;
}

uint32_t CSocket::GetListenAddress()
{
	return m_nListenAddress;
}

uint16_t CSocket::GetListenPort()
{
	return m_nListenPort;
}

void CSocket::SetBelongPort(uint16_t nPort)
{
	m_nBelongPort = nPort;
}

uint16_t CSocket::GetBelongPort()
{
	return m_nBelongPort;
}

void CSocket::SetCreateTime(time_t nCreateTime)
{
	m_nCreateTime = nCreateTime;
}

void CSocket::SetEpollObj(CEpoll *pEpoll)
{
	m_pEpoll = pEpoll;
}

CEpoll *CSocket::GetEpollObj()
{
	return m_pEpoll;
}

int32_t CSocket::ReadEvent()
{
	int32_t nRet = S_OK;

	//是监听套接字的读事件
	if(enmSocketType_Listen == m_nSocketType)
	{
		struct sockaddr_in stPeerAddress;
		socklen_t nPeerAddressLen = (socklen_t)sizeof(stPeerAddress);

		SocketFD nAcceptFD = enmInvalidSocketFD;
		do
		{
			nAcceptFD = accept(m_nSocketFD, (struct sockaddr *)&stPeerAddress, &nPeerAddressLen);
			if(nAcceptFD < 0)
			{
				break;
			}

			nRet = OnAccept(nAcceptFD, stPeerAddress, nPeerAddressLen);
			if(nRet < 0)
			{
				close(nAcceptFD);
			}
		}while(nAcceptFD > 0);

	}
	//非监听套接字
	else
	{
		int32_t nError = 0;
		socklen_t nLen = sizeof(int32_t);

		if(0 != getsockopt(m_nSocketFD, SOL_SOCKET, SO_ERROR, &nError, &nLen))
		{
			nError = 0;
		}

		nRet = OnRead(nError);
	}

	return nRet;
}

int32_t CSocket::WriteEvent()
{
	int32_t nError = 0;
	socklen_t nLen = sizeof(int32_t);

	if(0 != getsockopt(m_nSocketFD, SOL_SOCKET, SO_ERROR, &nError, &nLen))
	{
		nError = 0;
	}

	if(m_nSocketStatus == enmSocketStatus_Connecting)
	{
		int32_t nError = 0;
//		socklen_t nLen = sizeof(int32_t);
		//连接成功
		if(0 == nError)
		{
			return Connected();
		}
	}

	return OnWrite(nError);
}

int32_t CSocket::ErrorEvent()
{
	int32_t nError = 0;
	socklen_t nLen = sizeof(int32_t);

	if(0 != getsockopt(m_nSocketFD, SOL_SOCKET, SO_ERROR, &nError, &nLen))
	{
		nError = 0;
	}

	return OnError(nError);
}

//接收连接回调(重载此函数，可以在这里做些连接信息初始化的工作)
int32_t CSocket::OnAccept(SocketFD nAcceptFD, struct sockaddr_in stPeerAddress,
		socklen_t nPeerAddressLen)
{
	return S_OK;
}

//连接成功回调(重载此函数，可以在这里做些连接信息初始化的工作)
int32_t CSocket::OnConnected()
{
	return S_OK;
}

int32_t CSocket::OnConnectTimeout()
{
	return S_OK;
}

//断开连接回调(重载此函数，可以在这里做些资源回收的工作)
int32_t CSocket::OnDisconnect(int32_t nCloseCode)
{
	return S_OK;
}

//发送缓存中剩余的数据
int32_t CSocket::SendRestData()
{
	int32_t nRet = S_OK;
	//int32_t nWriteBytes = 0;
	//先发送缓存中剩余的数据
	if(m_stSendBuffer.Size() > 0)
	{
		uint8_t arrPacketBuf[enmRecvBufferSize];
		int32_t nWantSize = sizeof(arrPacketBuf);

		int32_t nRealSize = m_stSendBuffer.Read(arrPacketBuf, nWantSize);
		int32_t nBytes = nWrite(arrPacketBuf, nRealSize);
		if(nBytes < nRealSize)
		{
			int32_t nRewriteIndex = 0;
			if(nBytes > 0)
			{
				nRewriteIndex = nBytes;
			}
			//剩余数据写回缓存
			m_stSendBuffer.WriteToHead(&arrPacketBuf[nRewriteIndex], nRealSize - nRewriteIndex);
			if(errno != EAGAIN)
			{
				nRet = E_SOCKETERROR;
				//break;
			}
		}
	}
	ChangeWriteEvent();

	return nRet;
}

int32_t CSocket::OpenSocketAsServer(const char* szLocalIP, uint16_t nPort)
{
	//若socket连接已存在则先关闭套接字
	if (enmInvalidSocketFD != m_nSocketFD)
	{
		g_FrameLogEngine.WriteNetioLog(enmLogLevel_Notice, "[%s:%d] init socket fd is not invalid, close it first! sockfd=%d\n",
			__FILE__, __LINE__, m_nSocketFD);
		CloseSocket(SYS_EVENT_CONN_CONFLICT);
	}

	//打开套接字
	m_nSocketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (enmInvalidSocketFD == m_nSocketFD)
	{
		g_FrameLogEngine.WriteNetioLog(enmLogLevel_Notice, "[%s:%d] call socket to create server socket failed!\n", 
			__FILE__, __LINE__);
		return E_SOCKETCREATE;
	}

	if((m_nSocketType == enmSocketType_Listen) || (nPort > 0))
	{
		//设置套接字属性
		int32_t op = 1;
		int32_t ret = setsockopt(m_nSocketFD, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op));
		if (0 != ret)
		{
			CloseSocket();
			g_FrameLogEngine.WriteNetioLog(enmLogLevel_Error, "[%s:%d]set socket option so_reuseaddr failed! close this socket! sockfd=%d\n",
				__FILE__, __LINE__, m_nSocketFD);
			return E_SOCKETOPTION;
		}
		//填充服务器地址&端口信息
		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		if((NULL == szLocalIP) || (strlen(szLocalIP) == 0))
		{
			addr.sin_addr.s_addr = htonl(INADDR_ANY);
		}
		else
		{
			addr.sin_addr.s_addr = inet_addr(szLocalIP);
		}
		addr.sin_port = htons(nPort);

		ret = bind(m_nSocketFD, (struct sockaddr *)&addr, sizeof(addr));
		if (0 != ret)
		{
			CloseSocket();
			g_FrameLogEngine.WriteNetioLog(enmLogLevel_Error, "[%s:%d] bind server address failed! serverip=%s, serverport=%d, errorcode=%d\n", 
				__FILE__, __LINE__, inet_ntoa(addr.sin_addr), htons(addr.sin_port), errno);
			return E_SOCKETBIND;
		}

		//开始监听
		ret = listen(m_nSocketFD, SOMAXCONN);
		if (0 != ret)
		{
			CloseSocket();
			g_FrameLogEngine.WriteNetioLog(enmLogLevel_Error, "[%s:%d] listen on server socket failed! serverip=%s, serverport=%d, errorcode=%d\n", 
				__FILE__, __LINE__, inet_ntoa(addr.sin_addr), htons(addr.sin_port), errno);
			return E_SOCKETLISTEN;
		}

		m_nListenAddress = addr.sin_addr.s_addr;
		m_nListenPort = nPort;

		g_FrameLogEngine.WriteNetioLog(enmLogLevel_Debug, "[%s:%d] listen on server socket success! serverip=%s, serverport=%d\n", 
			__FILE__, __LINE__, inet_ntoa(addr.sin_addr), htons(addr.sin_port));
	}

	//设置为非阻塞
	set_non_block(m_nSocketFD);

	//更新套接字类型和状态
	m_nSocketType = enmSocketType_Listen;
	m_nSocketStatus = enmSocketStatus_Opened;

	return S_OK;
}

int32_t CSocket::OpenSocketAsClient(const char* szLocalIP/* = NULL*/)
{
	//若socket连接已存在则先关闭套接字
	if (enmInvalidSocketFD != m_nSocketFD)
	{
		CloseSocket(SYS_EVENT_CONN_CONFLICT);
	}
	//打开套接字
	m_nSocketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (enmInvalidSocketFD == m_nSocketFD)
	{
		return E_SOCKETCREATE;
	}

	if (NULL != szLocalIP)
	{
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));

		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr(szLocalIP);
		int32_t ret = bind(m_nSocketFD, (struct sockaddr *)&addr, sizeof(addr));
		if (0 != ret)
		{
			CloseSocket();
			return E_SOCKETBIND;
		}

	}

	set_non_block(m_nSocketFD);

	//更新套接字类型和状态
	m_nSocketType = enmSocketType_Communicate;
	m_nSocketStatus = enmSocketStatus_Opened;
	return S_OK;
}

//关闭套接字
void CSocket::CloseSocket(int32_t nCloseCode)
{
	if (enmInvalidSocketFD != m_nSocketFD)
	{
		if(m_nSocketStatus != enmSocketStatus_Closed)
		{
			Disconnected(nCloseCode);
			//更新套接字状态
			m_nSocketStatus = enmSocketStatus_Closed;
		}
		close(m_nSocketFD);
		m_nSocketFD = enmInvalidSocketFD;

		Clear();
	}
}

//与服务端建立连接
int32_t CSocket::Connect(const char* szRemoteIP, uint16_t nPort)
{
	if (NULL == szRemoteIP)
	{
		return E_REMOTEIP;
	}

	//判断套接字类型
	if (enmSocketType_Communicate != m_nSocketType)
	{
		return E_SOCKETTYPE;
	}

	//套接字是否打开
	if ((enmInvalidSocketFD == m_nSocketFD) || (enmSocketStatus_Opened != m_nSocketStatus))
	{
		return E_SOCKETNOTCREATED;
	}

	strcpy(m_szClientAddress, szRemoteIP);
	m_nPeerPort = nPort;
	m_nPeerAddress = (uint32_t)inet_addr(szRemoteIP);

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(szRemoteIP);
	addr.sin_port = htons(nPort);

	m_pSocketTimer = g_FrameSocketTimerMgt.CreateSocketTimer(this,
			static_cast<SocketTimerProc>(&CSocket::OnTimerEvent), 3);
	if(m_pSocketTimer == NULL)
	{
		return E_UNKNOWN;
	}

	//与服务器端建立连接
	int32_t ret = connect(m_nSocketFD, (const struct sockaddr*)&addr, sizeof(addr));
	if (0 != ret)
	{
		if (errno != EINPROGRESS)
		{
			CloseSocket();
			return E_SOCKETCONNECT;
		}
		else
		{
			//添加peer端与此socket对象的映射
			g_FrameSocketMgt.AddSocket(m_nPeerType, m_nPeerID, this);

			m_nSocketStatus = enmSocketStatus_Connecting;
			return E_SOCKET_CONNECTING;
		}
	}

	//添加peer端与此socket对象的映射
	g_FrameSocketMgt.AddSocket(m_nPeerType, m_nPeerID, this);

	//更新套接字状态
	m_nSocketStatus = enmSocketStatus_Connected;

	Connected();

	return S_OK;
}

//定时器事件
int32_t CSocket::OnTimerEvent(CFrameSocketTimer *pTimer)
{
	return ConnectTimeout();
}

//连接超时
int32_t CSocket::ConnectTimeout()
{
	if(m_pSocketTimer != NULL)
	{
		g_FrameSocketTimerMgt.DestroySocketTimer(m_pSocketTimer);
		m_pSocketTimer = NULL;
	}

	//多次发送连接请求的情况下，可能某一次连接成功了，另外一些连接请求超时
	if((m_nSocketStatus != enmSocketStatus_Closed) && (m_nSocketStatus != enmSocketStatus_Error))
	{
		return S_OK;
	}

	m_nSocketStatus = enmSocketStatus_Closed;

	if(m_pEpoll != NULL)
	{
		m_pEpoll->DeleteEvent(this);
	}

	return OnConnectTimeout();
}

//连接成功的回调
int32_t CSocket::Connected()
{
	if(m_pSocketTimer != NULL)
	{
		g_FrameSocketTimerMgt.DestroySocketTimer(m_pSocketTimer);
		m_pSocketTimer = NULL;
	}

	if((m_nSocketStatus != enmSocketStatus_Connected) &&
			(m_nSocketStatus != enmSocketStatus_Available))
	{
		m_nSocketStatus = enmSocketStatus_Connected;
	}

	sockaddr_in stLocalAddr;
	memset(&stLocalAddr, 0, sizeof(stLocalAddr));
	int32_t nAddrLen = sizeof(stLocalAddr);

	int32_t nRet = getsockname(m_nSocketFD, (sockaddr *)&stLocalAddr, (socklen_t *)&nAddrLen);
	if(nRet != 0)
	{
		g_FrameLogEngine.WriteNetioLog(enmLogLevel_Notice, "[%s:%d]getsockname error, close it first! sockfd=5d\n",
			__FILE__, __LINE__, m_nSocketFD);
		CloseSocket(SYS_EVENT_CONN_CONFLICT);

		return S_OK;
	}

	//如果本地地址和端口等于远程地址和端口，就断开连接
	if(((strcmp(m_szClientAddress, "127.0.0.1") == 0) ||
			(strcmp(m_szClientAddress, inet_ntoa(stLocalAddr.sin_addr)) == 0)) &&
			(stLocalAddr.sin_port == htons(m_nPeerPort)))
	{
		g_FrameLogEngine.WriteNetioLog(enmLogLevel_Notice, "[%s:%d]tcp self connection, close it first! sockfd=5d\n",
			__FILE__, __LINE__, m_nSocketFD);
		CloseSocket(SYS_EVENT_CONN_CONFLICT);

		return S_OK;
	}

	return OnConnected();
}

//连接成功的回调
int32_t CSocket::Disconnected(int32_t nCloseCode)
{
	if(m_pSocketTimer != NULL)
	{
		g_FrameSocketTimerMgt.DestroySocketTimer(m_pSocketTimer);
		m_pSocketTimer = NULL;
	}

	return OnDisconnect(nCloseCode);
}

//接收数据
int32_t CSocket::Recv(uint8_t *pBuffer, int32_t nSize, int32_t& nRecvBytes)
{
	nRecvBytes = 0;

	//套接字是否打开
	if (enmInvalidSocketFD == m_nSocketFD)
	{
		return E_SOCKETNOTCREATED;
	}

	if((enmSocketStatus_Available != m_nSocketStatus)&&(enmSocketStatus_Connected != m_nSocketStatus))
	{
		struct sockaddr_in sa;
		socklen_t nLen = sizeof(sa);
		if(!getpeername(m_nSocketFD, (struct sockaddr *)&sa, &nLen))
		{
			Connected();
			return S_OK;
		}

		return E_SOCKETERROR;
	}

	m_nLastRecvTime = time(NULL);

	//接收数据
	bool isConnClose = false;
	int32_t n = nRead(pBuffer, nSize,isConnClose);
	if (0 < n)
	{
		nRecvBytes = (uint32_t)n;
		if(isConnClose)
		{
			return E_SOCKETDISCONNECTED;
		}
		if(nRecvBytes < nSize)
		{
			if(errno != EAGAIN)
			{
				return E_SOCKETERROR;
			}
		}
		return S_OK;
	}
	else if (0 == n)
	{
		//连接已断开
		return E_SOCKETDISCONNECTED;
	}
	else if (EAGAIN != errno)
	{
		//接收出错
		return E_SOCKETERROR;
	}

	return S_OK;
}
//发送数据
int32_t CSocket::Send(const uint8_t *pBuffer, const int32_t nLength, int32_t& nSendBytes)
{
	nSendBytes = 0;

	//套接字是否打开
	if (enmInvalidSocketFD == m_nSocketFD)
	{
		return E_SOCKETNOTCREATED;
	}

	if((enmSocketStatus_Available != m_nSocketStatus)&&(enmSocketStatus_Connected != m_nSocketStatus))
	{
		struct sockaddr_in sa;
		socklen_t nLen = sizeof(sa);
		if(!getpeername(m_nSocketFD, (struct sockaddr *)&sa, &nLen))
		{
			Connected();
			return S_OK;
		}

		return E_SOCKETERROR;
	}

	//发送缓存还有数据
	if(m_stSendBuffer.Size() > 0)
	{
		uint8_t arrSendBuf[enmRecvBufferSize];
		int32_t nWantSendSize = sizeof(arrSendBuf);
		int32_t nRealRestSize = m_stSendBuffer.Read(arrSendBuf, nWantSendSize);
		if(nRealRestSize > 0)
		{
			int32_t nSendSizes = nWrite(arrSendBuf, nRealRestSize);
			if(nSendSizes < nRealRestSize)
			{
				int32_t nRewriteIndex = 0;
				if(nSendSizes > 0)
				{
					nRewriteIndex = nSendSizes;
				}
				//剩余数据写回发送缓存
				m_stSendBuffer.WriteToHead(&arrSendBuf[nRewriteIndex], nRealRestSize - nRewriteIndex);

				ChangeWriteEvent();
				if(errno != EAGAIN)
				{
					return E_SOCKETERROR;
				}
			}
		}
	}

	if ((NULL == pBuffer) || (0 == nLength))
	{
		return S_FALSE;
	}

	if(m_stSendBuffer.Size() > 0)
	{
		nSendBytes = nLength;
		ChangeWriteEvent();
		return m_stSendBuffer.Write(pBuffer, nLength);
	}

	nSendBytes = nWrite(pBuffer, nLength);
	if(nSendBytes < nLength)
	{
		int32_t nRewriteIndex = 0;
		if(nSendBytes > 0)
		{
			nRewriteIndex = nSendBytes;
		}
		//剩余数据写回发送缓存
		m_stSendBuffer.WriteToHead(&pBuffer[nRewriteIndex], nLength - nRewriteIndex);

		ChangeWriteEvent();
		if(errno != EAGAIN)
		{
			return E_SOCKETERROR;
		}
	}
	ChangeWriteEvent();

	return S_OK;
}

uint32_t CSocket::GetIOEvents()
{
	return m_nIOEvents;
}

void CSocket::SetIOEvents(uint32_t nEvents)
{
	m_nIOEvents = nEvents;
}

//尝试尽最大努力读取想读的数据
int32_t CSocket::nRead(uint8_t *pBuffer, const int32_t nLength,bool &isConnClose)
{
	int32_t nReadBytes = 0;
	isConnClose = false;
	if ((NULL == pBuffer) || (0 == nLength))
	{
		return nReadBytes;
	}

	//套接字是否打开
	if ((enmInvalidSocketFD == m_nSocketFD) || ((enmSocketStatus_Available != m_nSocketStatus)&&(enmSocketStatus_Connected != m_nSocketStatus)))
	{
		return nReadBytes;
	}

	do
	{
		int32_t nRecvBytes = recv(m_nSocketFD, pBuffer + nReadBytes, nLength - nReadBytes, 0);
		if(nRecvBytes > 0)
		{
			nReadBytes += nRecvBytes;
		}
		else if(nRecvBytes <= 0)
		{
			if(nRecvBytes == 0)
			{
				isConnClose = true;
			}
//			if(errno != EAGAIN)
//			{
//				CloseSocket();
//			}
			break;
		}
	}while(nReadBytes < nLength);

	m_nTotalRecvBytes += nReadBytes;
	m_nLastRecvTime = time(NULL);

	return nReadBytes;
}

//尝试尽最大努力写入想写的数据
int32_t CSocket::nWrite(const uint8_t *pBuffer, const int32_t nLength)
{
	int32_t nWriteBytes = 0;

	if ((NULL == pBuffer) || (0 == nLength))
	{
		return nWriteBytes;
	}

	//套接字是否打开
	if ((enmInvalidSocketFD == m_nSocketFD) || ((enmSocketStatus_Available != m_nSocketStatus)&&(enmSocketStatus_Connected != m_nSocketStatus)))
	{
		return nWriteBytes;
	}

	do
	{
		int32_t nBytes = send(m_nSocketFD, pBuffer + nWriteBytes, nLength - nWriteBytes, 0);
		if(nBytes > 0)
		{
			nWriteBytes += nBytes;
		}
		else if(nBytes <= 0)
		{
//			if(EAGAIN != errno)
//			{
//				//出现异常则先写入发送缓存
//				m_stSendBuffer.WriteToHead(pBuffer + nWriteBytes, nLength - nWriteBytes);
//				CloseSocket();
//			}
			break;
		}
	}while(nWriteBytes < nLength);

	m_nTotalSendBytes += nWriteBytes;
	m_nLastSendTime = time(NULL);

	return nWriteBytes;
}

//改变epoll事件
void CSocket::ChangeWriteEvent()
{
	if((m_stSendBuffer.Size() > 0) && ((m_nIOEvents & EPOLLOUT) == 0))
	{
		m_pEpoll->RegistEvent(this, EPOLLOUT);
	}
	else if((m_stSendBuffer.Size() <= 0) && ((m_nIOEvents & EPOLLOUT) != 0))
	{
		m_pEpoll->RemoveEvent(this, EPOLLOUT);
	}
}

FRAME_NAMESPACE_END
