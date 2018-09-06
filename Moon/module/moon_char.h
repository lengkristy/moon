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

#ifdef __cplusplus
}
#endif

#endif