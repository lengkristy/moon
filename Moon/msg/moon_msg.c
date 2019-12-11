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
 *	parse the message head on the network
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
Message* parse_msg_head(moon_char* str_message)
{
	cJSON * root = NULL;
	Message* p_message = NULL;
	char* ascii_msg = NULL;
	int length = moon_char_length(str_message);
	int size = length * 2 + 2;
	int len = 0;
	moon_char msg_id[50] = {0};
	cJSON *head = NULL;
	//cJSON *body = NULL;
	cJSON *item = NULL;
	//char* p_str_body = NULL;
	//moon_char *p_msg_body = NULL;
	//int msg_body_length = 0;
	ascii_msg = (char*)moon_malloc(size * sizeof(char));
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
			if (item != NULL && !stringIsEmpty(item->valuestring))
			{
				char_to_moon_char(item->valuestring,msg_id);
				moon_char_copy(p_message->p_message_head->msg_id,msg_id);
			}
			item = cJSON_GetObjectItem(head,"main_msg_num");
			if (item != NULL )
			{
				p_message->p_message_head->main_msg_num = item->valueint;
			}
			item = cJSON_GetObjectItem(head,"sub_msg_num");
			if (item != NULL)
			{
				p_message->p_message_head->sub_msg_num = item->valueint;
			}
		}
		//body = cJSON_GetObjectItem(root,"message_body");
		/*if (body != NULL)
		{
			item = cJSON_GetObjectItem(body,"content");
			if (item != NULL && !stringIsEmpty(item->valuestring))
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
		}*/
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

/**
 * @desc parse client running environment message
 * @param msgData:message body
 * @return if success return ClientEnvironment struct pointer,otherwise return NULL
 **/
ClientEnvironment* parse_client_running_environment(moon_char* msgData)
{
	ClientEnvironment* p_client_env = NULL;
	cJSON * root = NULL;
	cJSON * body = NULL;
	cJSON * item = NULL;
	char* ascii_msg;
	int msg_body_length = 0;
	int length = moon_char_length(msgData);
	int size = length * 2 + 2;
	ascii_msg = (char*)moon_malloc(size);
	memset(ascii_msg,0,size);
	moon_ms_windows_unicode_to_ascii(msgData,ascii_msg);
	root = cJSON_Parse(ascii_msg);
	if (root == NULL)
	{
		moon_free(ascii_msg);
		return NULL;
	}
	if (root->type == cJSON_Object)
	{
		body = cJSON_GetObjectItem(root,"message_body");
		if(body == NULL)
		{
			return NULL;
		}
		body = cJSON_GetObjectItem(body,"content");
		if(body == NULL)
		{
			return NULL;
		}
		p_client_env = (ClientEnvironment*)moon_malloc(sizeof(ClientEnvironment));
		memset(p_client_env,0,sizeof(ClientEnvironment));
		item = cJSON_GetObjectItem(body,"client_sdk_version");
		if (item != NULL && !stringIsEmpty(item->valuestring))
		{
			strcpy(p_client_env->client_sdk_version,item->valuestring);
		}
		item = cJSON_GetObjectItem(body,"client_platform");
		if (item != NULL && !stringIsEmpty(item->valuestring))
		{
			strcpy(p_client_env->client_platform,item->valuestring);
		}
		item = cJSON_GetObjectItem(body,"opra_system_version");
		if (item != NULL && !stringIsEmpty(item->valuestring))
		{
			strcpy(p_client_env->opra_system_version,item->valuestring);
		}
		item = cJSON_GetObjectItem(body,"connect_sdk_token");
		if (item != NULL && !stringIsEmpty(item->valuestring))
		{
			strcpy(p_client_env->connect_sdk_token,item->valuestring);
		}
	}
	moon_free(ascii_msg);
	return p_client_env;
}

/**
 * @desc parse client login id
 * @param msgData:message body
 * @param out_client_login_id:return client id
 **/
void parse_login_id(moon_char* msgData,_out_ char* out_client_login_id)
{
	cJSON * root = NULL;
	cJSON * item = NULL;
	char* ascii_msg;
	int msg_body_length = 0;
	int length = moon_char_length(msgData);
	int size = length * 2 + 2;
	ascii_msg = (char*)moon_malloc(size);
	memset(ascii_msg,0,size);
	moon_ms_windows_unicode_to_ascii(msgData,ascii_msg);
	root = cJSON_Parse(ascii_msg);
	if (root == NULL)
	{
		moon_free(ascii_msg);
	}
	if (root->type == cJSON_Object)
	{
		item = cJSON_GetObjectItem(root,"id");
		if (item != NULL && !stringIsEmpty(item->valuestring))
		{
			strcpy(out_client_login_id,item->valuestring);
		}
	}
	moon_free(ascii_msg);
}

/**
 * @desc create a login-successful message json data
 * @param out_login_suc_msg:return json data
 **/
void create_client_login_success_msg(_out_ moon_char* out_login_suc_msg)
{
	char* msg = "{\"message_head\":{\"msg_id\":\"%s\",\"main_msg_num\":%ld,\"sub_msg_num\":%ld},\"message_body\":{\"content\":\"login successful\"}}";
	char tmp_msg[512] = {0};
	moon_char msg_id[50] = {0};
	char cmsg_id[50] = {0};
	create_message_id(msg_id);
	moon_char_to_char(msg_id,cmsg_id);
	sprintf(tmp_msg,msg,cmsg_id,SYS_MAIN_PROTOCOL_LOGIN,SYS_SUB_PROTOCOL_LOGIN_SUCCESS);
	char_to_moon_char(tmp_msg,out_login_suc_msg);
}

/**
 * @desc create a login-failed message json data
 * @param out_login_failed_msg:return json data
 **/
void create_client_login_failed_msg(char* err_msg,_out_ moon_char* out_login_failed_msg)
{
	char* msg = "{\"message_head\":{\"msg_id\":\"%s\",\"main_msg_num\":%ld,\"sub_msg_num\":%ld},\"message_body\":{\"content\":\"%s\"}}";
	char tmp_msg[512] = {0};
	moon_char msg_id[50] = {0};
	char cmsg_id[50] = {0};
	create_message_id(msg_id);
	moon_char_to_char(msg_id,cmsg_id);
	sprintf(tmp_msg,msg,cmsg_id,SYS_MAIN_PROTOCOL_LOGIN,SYS_SUB_PROTOCOL_LOGIN_SUCCESS,err_msg);
	char_to_moon_char(tmp_msg,out_login_failed_msg);
}

#ifdef __cplusplus
}
#endif