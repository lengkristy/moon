/************************************************************************
 * this file using Microsoft IOCP port multiplexing communication model. 
 * coding by: haoran dai
 * time:2018-5-1 16:23                               
 ***********************************************************************/
#pragma once
#ifndef _MS_NT_IOCP
#define _MS_NT_IOCP
#include "../cfg/environment.h"
#include "../module/moon_config_struct.h"
#include "../module/module_log.h"

#ifdef MS_WINDOWS
#include <winsock2.h>
#include <MSWSock.h>
#pragma comment(lib,"ws2_32.lib")


#ifdef __cplusplus
extern "C" {
#endif

/**
 * function desc:
 *      start IOCP service
 * return:
 *      success return true,failed return false 
 */
bool ms_iocp_server_start();

/**
 * function desc:
 *      stop IOCP service
 */
void ms_iocp_server_stop();

#ifdef __cplusplus
}
#endif

#endif
#endif