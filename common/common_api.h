/*
 * common_api.h
 *
 *  Created on: 2011-11-21
 *      Author: jimm
 */

#ifndef COMMON_API_H_
#define COMMON_API_H_


#include <stdio.h>
#include "common_typedef.h"
#include <stdarg.h>
#include <sys/types.h>
#include <sys/syscall.h>

enum
{
	enmMaxProbabilityCount		= 1024,
};

#ifdef WIN32
#define snprintf _snprintf
//int snprintf(char *buffer, int count, const char *format, ...);	//the type of count according to MSDN _snprintf is size_t(unsignde), while "man snprintf" gives int. We keep the func consistent with the behavior on Linux

#endif

#ifdef __cplusplus
extern "C"
{
#endif

	//��ӡcommon��汾��
	void print_version_common_lib();


	//CPU���ߺ���
	void Delay(uint32_t usec);

	//�ַ�����������
	void StringCopy(char* szDest, const char* szSource, uint32_t nLength);

	//�޳��ַ��������Ŀ��ַ�
	void StringTrim(char* szString);
	//���ַ���ת��ΪСд
	void StringToLower(char* szString);

	//
	int64_t atoi64(const char* num);

	//��ȡһ�������
	int32_t Random(int32_t nRange);
	//��ȡһ���������
	int32_t Random2(int32_t nRange);
	//ͨ���������
	int32_t RandomByProbability(uint32_t nCount, int32_t arrID[], int32_t arrProbability[], int32_t nRange);

	//��ϣ����(ELF)
	uint32_t ELFHash(const char *str);

	//�Ƿ�δ������
	bool	IsPupil(const char* szIDCard);

	int32_t HexToInt32(const char* hex_num);

	int32_t UnicodeToUTF8(int32_t unicode_count, const uint16_t unicode_array[],
		uint32_t& utf8_count, uint8_t utf8_array[], uint32_t array_size);

	int32_t UTF8ToUnicode( int32_t utf8_count, const uint8_t utf8_array[],
		uint32_t& unicode_count, uint16_t unicode_array[], uint32_t array_size);

	int32_t GetUnicodeLengthFromUTF8Buffer( int32_t utf8_count,
									   const uint8_t utf8_array[]);

	int32_t base64_encode(char* dest, const uint8_t* src, const uint32_t dest_size, const uint32_t srclen);
	int32_t base64_decode(uint8_t* dest, const char* src, const uint32_t dest_size, uint32_t& destlen);

	/**
	����һ��Unicode�ַ��������ظ��ַ�����Ӧ��ANSI�ַ������ֽ���
	*/
	int32_t AnsiCodeLengthFromUnicodeBuffer(uint16_t* buf, int32_t len);
	/**
	����һ��UTF8�ַ��������ظ��ַ�����Ӧ��ANSI�ַ������ֽ���
	*/
	int32_t AnsiCodeLengthFromUTF8Buffer(uint8_t* buf, int32_t len);

	//����־�������ƴ���
	void MoveLogFile(int32_t max_file_size, const char* file_name, FILE*& pf);

	//�ַ���ת���ɶ˿ں�
	void StringToPorts(const char* szPorts, char separator, uint16_t& nPortCount, uint16_t arrPorts[]);

	int CommonGetLastError();

	uint8_t GetBitNum(uint8_t u);
	int32_t GetDateBySecs(int32_t secs);

	//nDate:20110506
	int32_t GetSecsByDate(int32_t nDate);

	//ȡָ�����ڵ���һ��
	int32_t GetNextDate(int32_t nDate);

	//��ȡ������˶�������
	int32_t GetSecsFromMiddleNight(int32_t secs);

	void RandomSerial(const int32_t nCount, const int32_t arrIn[], int32_t arrOut[]);

	//�Ƿ�ͬһ��
	bool IsSameDay(const int32_t nTime1, const int32_t nTime2);

	int32_t GetMiddleNightTime(const int32_t now);

	//��ȫ���ַ������� (ǿ��'\0'��β)
	char *stringcopy(char *dest, const char *src, size_t n);

	void WriteBill(const char* szFileName, const char* szFormat, ...);
	
	char *inet_ntoa_f(uint32_t ip);

	pid_t gettid();

#if defined(DEBUG) || defined(_DEBUG) || defined(_DEBUG_)
#define 	DEBUG_CODE(debug_code)	{debug_code}
#else
#define		DEBUG_CODE(debug_code)
#endif

#ifdef __cplusplus
}
#endif


#endif /* COMMON_API_H_ */
