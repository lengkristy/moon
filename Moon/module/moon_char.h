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

#ifdef __cplusplus
}
#endif

#endif