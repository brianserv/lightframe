/*
 * common_string.h
 *
 *  Created on: 2011-11-30
 *      Author: jimm
 */

#ifndef COMMON_STRING_H_
#define COMMON_STRING_H_

#include "common/common_object.h"
#include "common/common_errordef.h"
#include "common/common_codeengine.h"
#include <string.h>
#include <iostream>
using namespace std;

template<uint16_t SIZE>
class CString:public CObject
{
public:
	//锟斤拷锟届函锟斤拷
	CString(const char* init="")
	{
		if(strlen(init)<SIZE)
		{
            memset(m_char,0,sizeof(m_char));
			m_curlen = static_cast<uint16_t>(strlen(init));
			strncpy(m_char,init,strlen(init));
            m_char[strlen(init)]='\0';
		}
		else
		{
            m_curlen = SIZE;
			strncpy(m_char,init,SIZE);
            m_char[SIZE]='\0';
		}
	}
	CString(const CString& ob)
	{
		if(ob.m_curlen<SIZE)
		{
			memset(m_char,0,sizeof(m_char));
			strncpy(m_char,ob.m_char,ob.m_curlen);
			m_curlen=ob.m_curlen;
		}
		else
		{
            memset(m_char,0,sizeof(m_char));
			strncpy(m_char,ob.m_char,SIZE);
			m_curlen=SIZE;
		}

	}
	//锟斤拷锟斤拷锟斤拷锟斤拷
	~CString()
	{
	}

	int32_t Initialize()
	{
		m_curlen = 0;
		memset(m_char, 0, sizeof(m_char));

		return S_OK;
	}

	int32_t Resume()
	{
		m_curlen = 0;
		memset(m_char, 0, sizeof(m_char));

		return S_OK;
	}

	int32_t Uninitialize()
	{
		m_curlen = 0;
		memset(m_char, 0, sizeof(m_char));

		return S_OK;
	}

	//锟斤拷值锟斤拷锟斤拷锟�
	CString& operator=(const CString& ob)
	{
		if(ob.m_curlen<SIZE)
		{
			strncpy(m_char,ob.m_char,ob.m_curlen);
			m_curlen=ob.m_curlen;
		}
		else
		{
           strncpy(m_char,ob.m_char,SIZE);
		   m_curlen=SIZE;
		}
		m_char[m_curlen] = '\0';
		return *this;
	}
	CString& operator=(char* s)
	{
		memset(m_char,0,sizeof(m_char));
		m_curlen = 0;
		if(strlen(s)<SIZE)
		{
			strncpy(m_char,s,strlen(s));
			m_curlen=strlen(s);
		}
		else
		{
            strncpy(m_char,s,SIZE);
			m_curlen=SIZE;
		}
		return *this;
	}

	CString& operator=(const char* s)
	{
		memset(m_char,0,sizeof(m_char));
		m_curlen = 0;
		if(strlen(s)<SIZE)
		{
			strncpy(m_char,s,strlen(s));
			m_curlen=strlen(s);
		}
		else
		{
            strncpy(m_char,s,SIZE);
			m_curlen=SIZE;
		}
		return *this;
	}

	void SetString(const char* s)
	{
		memset(m_char,0,sizeof(m_char));
		m_curlen = 0;
		if(strlen(s)<SIZE)
		{
			strncpy(m_char,s,strlen(s));
			m_curlen=strlen(s);
		}
		else
		{
            strncpy(m_char,s,SIZE);
			m_curlen=SIZE;
		}
	}

