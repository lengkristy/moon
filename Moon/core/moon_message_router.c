#include "moon_message_router.h"
#include "../module/module_log.h"

#ifdef __cplusplus
extern "C" {
#endif


	/**
	 * 函数说明：
	 *    开启消息路由服务
	 * 参数说明：
	 *    p_server_config：服务配置
	 */
	void start_message_router_service(moon_server_config *p_server_config)
	{
		moon_write_info_log("start message router service...");

		//解析配置了多少个消息路由服务器
		char **p_config = NULL;
		
	}

	/**
	 * 函数说明：
	 *    停止消息路由服务
	 */
	void stop_message_router_service()
	{

	}


	/**
	 * 函数说明：
	 *   向router服务发送消息
	 * 参数说明：
	 *   utf8_send_buf：utf8消息
	 *   size:发送消息的字节大小
	 * 返回值：
	 *   如果成功则返回实际发送的字节长度，失败返回-1
	 */
	int send_message_to_router(moon_char * utf8_send_buf,int size)
	{

	}

#ifdef __cplusplus
}
#endif