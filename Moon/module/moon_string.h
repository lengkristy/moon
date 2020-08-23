/************************************************************************
 * this file contain string operate for moon
 * coding by: haoran dai
 * time:2018-5-2 22:01                               
 ***********************************************************************/
#pragma once
#ifndef _MOON_STRING_H
#define _MOON_STRING_H
#include "../cfg/environment.h"
#include "moon_base.h"
#ifdef MS_WINDOWS
#include <windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

unsigned long moon_get_string_length(const char* str);/*get string length,with out '\0'*/

void moon_trim(char *strIn, char *strOut);//Remove the beginning and end Spaces of the string.

void moon_trim_line(char *strIn, char *strOut);//Get rid of the beginning and end new lines of the string.

/**
 * function desc:
 *      Convert the string to unsigned int
 * return:
 *      success return true,failed return false
 */
bool moon_string_parse_to_int(const char* str,unsigned int* outInt);

/**
 * function desc:
 *      Determines whether the string is NULL.
 * params:
 *      string
 * return:
 *      null return true,not null return false
 */
bool stringIsEmpty(char* str);

/**
 * function desc:
 *      Determines whether the string is NULL.
 * params:
 *      string
 * return:
 *      null return true,not null return false
 */
bool moon_string_is_empty(moon_char* str);

/**
 * function desc:
 *      Converts string lowercase to uppercase
 * params:
 *      srcStr:The original string
 *      pOutCapital:The converted string.
 * return:
 *      Returns the number of actual converted letters.
 */
int moon_to_capital(char* srcStr,char* pOutCapital);

/**
 * function desc:
 *      get moon_char length
 * params:
 *      str:the moon_char string
 * return:
 *      return string length
 */
int moon_char_length(const moon_char* str);

/**
 * 函数说明：
 *   获取字符串占用内存的大小
 * 参数：
 *   str：moon_char
 * 返回值：
 *   返回字符串占用内存大小
 */
int moon_char_memory_size(const moon_char* str);

/**
 * function desc:
 *	create 32bit uuid
 * params:
 *	p_out_uuid:return uuid
 */
void moon_create_32uuid(_out_ moon_char* p_out_uuid);

/**
 * 函数说明：
 *   查询字符串，查询findStr在sourceStr中第一次出现的位置
 * 参数：
 *   sourceStr:被查询的源字符串
 *   findStr：被查询的字符串
 * 返回值：
 *   如果查询到，则返回第一次出现的位置，如果没有查询到则返回-1
 */
int index_of(const char* sourceStr,const char* findStr);

/**
 * 函数说明：
 *   查询字符串，查询findStr在sourceStr中第一次出现的位置
 * 参数：
 *   sourceStr:被查询的源字符串
 *   findStr：被查询的字符串
 * 返回值：
 *   如果查询到，则返回第一次出现的位置，如果没有查询到则返回-1
 */
int moon_char_index_of(const moon_char* sourceStr,const moon_char* findStr);

#ifdef MS_WINDOWS
/**
 * function desc:
 *      Turn the ASCII string to the utf8 string.
 * params:
 *      asciiStr:ASCII string
 *      outUTF8:the converted utf-8 string
 * return:
 *      success returns the number of bytes that are actually converted,failure returns -1
 */
int moon_ms_windows_ascii_to_utf8(const char* asciiStr,char* outUTF8);


/**
 * 函数说明：
 *     将int32转成4字节的字符串，只能转4位数及其以下的低于四位数，高位用0补齐
 * 参数：
 *     num：32位数字
 *     outCh：输出转换的字符
 */
void moon_int32_to_4byte(const int num,char outCh[5]);

/**
 * function desc:...
 *      turn the ascii string to the unicode string.
 * params:
 *      asciiStr:ASCII string
 *      outUnicode:the converted unicode string
 * return:
 *      success returns the number of bytes that are actually converted,failure returns -1
 */
int moon_ms_windows_ascii_to_unicode(const char* asciiStr,wchar_t* outUnicode);

/**
 * function desc:
 *      turn the unicode string to the utf8 string.
 * params:
 *      moonchar:unicode string
 *      outUTF8:the converted utf8 string
 * return:
 *      success returns the number of bytes that are actually converted,failure returns -1
 */
int moon_ms_windows_unicode_to_utf8(const wchar_t* moonchar,char* outUTF8);

/**
 * function desc:
 *      turn the unicode string to ascii string.
 * params:
 *      unicodeStr:unicode string
 *      outAscii:the converted ascii string
 * return:
 *      success returns the number of bytes that are actually converted,failure returns -1
 */
int moon_ms_windows_unicode_to_ascii(const wchar_t* unicodeStr,char* outAscii);

/**
 * function desc:
 *      turn the utf8 string to unicode string.
 * params:
 *      utf8Str:utf8 string
 *      outUnicode:the converted unicode string
 * return:
 *      success returns the number of bytes that are actually converted,failure returns -1
 */
int moon_ms_windows_utf8_to_unicode(const char* utf8Str,wchar_t* outUnicode);

/**
 * function desc:
 *      turn utf8 string to ascii string
 * params:
 *      utf8Str:utf8 string
 *      outAscii:the converted ascii string
 * return:
 *      success returns the number of bytes that are actually converted,failure returns -1
 */
int moon_ms_windows_utf8_to_ascii(const char* utf8Str,char* outAscii);

#endif

#ifdef __cplusplus
}
#endif

#endif