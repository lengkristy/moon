/************************************************************************/
/* 服务操作接口															*/
/************************************************************************/
#pragma once
#ifndef _SERVER_H
#define _SERVER_H
#include "../cfg/environment.h"
#include "../module/moon_config_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

extern Moon_Server_Config* p_global_server_config;/*global server config*/

void moon_start(); //start server

void moon_stop();//stop server

#ifdef __cplusplus
}
#endif

#endif