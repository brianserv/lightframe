/*
 * common_wrapcache.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_WRAPCACHE_H_
#define COMMON_WRAPCACHE_H_

#include "common_cache.h"

typedef int32_t	ObjectIndex;
enum
{
	enmInvalidObjectIndex					= -1,
};

template<typename T, typename Key, uint32_t CAPACITY, uint32_t BUCKETSIZE>
class CWrapCache : public CObject
{
	typedef CCache<T, CAPACITY, BUCKETSIZE> Cache;

public:
	CWrapCache()
	{
	}

	virtual ~CWrapCache()
	{
	}

public:
	//��ʼ��
	virtual int32_t Initialize()
	{
		return m_cache.Initialize();
	}

	virtual	int32_t	Resume()
	{
		return m_cache.Resume();
	}

	virtual int32_t Uninitialize()
	{
		return m_cache.Uninitialize();
	}

public:
	//��������
	T* CreateObject(Key key)
	{
		ObjectIndex index = enmInvalidObjectIndex;
		return CreateObject(key, index);
	}

	//��������
	T* CreateObject(Key key, ObjectIndex& index)
	{
		typename Cache::CIndex* pIndex = m_cache.CreateObject(MakeKey(key));
		return NULL == pIndex ? NULL : (index = pIndex->Index(), &pIndex->GetObject());
	}

	//��ȡ����
	T* GetObject(Key key)
	{
		typename Cache::CIndex* pIndex = m_cache.FindObject(MakeKey(key));
		return pIndex == NULL ? NULL : &pIndex->GetObject();
	}

	//��ȡ����
	T* GetObjectByIndex(ObjectIndex index)
	{
		typename Cache::CIndex* pIndex = m_cache.GetIndex(index);
		return pIndex == NULL ? NULL : &pIndex->GetObject();
	}

	//ɾ������
	int32_t DeleteObject(Key key)
	{
		return m_cache.DeleteObject(MakeKey(key));
	}

	//ɾ������
	int32_t DeleteObjectByIndex(ObjectIndex index)
	{
		typename Cache::CIndex* pIndex = m_cache.GetIndex(index);
		if (NULL == pIndex)
		{
			return E_INVALIDOBJECTINDEX;
		}
		return m_cache.DeleteObject(pIndex);
	}

	//������ж���
	void Clear()
	{
		m_cache.Clear();
	}

	//��һ������
	T* GetFirstObject()
	{
		typename Cache::CIndex pIndex = m_cache.FirstActive();
		return pIndex == NULL ? NULL : &pIndex->GetObject();
	}

	//��ȡ���һ������
	T* GetLastObject()
	{
		typename Cache::CIndex pIndex = m_cache.LastInactive();
		return pIndex == NULL ? NULL : &pIndex->GetObject();
	}

	//��ȡ���������
	uint32_t Capacity() const
	{
		return CAPACITY;
	}

	//��ȡ��������
	uint32_t GetObjectCount() const
	{
		return m_cache.ObjectCount();
	}

	//������Ƿ�Ϊ��
	bool IsEmpty() const
	{
		return m_cache.IsEmpty();
	}

	//������Ƿ�����
	bool IsFull() const
	{
		return m_cache.IsFull();
	}

protected:
	typename Cache::Key MakeKey(Key key)
	{
		Key arr[2];
		const uint32_t nKeyLength = (uint32_t)((((uint64_t)&arr[1] - (uint64_t)&arr[0])) * sizeof(uint8_t));

		typename Cache::Key cacheKey = { 0 };
		cacheKey.nKeyLength = nKeyLength;
		*(Key*)cacheKey.arrKey = key;
		return cacheKey;
	}

protected:
	Cache					m_cache;
};

#endif /* COMMON_WRAPCACHE_H_ */