	//锟斤拷系锟斤拷锟斤拷锟�
	bool operator==(const CString& ob) const
	{
		return strcmp(m_char,ob.m_char)==0;
	}
	bool operator==(char* s) const
	{
		return strcmp(m_char,s)==0;
	}
	friend bool operator==(char* s,const CString& ob)
	{
		return strcmp(s,ob.m_char)==0;
	}
	bool operator!=(const CString& ob) const
	{
		return strcmp(m_char,ob.m_char)!=0;
	}
	bool operator!=(char* s) const
	{
		return strcmp(m_char,s)!=0;
	}
	friend bool operator!=(char* s,const CString& ob)
	{
		return strcmp(s,ob.m_char)!=0;
	}
	//锟斤拷拼锟斤拷锟斤拷锟斤拷锟�
	CString operator+(const CString& ob) const
	{
		CString <SIZE> ret;
		memset(ret.m_char,0,sizeof(ret.m_char));
		if(ob.m_curlen+m_curlen>=SIZE)
		{
           strncpy(ret.m_char,m_char,m_curlen);
		   strncat(ret.m_char,ob.m_char,SIZE-m_curlen);
		   ret.m_curlen=SIZE;
		    return ret;
		}
		strncpy(ret.m_char,m_char,m_curlen);
		strcat(ret.m_char,ob.m_char);
		ret.m_curlen=ob.m_curlen+m_curlen;
		return ret;
	}
	CString operator+(char* s) const
	{
		CString <SIZE> ret;
		memset(ret.m_char,'\0',sizeof(ret.m_char));
		if(strlen(s)+m_curlen>=SIZE)
		{
            strncpy(ret.m_char,m_char,m_curlen);
            strncat(ret.m_char,s,SIZE-m_curlen);
            ret.m_curlen=SIZE;
			return ret;
		}
		strncpy(ret.m_char,m_char,m_curlen);
		strcat(ret.m_char,s);
		ret.m_curlen=strlen(s)+m_curlen;
		return ret;

	}
	friend CString operator+(char* s,const CString& ob)
	{
		CString <SIZE> ret;
		memset(ret.m_char,0,sizeof(ret.m_char));
		if(strlen(s)+ob.m_curlen>=SIZE)
		{
			strncpy(ret.m_char,s,strlen(s));
		    strncat(ret.m_char,ob.m_char,SIZE-strlen(s));
		    ret.m_curlen=SIZE;
			return ret;
		}
		strncpy(ret.m_char,s,strlen(s));
		strcat(ret.m_char,ob.m_char);
		ret.m_curlen=strlen(s)+ob.m_curlen;
		return ret;
	}

	friend ostream& operator<<(ostream& ostr,const CString& s)
	{
		ostr<<s.m_char;
		return ostr;
	}
	//锟斤拷锟斤拷锟斤拷锟斤拷
	uint16_t Length() const
	{
		return m_curlen;
	}

	uint16_t GetBufSize() const
	{
		return SIZE;
	}

	int StrEmpty() const
	{
		return m_curlen==0;
	}
	void Clear()
	{
		m_curlen = 0;
		memset(m_char,0,sizeof(m_char));
	}
	const char * GetString() const
	{
		return m_char;
	}

	//锟斤拷锟斤拷锟斤拷锟斤拷
	uint16_t GetStringLength() const
	{
		return m_curlen;
	}

	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
	{
		int32_t ret = S_OK;

		if(m_curlen > 0)
		{
			ret = CCodeEngine::Encode(buf, size, offset, m_char, m_curlen);
			if (0 > ret)
			{
				return ret;
			}
		}
		else
		{
			ret = CCodeEngine::Encode(buf, size, offset, m_curlen);
			if (0 > ret)
			{
				return ret;
			}
		}

		return ret;
	}

	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
	{
		int32_t ret = S_OK;

		m_char[sizeof(m_char) - 1] = '\0';

		ret = CCodeEngine::Decode(buf, size, offset, m_char, sizeof(m_char));
		if (0 > ret)
		{
			return ret;
		}
		m_curlen = strlen(m_char);
		return ret;
	}

private:
	uint16_t m_curlen;   			//锟街凤拷锟斤拷实锟绞的达拷小
	char m_char[SIZE+1];		//实锟绞匡拷锟皆达拷锟絊IZE锟斤拷char锟斤拷一锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷\0锟斤拷
};
#endif
