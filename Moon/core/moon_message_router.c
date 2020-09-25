#include "moon_message_router.h"
#include "../module/module_log.h"
#include "../module/moon_memory_pool.h"
#include "../cfg/environment.h"

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * 路由地址配置列表
	 */
	_global_ char **p_global_router_addr_config = NULL;

	/**
	 * 配置的路由地址数量
	 */
	_global_ int global_router_addr_count = 0;


	/**
	 * 函数说明：
	 *   解析配置
	 * 参数：
	 *   p_global_server_config：服务配置
	 * 返回值：
	 *   成功返回true，失败返回false
	 */
	static bool _parse_config(moon_server_config *p_server_config)
	{
		//解析配置了多少个消息路由服务器
		int index = 0;
		int tmp_index = 0;
		int p_index = 0;
		char addr[20] = {0};
		while(p_server_config->router_server_ip[index] != '\0')
		{
			if (p_server_config->router_server_ip[index] == ',')
			{
				global_router_addr_count++;
			}
			index++;
		}
		global_router_addr_count++;
		p_global_router_addr_config = (char**)moon_malloc(sizeof(char *) * global_router_addr_count);
		index = 0;
		while(p_server_config->router_server_ip[index] != '\0')
		{
			if (p_server_config->router_server_ip[index] != ',')
			{
				addr[tmp_index] = p_server_config->router_server_ip[index];
				tmp_index++;
			}
			else
			{
				//解析完成一个
				p_global_router_addr_config[p_index] = (char*)moon_malloc(20);
				memset((char*)p_global_router_addr_config[p_index],0,20);
				strcpy((char*)p_global_router_addr_config[p_index],addr);
				tmp_index = 0;
				memset(addr,0,20);
				p_index++;
			}
			index++;
		}

		//将最后一个ip添加进入
		p_global_router_addr_config[p_index] = (char*)moon_malloc(20);
		memset((char*)p_global_router_addr_config[p_index],0,20);
		strcpy((char*)p_global_router_addr_config[p_index],addr);
	}

	/**
	 * 函数说明：
	 *    开启消息路由服务
	 * 参数说明：
	 *    p_server_config：服务配置
	 */
	void start_message_router_service(moon_server_config *p_server_config)
	{

		moon_write_info_log("start message router service...");
		if (!_parse_config(p_server_config))
		{
			moon_write_error_log("parse router config failed,please check router config");
			return;
		}
		//开始连接
		
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