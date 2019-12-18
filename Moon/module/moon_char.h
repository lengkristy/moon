/************************************************************************
 * this file used to define type
 * coding by: haoran dai
 * time:2018-5-1 21:56                               
 ***********************************************************************/
#pragma once
#ifndef _MOON_CHAR_H
#define _MOON_CHAR_H
#include "../cfg/environment.h"
#ifdef MS_WINDOWS
#include <wchar.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MS_WINDOWS
	typedef wchar_t moon_char;
#endif

/**
 * moon char copy
 */
void moon_char_copy(moon_char* destStr,const moon_char* sourceStr);

/**
 * moon char concat
 */
void moon_char_concat(moon_char* destStr,const moon_char* sourceStr);

/**
 * function desc:
 *  make char string to moon char string
 * params:
 *	sourceStr:char string
 *	destStr:out moon char string
 */
void char_to_moon_char(const char* sourceStr,_out_ moon_char* destStr);

/**
 * @desc make moon char to char string
 * @param source_str:moon char string
 * @param dest_str:char string after convertting
 **/
void moon_char_to_char(const moon_char* source_str,_out_ char* dest_str);

/**
 * 函数说明：
 *  比较两个字符串是否相等
 * 参数：
 *  source_str：源字符串
 *  desc_str：目标字符串
 * 返回值：
 *  相等返回true,不相等返回false
 */
bool moon_char_equals(const moon_char* source_str,const moon_char* desc_str);


/**
 * 函数说明：
 *  实现字符串格式化
 * 参数：
 *  buf：格式化保存的地址
 *  fmt：格式化字符串
 *  ...：不定参数
 */
int moon_sprintf(moon_char *buf, const moon_char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif