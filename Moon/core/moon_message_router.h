/**********************************************************************************
 * 说明：该文件为消息和路由服务交互模块，向消息路由服务发送消息和接收路由服务的消息
 *  
 **********************************************************************************/
#ifndef _MOON_MESSAGE_ROUTER_H
#define _MOON_MESSAGE_ROUTER_H
#include "../module/moon_config_struct.h"
#include "../module/moon_base.h"

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * 函数说明：
	 *    开启消息路由服务
	 * 参数说明：
	 *    p_server_config：服务配置
	 */
	bool start_message_router_service(moon_server_config *p_server_config);

	/**
	 * 函数说明：
	 *    停止消息路由服务
	 */
	void stop_message_router_service();

	/**
	 * 函数说明：
	 *   向router服务发送消息
	 * 参数说明：
	 *   utf8_send_buf：utf8消息
	 *   size:发送消息的字节大小
	 * 返回值：
	 *   如果成功则返回实际发送的字节长度，失败返回-1
	 */
	int send_message_to_router(moon_char * utf8_send_buf,int size);

#ifdef __cplusplus
}
#endif

#endif