/*
 *  frame_dal.h
 *
 *  To do��
 *  Created on: 	2012-12-17
 *  Author: 		luocj
 */

#ifndef FRAME_DAL_H_
#define FRAME_DAL_H_

#include "frame_namespace.h"
#include "frame_def.h"
#include "common/common_singleton.h"


FRAME_NAMESPACE_BEGIN


class CFrameDalMgt : public CObject
{
public:
	CFrameDalMgt();
	virtual ~CFrameDalMgt();

public:
	//��ʼ�����ݷ��ʲ�
	 int32_t Initialize();
	//�ָ����ݷ��ʲ�
	 int32_t Resume();
	//�������ݷ��ʲ�
	 int32_t Uninitialize();

	void AddDataCenter(IDataCenter* pDataCenter);
	IDataCenter* GetDataCenter(const char *szName);

protected:
	 int32_t				m_nDataCenterCount;
	 //Ӧ�ó�����������ļ̳�IDataCenter
	 IDataCenter*			m_arrDataCenter[enmMaxDataCenterCount];
};

#define	CREATE_FRAMEDALMGT_INSTANCE		    CSingleton<CFrameDalMgt>::CreateInstance
#define	g_FrameDalMgt					    CSingleton<CFrameDalMgt>::GetInstance()
#define	DESTROY_FRAMEDALMGT_INSTANCE		CSingleton<CFrameDalMgt>::DestroyInstance

FRAME_NAMESPACE_END

#endif /* FRAME_DAL_H_ */
