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
typedef struct _Moon_Server_Config{
	char moon_manager_domain[1024];//MoonManager url address
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
}Moon_Server_Config;

#ifdef __cplusplus
}
#endif

#endif