/************************************************************************
 * this file using Microsoft IOCP port multiplexing communication model. 
 * coding by: haoran dai
 * time:2018-5-1 16:23                               
 ***********************************************************************/
#ifndef _MS_NT_IOCP_H
#define _MS_NT_IOCP_H
#include "../cfg/environment.h"
#include "../module/moon_config_struct.h"
#include "../module/module_log.h"
#include "moon_session_manager.h"

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
bool ms_iocp_server_start(const moon_server_config* p_global_server_config);

/**
 * function desc:
 *      stop IOCP service
 */
void ms_iocp_server_stop();

/**
 * @desc iocp send data to client
 * @param socket:client socket
 * @param send_buf:the data of will sent
 * @param size:the data memory length
 * @return the sent-data length
 **/
int ms_iocp_send(MS_SOCKET_CONTEXT * psocket_context,moon_char * utf8_send_buf,int size);

#ifdef __cplusplus
}
#endif

#endif
#endif