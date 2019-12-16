#include "../cfg/environment.h"
#include "server.h"
#include "../module/module_log.h"
#include "../module/module_cfg.h"
#include "../module/moon_memory_pool.h"
#include "ms_nt__iocp.h"
#include "../module/moon_http_service.h"
#include "socket_context_manager.h"
#ifdef MS_WINDOWS
#include "ms_socket_context.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif

Moon_Server_Config* p_global_server_config = NULL;/*global server config*/
bool b_config_load_finish = false;//config has inited

/************************************************************************/
/* start server			                                                */
/************************************************************************/
void moon_start()
{
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
#endif

	//start http server
	lauch_http_service();
}


/**
 * ����˵����
 *    ��ͻ��˷�����Ϣ
 * ������
 *	  client_id���ͻ���id
 *	  utf8_send_buf��utf8������Ϣ
 *    len:��Ϣ����
 */
void moon_server_send_msg(moon_char* client_id,char * utf8_send_buf,int len)
{
#ifdef MS_WINDOWS
	if(client_id != NULL)
	{
		MS_SOCKET_CONTEXT * psocket_context = get_socket_context_by_client_id(client_id);
		if(psocket_context != NULL)
		{
			ms_iocp_send(psocket_context->m_socket,utf8_send_buf,len);
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
#endif
	/*close http server*/
	end_http_service();
	//close log
	moon_log_close();
}


#ifdef __cplusplus
}
#endif