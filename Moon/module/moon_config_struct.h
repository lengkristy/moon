/************************************************************************
 * this file used to define system configuration struct
 * coding by: haoran dai
 * time:2018-5-1 21:56                               
 ***********************************************************************/
#pragma once
#ifndef _MOON_CONFIG_STRUCT_H
#define _MOON_CONFIG_STRUCT_H

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************
 * define server configuration
 ************************************************************************/
typedef struct _moon_server_config{
	unsigned int client_count;//Number of clients (single server allows maximum number of client connections)
	char server_ip[16];//Server IP
	unsigned int server_port;//Server port
	unsigned int http_port;//The port for the HTTP service.
	char server_node_name[255];//Server node name.
	char log_level_debug;//Configure the debug log.
	char log_level_info;//Configure the info log.
	char log_level_warnning;//Whether to configure the warnning log.
	char log_level_error;//Configure the error log.
	char moon_work_path[1024];//moon server work path
	/**
	 * 消息路由的ip，如果有多个的话，则逗号隔开，
	 * 如果有多个那么连接的时候第一个连不上就用第二个，以此类推
	 */
	char router_server_ip[128];
}moon_server_config;

#ifdef __cplusplus
}
#endif

#endif