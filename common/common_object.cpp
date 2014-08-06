/***************************************************************************************************
*	file name:	common_object.cpp
*	created:	2008-12-03
*	author:		teras xiao
*	version:	1.0
*	purpose:	实现所有实体对象的虚基类
*
*	revision:
*
***************************************************************************************************/


#include "common/common_object.h"
#include "common/common_errordef.h"


CObject::CObject()
{

}

CObject::~CObject()
{

}

int32_t CObject::Initialize()
{
	return S_OK;
}

int32_t CObject::Resume()
{
	return S_OK;
}

int32_t CObject::Uninitialize()
{
	return S_OK;
}

