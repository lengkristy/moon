#include "moon_msg.h"
#include "../module/moon_config_struct.h"
#include "../module/cJSON.h"
#include "../module/moon_memory_pool.h"
#include "../module/moon_string.h"
#include <memory.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

extern Moon_Server_Config* p_global_server_config;//global configuration struct

/**
 * function desc:
 *	parse the message on the network
 * params:
 *	str_message:the message string of json,like 
    {
	 "message_head": {
	 "msg_id": "node_one-3E18611A3D8B4A97A6027495347C45A4",
	 "main_msg_num": 10001,
	 "sub_msg_num": 10003
	 },
	 "message_body": {
		"content": "{\r\n  \"client_sdk_version\": null,\r\n  \"client_platform\": null,\r\n  \"opra_system_version\": null,\r\n  \"connect_sdk_token\": null\r\n}"
	 }
	 }
 * return:
 *	if success return the pointer of Message struct,otherwise return null
 */
Message* parse_msg(moon_char* str_message)
{
	cJSON * root = NULL;
	Message* p_message = NULL;
	char* ascii_msg;
	int length = moon_char_length(str_message);
	int size = length * 2 + 2;
	int len = 0;
	moon_char msg_id[50] = {0};
	cJSON *head = NULL;
	cJSON *body = NULL;
	cJSON *item = NULL;
	char* p_str_body = NULL;
	moon_char *p_msg_body = NULL;
	int msg_body_length = 0;
	ascii_msg = (char*)moon_malloc(size);
	memset(ascii_msg,0,size);
	len = moon_ms_windows_unicode_to_ascii(str_message,ascii_msg);
	root = cJSON_Parse(ascii_msg);
	if (root == NULL)
	{
		moon_free(ascii_msg);
		return NULL;
	}
	if (root->type == cJSON_Object)
	{
		p_message = (Message*)moon_malloc(sizeof(Message));
		memset(p_message,0,sizeof(Message));
		p_message->p_message_head = (MessageHead*)moon_malloc(sizeof(MessageHead));
		memset(p_message->p_message_head,0,sizeof(MessageHead));
		p_message->p_message_body = (MessageBody*)moon_malloc(sizeof(MessageBody));
		memset(p_message->p_message_body,0,sizeof(MessageBody));
		//get header
		head = cJSON_GetObjectItem(root,"message_head");
		if (head != NULL)
		{
			item = cJSON_GetObjectItem(head,"msg_id");
			if (item != NULL)
			{
				char_to_moon_char(item->valuestring,msg_id);
				moon_char_copy(p_message->p_message_head->msg_id,msg_id);
			}
			item = cJSON_GetObjectItem(head,"main_msg_num");
			if (item != NULL)
			{
				p_message->p_message_head->main_msg_num = item->valueint;
			}
			item = cJSON_GetObjectItem(head,"sub_msg_num");
			if (item != NULL)
			{
				p_message->p_message_head->sub_msg_num = item->valueint;
			}
		}
		body = cJSON_GetObjectItem(root,"message_body");
		if (body != NULL)
		{
			item = cJSON_GetObjectItem(body,"content");
			if (item != NULL)
			{
				msg_body_length = moon_get_string_length(item->valuestring);
				p_str_body = (char*)moon_malloc(msg_body_length + 1);
				memset(p_str_body,0,msg_body_length + 1);
				strcpy(p_str_body,item->valuestring);
				p_msg_body = (moon_char*)moon_malloc((msg_body_length + 1) * 2);
				memset(p_msg_body,0,(msg_body_length + 1) * 2);
				char_to_moon_char(p_str_body,p_msg_body);
				p_message->p_message_body->p_content = p_msg_body;
				moon_free(p_str_body);
			}
		}
	}
	cJSON_Delete(root);
	moon_free(ascii_msg);
	return p_message;
}

/**
 * release message memory
 */
void free_msg(Message* p_message)
{
	if (p_message != NULL)
	{
		if (p_message->p_message_body != NULL && p_message->p_message_body->p_content != NULL)
		{
			moon_free(p_message->p_message_body->p_content);
			p_message->p_message_body->p_content = NULL;
		}
		if (p_message->p_message_body != NULL)
		{
			moon_free(p_message->p_message_body);
			p_message->p_message_body = NULL;
		}
		if (p_message->p_message_head != NULL)
		{
			moon_free(p_message->p_message_head);
			p_message->p_message_head = NULL;
		}
		if (p_message != NULL)
		{
			moon_free(p_message);
			p_message = NULL;
		}
	}
}

/**
 * function desc:
 *	create message id,the rule as server_node_name + 32bit random number
 * params:
 *	out_msg_id:return message id
 */
void create_message_id(_out_ moon_char* out_msg_id)
{
	char splite[2] = {'-'};
	moon_char moon_splite[2] = {0};
	moon_char uuid[40] = {0};
	moon_char server_node_name[255] = {0};
	if (out_msg_id == NULL)
	{
		return;
	}
	moon_create_32uuid(uuid);
	char_to_moon_char(p_global_server_config->server_node_name,server_node_name);
	char_to_moon_char(splite,moon_splite);
	moon_char_copy(out_msg_id,server_node_name);
	moon_char_concat(out_msg_id,moon_splite);
	moon_char_concat(out_msg_id,uuid);
}

#ifdef __cplusplus
}
#endif