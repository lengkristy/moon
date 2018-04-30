/************************************************************************/
/* 使用微软IOCP端口复用通信模型                                         */
/************************************************************************/
#pragma once
#ifndef _MS_NT_IOCP
#define _MS_NT_IOCP
#include "../cfg/environment.h"
#include "../module/moon_config_struct.h"
#include "../module/module_log.h"

#ifdef MS_WINDOWS
// winsock 2 的头文件和库
#include <winsock2.h>
#include <MSWSock.h>
#pragma comment(lib,"ws2_32.lib")


#ifdef __cplusplus
extern "C" {
#endif

bool ms_iocp_server_start();/*启动IOCP服务*/

void ms_iocp_server_stop();/*停止IOCP服务*/

#ifdef __cplusplus
}
#endif

#endif
#endif