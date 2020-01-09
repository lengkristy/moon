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

	extern MemoryPool *g_global_mp;//内存池

	extern Queue* p_global_send_msg_queue;//发送消息队列

	_global_ long long l_global_send_msg_total = 0;//发送消息的总量

	_global_ long long l_global_receive_msg_total = 0;//接收消息的总量

//////////////////////////////////////////////////////////////////消息计数器/////////////////////////////////////////////////////////////////

	/**
	 * 函数说明：
	 *   发送消息队列回调事件
	 */
    int send_msg_queue_event(void* queue,void* data)
	{
		l_global_send_msg_total++;
		return 0;
	}

	/**
	 * 函数说明：
	 *   接收消息队列回调事件
	 */
    int receive_msg_queue_event(void* queue,void* data)
	{
		l_global_receive_msg_total++;
		return 0;
	}

//////////////////////////////////////////////////////////////////接收消息处理线程///////////////////////////////////////////////////////////

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
		SendMsg* send_msg = NULL;
		moon_char msgid[50] = {0};
		moon_char tmp[100] = {0};
		moon_char* msg = NULL;
		int msglen = PKG_BYTE_MAX_LENGTH;//消息初始大小
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
				char_to_moon_char("{\"message_head\":{\"msg_id\":\"%s\",\"main_msg_num\":%ld,\"sub_msg_num\":%ld},\"message_body\":{\"content\":[ ",tmp);
				moon_sprintf(msg,tmp,msgid,SYS_MAIN_PROTOCOL_SCI,SYS_SUB_PROTOCAL_ALL_CLIENBT_LIST_OK);
				memset(tmp,0,100);
				client_count = get_socket_context_count();
