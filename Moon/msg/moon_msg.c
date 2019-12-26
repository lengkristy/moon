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

extern MemoryPool *g_global_mp;//内存池


/**
 * 函数说明：
 *   判断在网络上获取的数据包是否是包头开始，数据报文的前4个字节为包头标识
 * 返回值：
 *   如果是返回true，否则返回false
 */
bool pkg_is_head(char* pkg)
{
	char head_flag[5] = {0};
	if(pkg == NULL)
	{
		return false;
	}
	if(strlen(pkg) < PKG_HEAD_LENGTH)
	{
		return false;
	}
	head_flag[0] = pkg[0];
	head_flag[1] = pkg[1];
	head_flag[2] = pkg[2];
	head_flag[3] = pkg[3];
	if(strcmp(head_flag,PKG_HEAD_FLAG) != 0)
	{
		return false;
	}
	return true;
}

/**
 * 函数说明：
 *   解析数据包，将包头后面的数据解析出来
 * 参数：
 *   src_pkg：原始数据
 *   out_data：解析后得到的业务JSON数据
 * 返回值：
 *   返回该包的大小
 */
int parse_pkg(char* src_pkg,_out_ char* out_data)
{
	unsigned int pkg_size = 0;

	char head_size[5] = {0};

	unsigned int index = 0;

	char* p = src_pkg;

	if(src_pkg == NULL)
	{
		return 0;
	}
	if(strlen(src_pkg) < PKG_HEAD_LENGTH)
	{
		return 0;
	}
	head_size[0] = src_pkg[4];
	head_size[1] = src_pkg[5];
	head_size[2] = src_pkg[6];
	head_size[3] = src_pkg[7];
	
	if(!moon_string_parse_to_int(head_size,&pkg_size))
	{
		return 0;
	}
	//从第8个字节开始读取，一直读pkg_size个字节
	p = p + 8;
	for(index = 0;index < pkg_size;index++)
	{
		out_data[index] = p[index];
	}
	return pkg_size;
}

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
	int size = (length + 1) * sizeof(moon_char);
	int len = 0;
	moon_char msg_id[50] = {0};
	cJSON *head = NULL;
	cJSON *item = NULL;
	ascii_msg = (char*)malloc(size);
	if(ascii_msg == NULL)
	{
		return NULL;
	}

	len = moon_ms_windows_unicode_to_ascii(str_message,ascii_msg);
	root = cJSON_Parse(ascii_msg);
	if (root == NULL)
	{
		free(ascii_msg);
		return NULL;
	}
	if (root->type == cJSON_Object)
	{
		p_message = (Message*)malloc(sizeof(struct _Message));
		if(p_message == NULL)
		{
			free(ascii_msg);
			return NULL;
		}
		memset(p_message,0,sizeof(struct _Message));
		p_message->p_message_head = (MessageHead*)malloc(sizeof(struct _MessageHead));
		if(p_message->p_message_head == NULL)
		{
			free(ascii_msg);
			free(p_message);
			return NULL;
		}
		memset(p_message->p_message_head,0,sizeof(struct _MessageHead));
		p_message->p_message_body = NULL;
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
			item = cJSON_GetObjectItem(head,"client_id");
			if (item != NULL)
			{
				char_to_moon_char(item->valuestring,msg_id);
				moon_char_copy(p_message->p_message_head->client_id,msg_id);
			}
		}
	}
	cJSON_Delete(root);
	free(ascii_msg);
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
			free(p_message->p_message_body->p_content);
			p_message->p_message_body->p_content = NULL;
		}
		if (p_message->p_message_body != NULL)
		{
			free(p_message->p_message_body);
			p_message->p_message_body = NULL;
		}
		if (p_message->p_message_head != NULL)
		{
			free(p_message->p_message_head);
			p_message->p_message_head = NULL;
		}
		if (p_message != NULL)
		{
			free(p_message);
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
bool parse_client_running_environment(moon_char* msgData,_out_ ClientEnvironment* p_client_env)
{
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
		return false;
	}
	if (root->type == cJSON_Object)
	{
		body = cJSON_GetObjectItem(root,"message_body");
		if(body == NULL)
		{
			moon_free(ascii_msg);
			cJSON_Delete(root);
			return false;
		}
		body = cJSON_GetObjectItem(body,"content");
		if(body == NULL)
		{
			moon_free(ascii_msg);
			cJSON_Delete(root);
			return false;
		}
		
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
		item = cJSON_GetObjectItem(body,"client_id");
		if (item != NULL && !stringIsEmpty(item->valuestring))
		{
			strcpy(p_client_env->client_id,item->valuestring);
		}
	}
	moon_free(ascii_msg);
	cJSON_Delete(root);
	return true;
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
		cJSON_Delete(root);
		return;
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
	cJSON_Delete(root);
}


/**
 * 函数说明：
 *   创建一个服务端收到客户端消息的相应包
 * 参数：
 *   msg_id:消息id
 *   success:是否处理成功
 * 返回值：
 *  返回json数据包
 */
void create_server_receive_message_rely(moon_char* msg_id,bool success,_out_ moon_char* out_reply_msg)
{
	char* msg = "{\"message_head\":{\"msg_id\":\"%s\",\"main_msg_num\":%ld,\"sub_msg_num\":%ld},\"message_body\":{\"content\":\"%s\"}}";
	char tmp_msg[512] = {0};
	char c_msg_id[50] = {0};
	int sub_protocol = 0;
	if(success)
	{
		sub_protocol = SYS_SUB_PROTOCOL_REPLY_OK;
	}
	else
	{
		sub_protocol = SYS_SUB_PROTOCOL_REPLY_FAILD;
	}
	moon_char_to_char(msg_id,c_msg_id);
	sprintf(tmp_msg,msg,c_msg_id,SYS_MAIN_PROTOCOL_REPLY,sub_protocol,"");
	char_to_moon_char(tmp_msg,out_reply_msg);
}


#ifdef __cplusplus
}
#endif