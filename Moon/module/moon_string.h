/************************************************************************
 * this file contain string operate for moon
 * coding by: haoran dai
 * time:2018-5-2 22:01                               
 ***********************************************************************/
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
 * function desc:
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
int moon_ms_windows_moonchar_to_utf8(const moon_char* moonchar,char* outUTF8);

/**
 * function desc:
 *      turn the unicode string to ascii string.
 * params:
 *      unicodeStr:unicode string
 *      outAscii:the converted ascii string
 * return:
 *      success returns the number of bytes that are actually converted,failure returns -1
 */
int moon_ms_windows_unicode_to_ascii(const moon_char* unicodeStr,char* outAscii);

/**
 * function desc:
 *      turn the utf8 string to unicode string.
 * params:
 *      utf8Str:utf8 string
 *      outUnicode:the converted unicode string
 * return:
 *      success returns the number of bytes that are actually converted,failure returns -1
 */
int moon_ms_windows_utf8_to_unicode(const char* utf8Str,moon_char* outUnicode);

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