/************************************************************************/
/* the moon server interface          									*/
/************************************************************************/
#pragma once
#ifndef _SERVER_H
#define _SERVER_H
#include "../cfg/environment.h"
#include "../module/moon_config_struct.h"
#include "../module/moon_base.h"

#ifdef MS_WINDOWS
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern Moon_Server_Config* p_global_server_config;/*global server config*/

void moon_start(); //start server

/**
 * ����˵����
 *    ��ͻ��˷�����Ϣ
 * ������
 *	  client_id���ͻ���id
 *	  utf8_send_buf��utf8������Ϣ
 *    size:��Ϣ������ռ���ڴ��С
 */
void moon_server_send_msg(moon_char* client_id,moon_char * utf8_send_buf,int size);

void moon_stop();//stop server

#ifdef __cplusplus
}
#endif

#endif
