/*
 * frame_eventid.h
 *
 *  Created on: 2012-12-13
 *      Author: jimm
 */

#ifndef FRAME_EVENTID_H_
#define FRAME_EVENTID_H_

#include "frame_namespace.h"

FRAME_NAMESPACE_BEGIN

//链接关闭事件（客户端主动）
#define SYS_EVENT_CONN_CLIENT_CLOSED				0x0001

//链接关闭事件（服务器主动）
#define SYS_EVENT_CONN_SERVER_CLOSED				0x0002

//链接出错事件
#define SYS_EVENT_CONN_ERROR						0x0003

//链接冲突事件
#define SYS_EVENT_CONN_CONFLICT						0x0004

//注册失败关闭连接事件
#define SYS_EVENT_CONN_REGISTFAILED					0x0005

//注册服务器类型和服务器id事件
#define SYS_EVENT_REGIST_SERVER						0x0006

//服务器内部连接断开事件
#define SYS_EVENT_DISCONNECT_SERVER						0x0007


FRAME_NAMESPACE_END

#endif /* FRAME_EVENTID_H_ */
