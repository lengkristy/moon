#include "../cfg/environment.h"
#include "server.h"
#include "../module/module_log.h"
#include "../module/module_cfg.h"
#include "../module/moon_memory_pool.h"
#include "ms_nt__iocp.h"
#include "../module/moon_http_service.h"
#include "moon_session_manager.h"
#include "../collection/queue.h"
#include "../msg/moon_msg_handle.h"
#include "../module/moon_string.h"
#include "../module/moon_time.h"
#ifdef MS_WINDOWS
#include "ms_socket_context.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif

moon_server_config* p_global_server_config = NULL;/*global server config*/
bool b_config_load_finish = false;//config has inited

_global_ char g_global_server_start_time[100] = {0};//服务器启动时间

#ifdef MS_WINDOWS
HANDLE g_hReceiveMsgThread = NULL;
HANDLE g_hSendMsgThread = NULL;
#endif

Queue* p_global_receive_msg_queue = NULL;/*全局接收消息队列*/

Queue* p_global_send_msg_queue = NULL;/*全局发送消息队列，所有发送的消息都往该队列放入*/

/************************************************************************/
/* start server			                                                */
/************************************************************************/
void moon_start()
{
	void * p;
#ifdef MS_WINDOWS
	DWORD threadid = 0;
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
	moon_write_info_log("init log environment");
	//init log
	if(!moon_log_init())
	{
		moon_write_error_log("open log file faild,server has exit.");
		return;
	}
	moon_write_info_log("starting server,please waiting...");
	//load config
	if(NULL == p_global_server_config)
	{
		p_global_server_config = (moon_server_config*)moon_malloc(sizeof(moon_server_config));
	}
	if(!load_config(p_global_server_config))
	{
		moon_write_error_log("load config error,server has exit.");
		return;
	}
	b_config_load_finish = true;
#ifdef MS_WINDOWS
	moon_write_info_log("start ms_nt_iocp server...");
	//start windows platform server
	if(!ms_iocp_server_start(p_global_server_config))
	{
		moon_write_error_log("start ms_nt_iocp server falied");
		return;
	}
	moon_write_info_log("ms_nt_iocp server has start");
	//初始化消息队列
	p_global_receive_msg_queue = Queue_Init(receive_msg_queue_event);
	//开启接收消息队列处理线程
	g_hReceiveMsgThread = CreateThread(0, 0, msg_receive_handle_thread, p_global_receive_msg_queue, 0, &threadid);
	if (g_hReceiveMsgThread == NULL)
	{
		moon_write_error_log("create receive msg thread failed");
		moon_stop();
		return;
	}
	p_global_send_msg_queue = Queue_Init(send_msg_queue_event);
	//开启发送消息队列处理线程
	g_hSendMsgThread = CreateThread(0, 0, msg_send_handle_thread, p_global_send_msg_queue, 0, &threadid);
	if(g_hSendMsgThread = NULL)
	{
		moon_write_error_log("create send msg thread failed");
		moon_stop();
		return;
	}
#endif

	//start http server
	lauch_http_service();

	//设置服务器启动时间
	moon_current_time(g_global_server_start_time);
}


/**
 * 函数说明：
 *    向客户端发送消息
 * 参数：
 *	  client_id：客户端id
 *	  utf8_send_buf：utf8编码消息
 *    size:消息缓冲区占用内存大小
 */
void moon_server_send_msg(moon_char* client_id,moon_char * utf8_send_buf,int size)
{
	char str_msg[128] = {0};
	char cid[50] = {0};
	moon_session* p_moon_session = NULL;
	int msgCount = 1;
	int i = 0;
	moon_char* send_msg = NULL;//发送的数据包
	int data_read_lenth = 0; //总共读取数据大小
	char strlength[5] = {0};//数据体大小
	moon_char mclength[5] = {0};//moon_char数据体大小
	//将utf8_send_buf加上包头PKG_HEAD_FLAG标识，为了解决粘包，如果消息长度大于了DATA_BYTE_MAX_LENGTH，那么分批次发送
	if(size > DATA_BYTE_MAX_LENGTH){
		sprintf(str_msg,"the send message length is greater than :%d\r\n",DATA_BYTE_MAX_LENGTH);
		moon_write_error_log(str_msg);
		moon_write_error_log(utf8_send_buf);
		return;
	}
	//读取DATA_BYTE_MAX_LENGTH个字符
	send_msg = (char*)moon_malloc(PKG_BYTE_MAX_LENGTH);
	if(send_msg == NULL) return;
	memset(send_msg,0,PKG_BYTE_MAX_LENGTH);
	strcpy(send_msg,PKG_HEAD_FLAG);
	//读取真实发送字节数
	moon_int32_to_4byte(size - data_read_lenth,strlength);
	char_to_moon_char(strlength,mclength);
	strcat(send_msg,mclength);
	//读取len - data_read_lenth个字节
	memcpy(send_msg + PKG_HEAD_LENGTH,utf8_send_buf,size - data_read_lenth);

	//写入包尾标识
	strcat(send_msg,PKG_TAIL_FLAG);
	
	//
	if(client_id != NULL && moon_char_length(client_id) > 0 && size > 0)
	{
		p_moon_session = get_moon_session_by_client_id(client_id);
		if(p_moon_session != NULL && p_moon_session->p_socket_context != NULL)
		{
#ifdef MS_WINDOWS
			if(ms_iocp_send(p_moon_session->p_socket_context,send_msg,moon_char_memory_size(send_msg)) == -1)
			{
				sprintf(str_msg,"the client id %s send mssage faild.the message :\r\n",cid,utf8_send_buf);
				moon_write_error_log(str_msg);
			}
#endif
		}
		else
		{
			//将消息发送到消息路由服务
			
		}
	}
}

/************************************************************************/
/* stop server                                                          */
/************************************************************************/
void moon_stop()
{
	/*dispose resouce*/
	if(NULL != p_global_server_config)
	{
		moon_free(p_global_server_config);
		p_global_server_config = NULL;
	}
#ifdef MS_WINDOWS
	/*close server*/
	ms_iocp_server_stop();
	if(g_hReceiveMsgThread != NULL)
	{
		CloseHandle(g_hReceiveMsgThread);
	}
	if(g_hSendMsgThread != NULL)
	{
		CloseHandle(g_hSendMsgThread);
	}
#endif
	/*close http server*/
	end_http_service();
	//close log
	moon_log_close();
}


#ifdef __cplusplus
}
#endif