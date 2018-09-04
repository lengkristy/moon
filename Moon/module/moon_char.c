#include "moon_char.h"

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

#ifdef __cplusplus
}
#endif