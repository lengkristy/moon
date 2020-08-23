#include "../module/moon_base.h"
#include "../collection/array_list.h"

#ifdef MS_WINDOWS
#include "ms_socket_context.h"
#endif

/**********************************************************************
 * this file used to deal with client info
 * coding by: haoran dai
 * time:2018-09-05 12:06
 **********************************************************************/
#ifndef _MOON_SESSION_H
#define _MOON_SESSION_H

#ifdef __cplusplus
extern "C" {
#endif

//define client environment struct
typedef struct _client_environment{
	moon_char client_sdk_version[20];/*client sdk version*/
	moon_char client_platform[20];/*client platform info,windows/linux/android/ios*/
	moon_char opra_system_version[50];/*system version*/
	moon_char connect_sdk_token[100];/*sdk token*/
	moon_char client_id[50];/**�ͻ���id*/
}client_environment;

//define client struct
typedef struct _moon_session{
	client_environment *p_client_environment;/*�ͻ��˻�����Ϣ*/
	moon_char conn_datetime[40];/*�ͻ�������ʱ��*/

#ifdef MS_WINDOWS
	MS_SOCKET_CONTEXT* p_socket_context;/*����windows�µ�socket ������p_socket_context,�����linux����ͬ����������Ϊ��p_socket_context*/
#endif

}moon_session;


#ifdef __cplusplus
}
#endif

#endif