#include "moon_msg_handle.h"
#include "../module/moon_memory_pool.h"
#include "../module/module_log.h"
#include "../module/moon_string.h"
#include "../core/server.h"
#include "../msg/moon_msg.h"
#include "../core/socket_context_manager.h"
#include "../collection/queue.h"

#ifdef __cplusplus
extern "C" {
#endif


	/**
	 * 函数说明：
	 *   处理获取服务器客户端信息
	 * 参数：
	 *   client_id：客户端id
	 *   pmsg：消息头
	 */
	void msg_handler_sci(moon_char* client_id,Message* pmsg)
	{
		char* utf8_msg = NULL;
		int index = 0;
		int client_count = 0;
		void* p_socket_context = NULL;
		moon_char msgid[50] = {0};
		moon_char tmp[100] = {0};
		moon_char* msg = NULL;
		int msglen = 512;//消息初始大小
		switch(pmsg->p_message_head->sub_msg_num)
		{
		case SYS_SUB_PROTOCOL_ALL_CLIENT_LIST://获取所有客户端列表
			{
				msg = (moon_char*)moon_malloc(msglen);
				if(msg == NULL)
				{
					return;
				}
				create_message_id(msgid);
				char_to_moon_char("{\"message_head\":{\"msg_id\":\"%s\",\"main_msg_num\":%ld,\"sub_msg_num\":%ld},\"message_body\":{\"content\":[",tmp);
				moon_sprintf(msg,tmp,msgid,SYS_MAIN_PROTOCOL_SCI,SYS_SUB_PROTOCAL_ALL_CLIENBT_LIST_OK);
				memset(tmp,0,100);
				client_count = get_socket_context_count();
#ifdef MS_WINDOWS
				for(index = 0;index < client_count;index++)
				{
					p_socket_context = get_socket_context_by_index(index);
					if(p_socket_context != NULL)
					{
						if((moon_char_memory_size(msg) + moon_char_memory_size(((MS_SOCKET_CONTEXT *)p_socket_context)->m_client_id)) > msglen)
						{
							//重新分配内存
							msg = (moon_char*)moon_realloc(msg,msglen + msglen);
							msglen += msglen;
						}
						char_to_moon_char("\"",tmp);
						moon_char_concat(msg,tmp);
						memset(tmp,0,100);
						moon_char_concat(msg,((MS_SOCKET_CONTEXT *)p_socket_context)->m_client_id);
						char_to_moon_char("\"",tmp);
						moon_char_concat(msg,tmp);
						memset(tmp,0,100);
						char_to_moon_char(",",tmp);
						moon_char_concat(msg,tmp);
						memset(tmp,0,100);

					}
				}
				//去掉最后一个逗号
				if(client_count > 0)
				{
					msg[moon_char_length(msg) - 1] = L'\0';
				}
#endif
				char_to_moon_char("]}}",tmp);
				moon_char_concat(msg,tmp);
				memset(tmp,0,100);
				msglen = moon_char_memory_size(msg) * 3 + 3;
				utf8_msg = (char*)moon_malloc(msglen);
				moon_ms_windows_moonchar_to_utf8(msg,utf8_msg);
				moon_server_send_msg(client_id,utf8_msg,msglen);
				//释放内存
				if(msg != NULL)
				{
					moon_free(msg);
					msg = NULL;
				}
				if(utf8_msg != NULL)
				{
					moon_free(utf8_msg);
					utf8_msg = NULL;
				}
			}
			break;
		default:
			break;
		}
	}


	/**
	 * 点对点消息
	 */
	void msg_handler_ptp(moon_char* client_id,moon_char* client_msg)
	{

	}

	/**
	 * 函数说明：
	 *    对接收的客户端的JSON包进行处理
	 * 参数：
	 *    client_id:客户端id
	 *    utf8_package:JSON数据包
	 *	  size:包大小
	 */
	void msg_handler(char* utf8_package,int size)
	{
		moon_char reply_msg[512] = {0};
		char utf8_reply_msg[512] = {0};
		int len = 0;
		moon_char* client_msg = NULL;
		Message* p_msg = NULL;
		size = sizeof(moon_char) * size + sizeof(moon_char);
		client_msg = (moon_char*)moon_malloc(size);
		if (client_msg == NULL)
		{
			moon_write_error_log("内存分配失败");
			return;
		}

#ifdef MS_WINDOWS
		len = moon_ms_windows_utf8_to_unicode(utf8_package,client_msg);//将收到的utf-8的字节序转化为moon_char
#endif

		p_msg = parse_msg_head(client_msg);

		if (p_msg == NULL)
		{
			moon_free(client_msg);
			client_msg = NULL;
			return;
		}
		//发送一条接收成功的消息
		create_server_receive_message_rely(p_msg->p_message_head->msg_id,true,reply_msg);
#ifdef MS_WINDOWS
		//转成utf8编码
		len = moon_ms_windows_moonchar_to_utf8(reply_msg,utf8_reply_msg);
#endif
		moon_server_send_msg(p_msg->p_message_head->client_id,utf8_reply_msg,len);
		if(client_msg != NULL)
		{
			moon_free(client_msg);
			client_msg = NULL;
		}

		//判断消息类型，处理消息，后期采用消息队列的方式处理
		switch(p_msg->p_message_head->main_msg_num)
		{
		case SYS_MAIN_PROTOCOL_MSG_POINT_TO_POINT:
			msg_handler_ptp(p_msg->p_message_head->client_id,client_msg);
			break;
		case SYS_MAIN_PROTOCOL_SCI:
			msg_handler_sci(p_msg->p_message_head->client_id,p_msg);
			break;
		default:
			break;
		}


		//销毁资源
		if(p_msg != NULL)
		{
			moon_free(p_msg);
			p_msg = NULL;
		}
		
		if(client_msg != NULL)
		{
			moon_free(client_msg);
			client_msg = NULL;
		}
	}

#ifdef MS_WINDOWS
	/**
	 * 函数说明：
	 *    消息队列处理线程
	 */
	DWORD WINAPI msg_handle_thread(LPVOID lpParam)
	{
		Queue* p_msg_queue = (Queue* )lpParam;
		char* utf8_package = NULL;
		while(true)
		{
			if(p_msg_queue->length > 0)
			{
				utf8_package = (char*)Queue_GetFromTail(p_msg_queue);
				msg_handler(utf8_package,strlen(utf8_package));
			}
			Sleep(10);
		}
		return 0;
	}
#endif

#ifdef __cplusplus
}
#endif