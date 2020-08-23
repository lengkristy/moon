#include "moon_base.h"
#include "moon_string.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#ifdef MS_WINDOWS
#include <windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * moon char copy
 */
void moon_char_copy(moon_char* destStr,const moon_char* sourceStr)
{
	strcpy(destStr,sourceStr);
}

/**
 * moon char concat
 */
void moon_char_concat(moon_char* destStr,const moon_char* sourceStr)
{
	strcat(destStr,sourceStr);
}

/**
 * function desc:
 *  make char string to moon char string
 * params:
 *	sourceStr:char string
 *	destStr:out moon char string
 */
void char_to_moon_char(const char* sourceStr,_out_ moon_char* destStr)
{
#ifdef MS_WINDOWS //windows platform

	//将asci 转成 utf-8编码
	moon_ms_windows_ascii_to_utf8(sourceStr,destStr);
#endif
}

/**
 * @desc make moon char to char string
 * @param source_str:moon char string
 * @param dest_str:char string after convertting
 **/
void moon_char_to_char(const moon_char* source_str,_out_ char* dest_str)
{
#ifdef MS_WINDOWS //windows platform

	//将utf-8转成asci编码
	moon_ms_windows_utf8_to_ascii(source_str,dest_str);
#endif
}

/**
 * 函数说明：
 *  比较两个字符串是否相等
 * 参数：
 *  source_str：源字符串
 *  desc_str：目标字符串
 * 返回值：
 *  相等返回true,不相等返回false
 */
bool moon_char_equals(const moon_char* source_str,const moon_char* desc_str)
{
	if(strcmp(source_str,desc_str) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}


/**
 * 函数说明：
 *  实现字符串格式化
 * 参数：
 *  buf：格式化保存的地址
 *  fmt：格式化字符串
 *  ...：不定参数
 */
int moon_sprintf(moon_char *buf, const moon_char *fmt, ...)
{
	va_list ap;
	int ret = -1;
	va_start(ap, fmt);
	ret = vsprintf(buf, fmt, ap);
	va_end(ap);
	return ret;
}

#ifdef __cplusplus
}
#endif