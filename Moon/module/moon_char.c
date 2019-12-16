#include "moon_char.h"
#include "moon_string.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * moon char copy
 */
void moon_char_copy(moon_char* destStr,const moon_char* sourceStr)
{
#ifdef MS_WINDOWS //windows platform
	wcscpy(destStr,sourceStr);
#endif
}

/**
 * moon char concat
 */
void moon_char_concat(moon_char* destStr,const moon_char* sourceStr)
{
#ifdef MS_WINDOWS //windows platform
	wcscat(destStr,sourceStr);
#endif
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
	moon_ms_windows_ascii_to_unicode(sourceStr,destStr);
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
	moon_ms_windows_unicode_to_ascii(source_str,dest_str);
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
#ifdef MS_WINDOWS
	 if(wcscmp(source_str,desc_str) == 0)
	 {
		 return true;
	 }
	 else
	 {
		 return false;
	 }
#endif
	 return false;
}

#ifdef __cplusplus
}
#endif