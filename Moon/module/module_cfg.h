/************************************************************************/
/* 该模块为配置相关		                                                */
/************************************************************************/
#pragma once
#ifndef _MODULE_LOG_H
#define _MODULE_LOG_H
#include "moon_config_struct.h"
#include "../cfg/environment.h"

#ifdef __cplusplus
extern "C" {
#endif

bool load_config(Moon_Server_Config* pConf);//load configuration file

#ifdef __cplusplus
}
#endif

#endif