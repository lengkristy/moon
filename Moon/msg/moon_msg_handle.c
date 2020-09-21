#include "moon_msg_handle.h"
#include "../module/moon_memory_pool.h"
#include "../module/module_log.h"
#include "../module/moon_string.h"
#include "../core/server.h"
#include "../msg/moon_msg.h"
#include "../core/moon_session_manager.h"
#include "../collection/queue.h"
#include "../module/moon_time.h"

#ifdef __cplusplus
extern "C" {
#endif

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
	void msg_handler_sci(moon_char* client_id,moon_message* pmsg)
	{
		moon_char* utf8_msg = NULL;
		int index = 0;
		int client_count = 0;
		moon_session* p_moon_session = NULL;
		msg_send* send_msg = NULL;
		moon_char msgid[50] = {0};
		moon_char tmp[200] = {0};
		moon_char* msg = NULL;
		int msglen = DATA_BYTE_MAX_LENGTH;//消息初始大小
		int msg_order = 0;//消息次序
		moon_char msg_time[40] = {0};//消息发送时间
		char ch_current_time[40] = {0};//当前时间
		switch(pmsg->p_message_head->sub_msg_num)
		{
		case MN_PROTOCOL_SUB_ALL_CLIENT_LIST://获取所有客户端列表
			{
				
				msg = (char*)moon_malloc(msglen);
				if(msg == NULL)
				{
					return;
				}
				create_message_id(msgid);
				moon_current_time(ch_current_time);
				char_to_moon_char(ch_current_time,msg_time);
				char_to_moon_char("{\"message_head\":{\"msg_id\":\"%s\",\"main_msg_num\":%ld,\"sub_msg_num\":%ld,\"msg_order\":%ld,\"msg_time\":\"%s\"}, \"message_body\":{\"content\":[ ",tmp);
				moon_sprintf(msg,tmp,msgid,MN_PROTOCOL_MAIN_SCI,MN_PROTOCAL_SUB_ALL_CLIENBT_LIST_OK,msg_order,msg_time);
				memset(tmp,0,200);
				client_count = get_socket_context_count();
#ifdef MS_WINDOWS
				for(index = 0;index < client_count;index++)
				{
					p_moon_session = get_moon_session_by_index(index);
					if(p_moon_session != NULL && p_moon_session->p_client_environment != NULL)
					{
						if((moon_char_memory_size(msg) + 3 * moon_char_memory_size(p_moon_session->p_client_environment->client_id)) > msglen)
						{
							//先发送到客户端，然后再发送后面的信息
							//去掉最后一个逗号
							if(client_count > 0)
							{
								msg[moon_char_length(msg) - 1] = '\0';
							}
							char_to_moon_char("]}}",tmp);
							moon_char_concat(msg,tmp);
							memset(tmp,0,200);
							utf8_msg = (char*)moon_malloc(msglen);
							moon_char_copy(utf8_msg,msg);
							//发送消息
							send_msg = (msg_send*)moon_malloc(sizeof(msg_send));
							moon_char_copy(send_msg->send_client_id,client_id);
							send_msg->utf8_msg_buf = utf8_msg;
							send_msg->size = sizeof(moon_char) * moon_char_length(send_msg->utf8_msg_buf);
							//将发送的消息丢入队列
							Queue_AddToHead(p_global_send_msg_queue,send_msg);
							msg_order++;
							
							//
							memset(tmp,0,200);
							memset(msg,0,msglen);
							moon_current_time(ch_current_time);
							char_to_moon_char(ch_current_time,msg_time);
							char_to_moon_char("{\"message_head\":{\"msg_id\":\"%s\",\"main_msg_num\":%ld,\"sub_msg_num\":%ld,\"msg_order\":%ld,\"msg_time\":\"%s\"},\"message_body\":{\"content\":[ ",tmp);
							moon_sprintf(msg,tmp,msgid,MN_PROTOCOL_MAIN_SCI,MN_PROTOCAL_SUB_ALL_CLIENBT_LIST_OK,msg_order,msg_time);
						}
						
						char_to_moon_char("\"",tmp);
						moon_char_concat(msg,tmp);
						memset(tmp,0,200);
						moon_char_concat(msg,p_moon_session->p_client_environment->client_id);
						char_to_moon_char("\"",tmp);
						moon_char_concat(msg,tmp);
						memset(tmp,0,200);
						char_to_moon_char(",",tmp);
						moon_char_concat(msg,tmp);
						memset(tmp,0,200);
					}
				}
				//去掉最后一个逗号
				if(client_count > 0)
				{
					msg[moon_char_length(msg) - 1] = '\0';
				}
#endif
				char_to_moon_char("]}}",tmp);
				moon_char_concat(msg,tmp);
				memset(tmp,0,200);
				utf8_msg = (char*)moon_malloc(msglen);
				moon_char_copy(utf8_msg,msg);
				//发送消息
				send_msg = (msg_send*)moon_malloc(sizeof(msg_send));
				moon_char_copy(send_msg->send_client_id,client_id);
				send_msg->utf8_msg_buf = utf8_msg;
				send_msg->size = sizeof(moon_char) * moon_char_length(send_msg->utf8_msg_buf);
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
	void msg_handler_ptp(message_head* p_message_head,moon_char* client_msg)
	{
		//解析消息体，获取发向对象的客户端id
		ptp_message_body* p_ptp_msg_body = NULL;
		msg_send* send_msg = NULL;
		p_ptp_msg_body = (ptp_message_body*)moon_malloc(sizeof(ptp_message_body));
		parse_ptp_message_body(client_msg,p_ptp_msg_body);
		if (!moon_string_is_empty(p_ptp_msg_body->to_client_id))
		{
			//构建发送的消息
			send_msg = (msg_send*)moon_malloc(sizeof(msg_send));
			moon_char_copy(send_msg->send_client_id,p_ptp_msg_body->to_client_id);
			send_msg->size = moon_char_memory_size(client_msg);
			send_msg->utf8_msg_buf = (moon_char*)moon_malloc(moon_char_memory_size(client_msg));
			moon_char_copy(send_msg->utf8_msg_buf,client_msg);
			//将发送的消息丢入队列
			Queue_AddToHead(p_global_send_msg_queue,send_msg);
		}
		//释放资源
		moon_free(p_ptp_msg_body->p_content);
		moon_free(p_ptp_msg_body);
	}

	/**
	 * 函数说明：
	 *   处理广播消息
	 * 参数：
	 *   client_id：发送方的客户端id
	 *   client_msg：客户端原始消息
	 */
	void handler_msg_broadcast(message_head* p_message_head,moon_char* client_msg)
	{
		user_broadcast_message_body* p_user_broadcast_msg_body = NULL;
		int index = 0;
		msg_send* send_msg = NULL;
		moon_char* send_client;
		if (p_message_head->sub_msg_num == MN_PROTOCOL_SUB_USER_TEXT_BROADCAST) //用户发送的群消息
		{
			p_user_broadcast_msg_body = (user_broadcast_message_body*)moon_malloc(sizeof(user_broadcast_message_body));
			if(p_user_broadcast_msg_body == NULL) return;
			parse_broadcast_message_body(client_msg,p_user_broadcast_msg_body);
			//判断有哪些客户端
			for (index = 0; index < p_user_broadcast_msg_body->p_to_client_ids->length;index++)
			{
				send_client = (moon_char*)array_list_getAt(p_user_broadcast_msg_body->p_to_client_ids,index);
				//构建发送的消息
				send_msg = (msg_send*)moon_malloc(sizeof(msg_send));
				moon_char_copy(send_msg->send_client_id,send_client);
				send_msg->size = moon_char_memory_size(client_msg);
				send_msg->utf8_msg_buf = (moon_char*)moon_malloc(moon_char_memory_size(client_msg));
				moon_char_copy(send_msg->utf8_msg_buf,client_msg);
				//将发送的消息丢入队列
				Queue_AddToHead(p_global_send_msg_queue,send_msg);
			}
		}
		if (p_user_broadcast_msg_body != NULL)
		{
			//释放资源
			for (index = 0; index < p_user_broadcast_msg_body->p_to_client_ids->length;index++)
			{
				send_client = (moon_char*)array_list_getAt(p_user_broadcast_msg_body->p_to_client_ids,index);
				moon_free(send_client);
			}
			array_list_free(p_user_broadcast_msg_body->p_to_client_ids);
			moon_free(p_user_broadcast_msg_body);
		}
	}

	/**
	 * 函数说明：
	 *    对接收的客户端的JSON包进行处理
	 * 参数：
	 *    package:JSON数据包
	 *    size:包大小
	 */
	void msg_handler(moon_char* package,int size)
	{
		moon_char reply_msg[512] = {0};
		//moon_char utf8_reply_msg[512] = {0};
		msg_send* send_msg = NULL;
		int len = 0;
		moon_char* client_msg = NULL;
		moon_message* p_msg = NULL;
		len = sizeof(moon_char) * (moon_char_length(package) + 1);
		client_msg = (moon_char*)moon_malloc(len);
		memset(client_msg,0,len);
		if (client_msg == NULL)
		{
			return;
		}

		moon_char_copy(client_msg,package);

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

		if(p_msg->p_message_head->client_id != NULL && moon_char_length(p_msg->p_message_head->client_id) > 0 && reply_msg != NULL && moon_char_length(reply_msg) > 0 && len > 0)
		{
			len = sizeof(moon_char) * moon_char_length(reply_msg);
			send_msg = (msg_send*)moon_malloc(sizeof(msg_send));
			moon_char_copy(send_msg->send_client_id,p_msg->p_message_head->client_id);
			send_msg->utf8_msg_buf = (moon_char*)moon_malloc(len + 1);
			memset(send_msg->utf8_msg_buf,0,len+1);
			memcpy(send_msg->utf8_msg_buf,reply_msg,len);
			send_msg->size = sizeof(moon_char) * moon_char_length(send_msg->utf8_msg_buf);
			//将发送的消息丢入队列
			Queue_AddToHead(p_global_send_msg_queue,send_msg);
		}
		
		//判断消息类型，处理消息，后期采用消息队列的方式处理
		switch(p_msg->p_message_head->main_msg_num)
		{
		case MN_PROTOCOL_MAIN_MSG_POINT_TO_POINT://点对点消息
			msg_handler_ptp(p_msg->p_message_head,client_msg);
			break;
		case MN_PROTOCOL_MAIN_SCI://获取服务节点的所有客户端列表
			{
				msg_handler_sci(p_msg->p_message_head->client_id,p_msg);
			}
			break;
		case MN_PROTOCOL_MAIN_BROADCAST://广播消息
			{
				handler_msg_broadcast(p_msg->p_message_head,client_msg);
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
		moon_char* utf8_package = NULL;
		while(true)
		{
			if(p_msg_queue->length > 0)
			{
				utf8_package = (moon_char*)Queue_GetFromTail(p_msg_queue);
				////进行消息包的校验，并且去掉包头和包尾
				if(check_data_package_valid(utf8_package))
				{
					msg_handler(utf8_package,strlen(utf8_package));
				}
				else
				{
					//无效的数据包，记录日志
					moon_write_error_log("无效的数据包：");
					moon_write_error_log(utf8_package);
				}
				moon_free(utf8_package);
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
				msg_send* msg_of_send = (msg_send*)Queue_GetFromTail(p_msg_queue);
				if(msg_of_send != NULL && msg_of_send->size > 0)
				{
					moon_server_send_msg(msg_of_send->send_client_id,msg_of_send->utf8_msg_buf,msg_of_send->size);
					moon_free(msg_of_send->utf8_msg_buf);
					moon_free(msg_of_send);
				}
			}
			Sleep(1);
		}
		return 0;
	}
#endif

#ifdef __cplusplus
}
#endif