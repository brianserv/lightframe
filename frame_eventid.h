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

//���ӹر��¼����ͻ���������
#define SYS_EVENT_CONN_CLIENT_CLOSED				0x0001

//���ӹر��¼���������������
#define SYS_EVENT_CONN_SERVER_CLOSED				0x0002

//���ӳ����¼�
#define SYS_EVENT_CONN_ERROR						0x0003

//���ӳ�ͻ�¼�
#define SYS_EVENT_CONN_CONFLICT						0x0004

//ע��ʧ�ܹر������¼�
#define SYS_EVENT_CONN_REGISTFAILED					0x0005

//ע����������ͺͷ�����id�¼�
#define SYS_EVENT_REGIST_SERVER						0x0006

//�������ڲ����ӶϿ��¼�
#define SYS_EVENT_DISCONNECT_SERVER						0x0007


FRAME_NAMESPACE_END

#endif /* FRAME_EVENTID_H_ */
