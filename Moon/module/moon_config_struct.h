/************************************************************************/
/* 该模块系统使用配置的结构                                             */
/************************************************************************/
#pragma once
#ifndef _MOON_CONFIG_STRUCT_H
#define _MOON_CONFIG_STRUCT_H

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/* 定义服务器配置的结构                                                 */
/************************************************************************/
typedef struct _Moon_Server_Config{
	char moon_manager_domain[1024];//MoonManager域名地址
	unsigned int client_count;//客户端的数量（单台服务器允许最大客户端连接数量）
	char server_ip[16];//服务器IP
	unsigned int server_port;//服务器端口
	unsigned int http_port;//http服务所用端口
	char server_node_name[255];//服务器节点名称
	char log_level_debug;//是否配置debug日志
	char log_level_info;//是否配置info日志
	char log_level_warnning;//是否配置warnning日志
	char log_level_error;//是否配置error日志
	char moon_work_path[1024];//moon server work path
}Moon_Server_Config;

#ifdef __cplusplus
}
#endif

#endif