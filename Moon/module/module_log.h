/************************************************************************/
/* 该模块为日志操作		                                                */
/************************************************************************/
#pragma once
#ifndef _MODULE_LOG_H
#define _MODULE_LOG_H
#include "../cfg/environment.h"

#ifdef __cplusplus
extern "C" {
#endif

/*define log level*/
#ifndef LOG_LEVEL
#define LOG_ERROR "error"
#define LOG_WARN  "warnning"
#define LOG_DEBUG "debug"
#define LOG_INFO "info"
/**thread mutexes object name*/
#define LOG_MUTEX "LOG_MUTEX"
#endif

bool moon_log_init();//init log

void moon_log_close();//close log

void moon_write_info_log(char* log); //write info log

void moon_write_warn_log(char* log); //write warn log

void moon_write_error_log(char* log); //write error log

void moon_write_debug_log(char* log); //write debug log

#ifdef __cplusplus
}
#endif

#endif