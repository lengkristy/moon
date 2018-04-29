/************************************************************************/
/* 该模块为字符串处理操作相关											*/
/************************************************************************/
#pragma once
#ifndef _MOON_STRING_H
#define _MOON_STRING_H
#include "../cfg/environment.h"
#ifdef MS_WINDOWS
#include <wchar.h>
#endif
#include "moon_char.h"

#ifdef __cplusplus
extern "C" {
#endif

unsigned long moon_get_string_length(const char* str);/*get string length,with out '\0'*/

void moon_trim(char *strIn, char *strOut);//去除字符串首尾空格

void moon_trim_line(char *strIn, char *strOut);//去掉首尾换行

bool moon_string_parse_to_int(const char* str,unsigned int* outInt);/*将字符串转成unsigned int,如果失败返回false，成功返回true*/

bool stringIsEmpty(char* str);//判断字符串是否为NULL

/************************************************************************/
/* 将字符串小写转换成大写，如果                                         */
/*   参数：srcStr:原字符串												*/
/*		   pOutCapital:转换后的字符串									*/
/*	返回值：返回实际转换字母的个数										*/
/************************************************************************/
int moon_to_capital(char* srcStr,char* pOutCapital);

#ifdef MS_WINDOWS
int moon_ms_windows_ascii_to_utf8(const char* asciiStr,wchar_t* outUTF8);//将ascii转到utf8,成功返回实际转换的字节数，失败返回-1
int moon_ms_windows_ascii_to_unicode(const char* asciiStr,wchar_t* outUnicode);//将ascii转到unicode,成功返回实际转换的字节数，失败返回-1
int moon_ms_windows_unicode_to_utf8(const wchar_t* unicodeStr,wchar_t* outUTF8);//将unicode转到utf8,成功返回实际转换的字节数，失败返回-1
int moon_ms_windows_unicode_to_ascii(const moon_char* unicodeStr,char* outAscii);//将unicode转到ascii,成功返回实际转换的字节数，失败返回-1
int moon_ms_windows_utf8_to_unicode(const moon_char* utf8Str,moon_char* outUnicode);//将utf8转到unicode,成功返回实际转换的字节数，失败返回-1
int moon_ms_windows_utf8_to_ascii(const moon_char* utf8Str,char* outAscii);//将utf8转到unicode,成功返回实际转换的字节数，失败返回-1
#endif

#ifdef __cplusplus
}
#endif

#endif