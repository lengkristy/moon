#include "moon_char.h"
/**********************************************************************
 * this file used to deal with client info
 * coding by: haoran dai
 * time:2018-09-05 12:06
 **********************************************************************/
#ifndef _MOON_CLIENT_H
#define _MOON_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

//define client environment struct
typedef struct _ClientEnvironment{
	char client_sdk_version[20];/*client sdk version*/
	char client_platform[20];/*client platform info,windows/linux/android/ios*/
	char opra_system_version[50];/*system version*/
	char connect_sdk_token[100];/*sdk token*/
}ClientEnvironment;

//define client struct
typedef struct _MoonClient{
	char id[32];/*client id*/
	moon_char token[512];
	ClientEnvironment *p_client_environment;
}MoonClient;


#ifdef __cplusplus
}
#endif

#endif