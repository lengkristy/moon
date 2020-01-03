#include "../cfg/environment.h"
#include "server.h"
#include "../module/module_log.h"
#include "../module/module_cfg.h"
#include "../module/moon_memory_pool.h"
#include "ms_nt__iocp.h"
#include "../module/moon_http_service.h"
#include "socket_context_manager.h"
#include "../collection/queue.h"
#include "../msg/moon_msg_handle.h"
#include "../module/moon_string.h"
#ifdef MS_WINDOWS
#include "ms_socket_context.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif

Moon_Server_Config* p_global_server_config = NULL;/*global server config*/
bool b_config_load_finish = false;//config has inited

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
		p_global_server_config = (Moon_Server_Config*)moon_malloc(sizeof(Moon_Server_Config));
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
}


/**
 * 函数说明：
 *    向客户端发送消息
 * 参数：
 *	  client_id：客户端id
 *	  utf8_send_buf：utf8编码消息
 *    len:消息长度
 */
void moon_server_send_msg(moon_char* client_id,char * utf8_send_buf,int len)
{
	char str_msg[128] = {0};
	char cid[50] = {0};
	MS_SOCKET_CONTEXT * psocket_context = NULL;
#ifdef MS_WINDOWS
	if(client_id != NULL && moon_char_length(client_id) > 0 && len > 0)
	{
		psocket_context = get_socket_context_by_client_id(client_id);
		if(psocket_context != NULL)
		{
			if(ms_iocp_send(psocket_context->m_socket,utf8_send_buf,len) == -1)
			{
				sprintf(str_msg,"the client id %s send mssage faild.the message :\r\n",cid,utf8_send_buf);
				moon_write_error_log(str_msg);
			}
		}
	}
#endif
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