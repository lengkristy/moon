#include "moon_message_router.h"
#include "../module/module_log.h"
#include "../module/moon_memory_pool.h"
#include "../cfg/environment.h"
#include "../msg/moon_msg.h"
#include "../module/moon_string.h"


#ifdef MS_WINDOWS
#include <WinSock2.h>
#endif

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
	 * 定义路由套接字客户端
	 */
#ifdef MS_WINDOWS
	static SOCKET router_socket;//连接路由服务套接字
	static HANDLE handle_recv_data_thread;//接收数据线程句柄
	static int thread_id_recv_data;//接收数据线程id
	static bool server_is_stop = false;//服务是否停止
#endif


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
	 *   解析ip和端口
	 * 参数：
	 *   p_config：配置字符串
	 *   p_out_ip：输出解析的ip信息
	 *   p_port：输出端口信息
	 * 返回值：
	 *   如果成功返回true,失败返回false
	 */
	static bool _parse_ip_port(char* p_config,_out_ char* p_out_ip,_out_ int *p_port)
	{
		char str_port[10] = {0};
		bool ip_over = false;
		char *p_str;
		int index = 0;
		if (p_config == NULL)
		{
			return false;
		}
		p_str = p_config;
		while (p_str[0] != '\0')
		{
			if (p_str[0] != ':' && !ip_over)
			{
				p_out_ip[index] = p_str[0];
				index++;
			}
			else if (p_str[0] == ':')
			{
				ip_over = true;
				index = 0;
			}
			else if (p_str[0] != ':' && ip_over)
			{
				str_port[index] = p_str[0];
				index++;
			}
			p_str++;
		}
		//将字符串端口转成int
		return moon_string_parse_to_int(str_port,p_port);
	}


#ifdef MS_WINDOWS

	/**
	 * 函数说明：
	 *   接收消息
	 * 参数：
	 *   接收消息的套接字
	 */
	void recv_msg(SOCKET socket)
	{

	}

	/**
	 * 函数说明：
	 *   数据接收线程
	 */
	DWORD static WINAPI _data_recv_thread(LPVOID lpParam)
	{
		//打印日志
		struct fd_set fd_read;
		struct timeval timeout;
		char err_msg[255] = {0};
		int ret = 0;
		sprintf(err_msg,"connect router server thread lauched,thread id:%d\n\r",thread_id_recv_data);
		moon_write_info_log(err_msg);
		while(!server_is_stop)
		{
			timeout.tv_sec = 0;
			timeout.tv_usec = 0;
			FD_ZERO(&fd_read);
			FD_SET(router_socket, &fd_read);
			ret = select(router_socket, &fd_read, NULL, NULL, &timeout);
			if (ret < 0)
			{
				memset(err_msg,0,255);
				sprintf(err_msg,"router client select failed,select exit,errno: %ld\n\r",WSAGetLastError());
				moon_write_error_log(err_msg);
				break;
			}
			if (FD_ISSET(router_socket, &fd_read))//数据到来
			{
				recv_msg(router_socket);
			}
			Sleep(10);
		}
		moon_write_info_log("router server thread exit...\n\r");
	}

	/**
	 * 函数说明：
	 *   连接router服务，windows实现方式
	 * 返回值：
	 *   连接成功返回true，连接失败返回false
	 */
	static bool _ms_connect()
	{
		struct sockaddr_in sock_in;
		int index = 0;
		char config[30] = {0};
		char ip[13] = {0};
		int port = 0;
		char err_msg[255] = {0};
		bool suc = false;
		int mode = 1;
		int errnum = 0;
		router_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (router_socket == INVALID_SOCKET) 
		{
			moon_write_error_log("create router socket failed: %ld\n\r", WSAGetLastError());
			return false;
		}
		if (SOCKET_ERROR == ioctlsocket(router_socket,FIONBIO,(u_long FAR*)&mode))
		{
			moon_write_error_log("ioctlsocket() socket failed: %ld\n\r", WSAGetLastError());
			return false;
		}
		
		while (index < global_router_addr_count)
		{
			sock_in.sin_family = AF_INET;
			memset(config,0,30);
			strcpy(config,p_global_router_addr_config[index]);
			if (!_parse_ip_port(config,ip,&port))
			{
				memset(err_msg,0,255);
				sprintf(err_msg,"parse router server config falied,config : %s\n\r",config);
				moon_write_error_log(err_msg);
				index++;
				continue;
			}
			sock_in.sin_addr.s_addr = inet_addr(ip);
			sock_in.sin_port = htons(port);
			//连接服务器
			while(true)
			{
				if (connect(router_socket,(SOCKADDR*)&sock_in,sizeof(sock_in)) == SOCKET_ERROR)
				{
					errnum = WSAGetLastError();
					//无法立即完成非阻塞Socket上的操作
					if(errnum==WSAEWOULDBLOCK||errnum==WSAEINVAL)
					{
						Sleep(1);
						continue;
					}
					else if(errnum==WSAEISCONN)//已建立连接
					{
						suc = true;
						break;
					}
					else
					{
						memset(err_msg,0,255);
						sprintf(err_msg,"current router config:%s can not connect,and try again next config,err_no:\n\r",config,WSAGetLastError());
						moon_write_error_log(err_msg);
						index++;
						break;
					}
				}
			}
			if (suc == true) 
			{
				moon_write_info_log("connect router server sucessful...\n\r");
				break; //已经建立连接则不需要再次建立
			}
		}
		if (!suc)
		{
			moon_write_error_log("can not connect router server,please check config\n\r");
		}
		//开启数据接收线程
		handle_recv_data_thread =  CreateThread(0, 0, _data_recv_thread, NULL, 0, &thread_id_recv_data);
		return suc;
	}
#endif

	/**
	 * 函数说明：
	 *    开启消息路由服务
	 * 参数说明：
	 *    p_server_config：服务配置
	 */
	bool start_message_router_service(moon_server_config *p_server_config)
	{
		moon_char regist_msg[512] = {0};
		bool suc = false;
		moon_write_info_log("start message router service...\n\r");
		if (!_parse_config(p_server_config))
		{
			moon_write_error_log("parse router config failed,please check router config\n\r");
			return suc;
		}
		//开始连接
#ifdef MS_WINDOWS
		suc = _ms_connect();
		if (!suc)
		{
			return suc;
		}
#endif
		//连接建立成功之后发送一条注册信息，将当前通信服务器信息发送到路由服务器
		create_regist_router_server_msg(regist_msg);
		send_message_to_router(regist_msg,moon_char_memory_size(regist_msg));
		return suc;
	}

	/**
	 * 函数说明：
	 *    停止消息路由服务
	 */
	void stop_message_router_service()
	{
		server_is_stop = true;//停止服务
#ifdef MS_WINDOWS
		WaitForSingleObject(handle_recv_data_thread,INFINITE);
		CloseHandle(handle_recv_data_thread);
		closesocket(router_socket);
#endif
		
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
		int errnum = 0;
		int sendlen = 0;
		while(true)
		{
			sendlen = send(router_socket,utf8_send_buf,size,0);
			if (sendlen == SOCKET_ERROR)
			{
				int errnum=WSAGetLastError();
				if(errnum==WSAEWOULDBLOCK)
				{
					//无法立即完成非阻塞Socket上的操作
					Sleep(5);
					continue;
				}

				else
				{
					moon_write_error_log("send message to router server failed,errno:%d",errnum);
					return -1;
				}
			}
			break;
		}
	}

#ifdef __cplusplus
}
#endif