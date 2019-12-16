/************************************************************************/
/* the moon server interface          									*/
/************************************************************************/
#pragma once
#ifndef _SERVER_H
#define _SERVER_H
#include "../cfg/environment.h"
#include "../module/moon_config_struct.h"
#include "../module/moon_char.h"

#ifdef __cplusplus
extern "C" {
#endif

extern Moon_Server_Config* p_global_server_config;/*global server config*/

void moon_start(); //start server

/**
 * 函数说明：
 *    向客户端发送消息
 * 参数：
 *	  client_id：客户端id
 *	  utf8_send_buf：utf8编码消息
 *    len:消息长度
 */
void moon_server_send_msg(moon_char* client_id,char * utf8_send_buf,int len);

void moon_stop();//stop server

#ifdef __cplusplus
}
#endif

#endif