#ifdef MS_WINDOWS
				for(index = 0;index < client_count;index++)
				{
					p_socket_context = get_socket_context_by_index(index);
					if(p_socket_context != NULL)
					{
						if((moon_char_memory_size(msg) + 3 * moon_char_memory_size(((MS_SOCKET_CONTEXT *)p_socket_context)->m_client_id)) > msglen)
						{
							//先发送到客户端，然后再发送后面的信息
							//去掉最后一个逗号
							if(client_count > 0)
							{
								msg[moon_char_length(msg) - 1] = L'\0';
							}
							char_to_moon_char("]}}",tmp);
							moon_char_concat(msg,tmp);
							memset(tmp,0,100);
							utf8_msg = (char*)moon_malloc(PKG_BYTE_MAX_LENGTH);
							moon_ms_windows_moonchar_to_utf8(msg,utf8_msg);
							//发送消息
							send_msg = (SendMsg*)moon_malloc(sizeof(SendMsg));
							memset(send_msg,0,sizeof(SendMsg));
							send_msg->length = msglen;
							moon_char_copy(send_msg->send_client_id,client_id);
							send_msg->utf8_msg_buf = utf8_msg;
							//将发送的消息丢入队列
							Queue_AddToHead(p_global_send_msg_queue,send_msg);
							
							//
							memset(tmp,0,100);
							memset(msg,0,PKG_BYTE_MAX_LENGTH);
							char_to_moon_char("{\"message_head\":{\"msg_id\":\"%s\",\"main_msg_num\":%ld,\"sub_msg_num\":%ld},\"message_body\":{\"content\":[ ",tmp);
							moon_sprintf(msg,tmp,msgid,SYS_MAIN_PROTOCOL_SCI,SYS_SUB_PROTOCAL_ALL_CLIENBT_LIST_OK);
							continue;
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
				utf8_msg = (char*)moon_malloc(PKG_BYTE_MAX_LENGTH);
				moon_ms_windows_moonchar_to_utf8(msg,utf8_msg);
				//发送消息
				send_msg = (SendMsg*)moon_malloc(sizeof(SendMsg));
				memset(send_msg,0,sizeof(SendMsg));
				send_msg->length = msglen;
				moon_char_copy(send_msg->send_client_id,client_id);
				send_msg->utf8_msg_buf = utf8_msg;
				//将发送的消息丢入队列
				Queue_AddToHead(p_global_send_msg_queue,send_msg);
				//释放内存
				if(msg != NULL)
				{
					moon_free(msg);
					msg = NULL;
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
		SendMsg* send_msg = NULL;
		int len = 0;
		moon_char* client_msg = NULL;
		Message* p_msg = NULL;
		len = sizeof(moon_char) * (strlen(utf8_package) + 1);
		client_msg = (moon_char*)moon_malloc(len);
		memset(client_msg,0,len);
		if (client_msg == NULL)
		{
			return;
		}

#ifdef MS_WINDOWS
		len = moon_ms_windows_utf8_to_unicode(utf8_package,client_msg);//将收到的utf-8的字节序转化为moon_char
#endif

		p_msg = parse_msg_head(client_msg);

		
		if (p_msg == NULL || p_msg->p_message_head == NULL)
		{
			moon_free(client_msg);
			free_msg(p_msg);
			client_msg = NULL;
			p_msg = NULL;
			return;
		}
		
		
		//发送一条接收成功的消息
		create_server_receive_message_rely(p_msg->p_message_head->msg_id,true,reply_msg);
		
#ifdef MS_WINDOWS
		//转成utf8编码
		len = moon_ms_windows_moonchar_to_utf8(reply_msg,utf8_reply_msg);
#endif
		
		
		if(p_msg->p_message_head->client_id != NULL && moon_char_length(p_msg->p_message_head->client_id) > 0 && utf8_reply_msg != NULL && strlen(utf8_reply_msg) > 0 && len > 0)
		{
			send_msg = (SendMsg*)moon_malloc(sizeof(SendMsg));
			memset(send_msg,0,sizeof(SendMsg));
			send_msg->length = len;
			moon_char_copy(send_msg->send_client_id,p_msg->p_message_head->client_id);
			send_msg->utf8_msg_buf = (char*)moon_malloc(len + 1);
			memset(send_msg->utf8_msg_buf,0,len+1);
			memcpy(send_msg->utf8_msg_buf,utf8_reply_msg,len);
			//将发送的消息丢入队列
			Queue_AddToHead(p_global_send_msg_queue,send_msg);
		}
		
		//判断消息类型，处理消息，后期采用消息队列的方式处理
		switch(p_msg->p_message_head->main_msg_num)
		{
		case SYS_MAIN_PROTOCOL_MSG_POINT_TO_POINT:
			//msg_handler_ptp(p_msg->p_message_head->client_id,client_msg);
			break;
		case SYS_MAIN_PROTOCOL_SCI:
			{
				//msg_handler_sci(p_msg->p_message_head->client_id,p_msg);
			}
			break;
		default:
			break;
		}
		


		//销毁资源
		if(p_msg != NULL)
		{
			free_msg(p_msg);
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
	DWORD WINAPI msg_receive_handle_thread(LPVOID lpParam)
	{
		Queue* p_msg_queue = (Queue* )lpParam;
		char* utf8_package = NULL;
		while(true)
		{
			if(p_msg_queue->length > 0)
			{
				utf8_package = (char*)Queue_GetFromTail(p_msg_queue);
				if(utf8_package != NULL && strlen(utf8_package) > 0)
				{
					msg_handler(utf8_package,strlen(utf8_package));
					moon_free(utf8_package);
				}
			}
			Sleep(1);
		}
		return 0;
	}
#endif


/////////////////////////////////////////////////////////////////////////////发送消息处理///////////////////////////////////////////////////////////////////

#ifdef MS_WINDOWS
	/**
	 * 函数说明：
	 *     消息发送队列处理线程
	 */
	DWORD WINAPI msg_send_handle_thread(LPVOID lpParam)
	{
		Queue* p_msg_queue = (Queue* )lpParam;
		while(true)
		{
			if(p_msg_queue->length > 0)
			{
				SendMsg* sendMsg = (SendMsg*)Queue_GetFromTail(p_msg_queue);
				if(sendMsg != NULL && sendMsg->length > 0)
				{
					moon_server_send_msg(sendMsg->send_client_id,sendMsg->utf8_msg_buf,sendMsg->length);
					moon_free(sendMsg->utf8_msg_buf);
					moon_free(sendMsg);
				}
			}
			Sleep(2);
		}
		return 0;
	}
#endif

#ifdef __cplusplus
}
#endif