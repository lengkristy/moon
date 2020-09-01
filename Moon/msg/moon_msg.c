#include "moon_msg.h"
#include "../module/moon_config_struct.h"
#include "../module/cJSON.h"
#include "../module/moon_memory_pool.h"
#include "../module/moon_string.h"
#include "../module/moon_time.h"
#include <memory.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

extern moon_server_config* p_global_server_config;//global configuration struct


/**
 * 函数说明：
 *   创建消息
 * 参数说明：
 *   msg_id：消息id，在整个网络中流转的消息唯一标识，由服务节点名称+uuid构成
 *   msg_order：消息次序
 *   main_msg_num：主消息号
 *   sub_msg_num：子消息号
 *   msg_size：消息大小，单位字节
 *   msg_time：消息发送时间
 *   client_id：客户端id
 *   content：消息体内容
 *   msg_end：消息结束标志：解决多侦的问题，对于同一个消息id的消息如果消息没有传输完成，那么则为1，消息传输完成则为0
 *   msg：输出消息JSON包
 */
static void _create_moon_msg(moon_char* msg_id,int* msg_order,int main_msg_num,int sub_msg_num,int msg_size,moon_char* msg_time,
	moon_char* client_id,moon_char* content,int msg_end,_out_ moon_char* msg)
{
	char* p_msg_tmp = "{\"message_head\":{\"msg_id\":\"%s\",\"msg_order\":%ld,\"main_msg_num\":%ld,\"sub_msg_num\":%ld,\"msg_size\":%ld,\"msg_time\":\"%s\",\"client_id\":\"%s\",\"msg_end\":%ld},\"message_body\":{\"content\":\"%s\"}}";
	moon_char mn_msg[512] = {0};
	char_to_moon_char(p_msg_tmp,mn_msg);
	moon_sprintf(msg,mn_msg,msg_id,msg_order,main_msg_num,sub_msg_num,msg_size,msg_time,client_id,msg_end,content);
}


/**
 * 函数说明：
 *   判断在网络上获取的数据包是否是包头开始，数据报文的前8个字节为包头标识
 * 返回值：
 *   如果是返回true，否则返回false
 */
bool pkg_is_head(moon_char* pkg)
{
	moon_char head_flag[9] = {0};
	moon_char stand_head_flag[9] = {0};
	if(pkg == NULL)
	{
		return false;
	}
	if(moon_char_length(pkg) < PKG_HEAD_LENGTH)
	{
		return false;
	}
	head_flag[0] = pkg[0];
	head_flag[1] = pkg[1];
	head_flag[2] = pkg[2];
	head_flag[3] = pkg[3];
	head_flag[4] = pkg[4];
	head_flag[5] = pkg[5];
	head_flag[6] = pkg[6];
	head_flag[7] = pkg[7];
	char_to_moon_char(PKG_HEAD_FLAG,stand_head_flag);
	if(!moon_char_equals(head_flag,stand_head_flag))
	{
		return false;
	}
	return true;
}

/**
 * 函数说明：
 *   判断在网络上获取的数据包是否是以包尾结束，数据报文的后8个字节为包尾标识
 * 返回值：
 *   如果是返回true，否则返回false
 */
bool pkg_is_tail(moon_char* pkg)
{
	moon_char head_flag[9] = {0};
	moon_char stand_tail_flag[9] = {0};
	int length = moon_char_length(pkg);
	if(pkg == NULL)
	{
		return false;
	}
	if(length < PKG_HEAD_LENGTH)
	{
		return false;
	}
	head_flag[0] = pkg[length - 8];
	head_flag[1] = pkg[length - 7];
	head_flag[2] = pkg[length - 6];
	head_flag[3] = pkg[length - 5];
	head_flag[4] = pkg[length - 4];
	head_flag[5] = pkg[length - 3];
	head_flag[6] = pkg[length - 2];
	head_flag[7] = pkg[length - 1];
	char_to_moon_char(PKG_TAIL_FLAG,stand_tail_flag);
	if(!moon_char_equals(head_flag,stand_tail_flag))
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
int parse_pkg(moon_char* src_pkg,_out_ moon_char* out_data)
{
	unsigned int pkg_size = 0;

	moon_char head_size[9] = {0};

	unsigned int index = 0;

	moon_char* p = src_pkg;

	if(src_pkg == NULL)
	{
		return 0;
	}
	if(moon_char_length(src_pkg) < PKG_HEAD_LENGTH)
	{
		return 0;
	}
	head_size[0] = src_pkg[8];
	head_size[1] = src_pkg[9];
	head_size[2] = src_pkg[10];
	head_size[3] = src_pkg[11];
	
	if(!moon_string_parse_to_int((char*)head_size,&pkg_size))
	{
		return 0;
	}
	//从第8个字节开始读取，一直读pkg_size个字节
	p = p + PKG_HEAD_LENGTH;
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
	int size = (length + 1) * MOON_CHAR_SIZE;
	moon_char msg_id[50] = {0};
	cJSON *head = NULL;
	cJSON *item = NULL;
	ascii_msg = (char*)moon_malloc(size);
	if(ascii_msg == NULL)
	{
		return NULL;
	}

	moon_char_to_char(str_message,ascii_msg);
	root = cJSON_Parse(ascii_msg);
	if (root == NULL)
	{
		moon_free(ascii_msg);
		return NULL;
	}
	if (root->type == cJSON_Object)
	{
		p_message = (Message*)moon_malloc(sizeof(struct _Message));
		if(p_message == NULL)
		{
			moon_free(ascii_msg);
			return NULL;
		}
		memset(p_message,0,sizeof(struct _Message));
		p_message->p_message_head = (MessageHead*)moon_malloc(sizeof(struct _MessageHead));
		if(p_message->p_message_head == NULL)
		{
			moon_free(ascii_msg);
			moon_free(p_message);
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
 *	create message id,the rule as server_node_name + 32bit random number,eg:node_one-7a06ed8e8df94e04981a45ae5adc5020
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
bool parse_client_running_environment(moon_char* msgData,_out_ client_environment* p_client_env)
{
	cJSON * root = NULL;
	cJSON * body = NULL;
	cJSON * item = NULL;
	char* ascii_msg;
	int msg_body_length = 0;
	int length = moon_char_length(msgData);
	int size = (length + 1) * MOON_CHAR_SIZE;//
	ascii_msg = (moon_char*)moon_malloc(size);
	moon_char_to_char(msgData,ascii_msg);
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
			char_to_moon_char(item->valuestring,p_client_env->client_sdk_version);
		}
		item = cJSON_GetObjectItem(body,"client_platform");
		if (item != NULL && !stringIsEmpty(item->valuestring))
		{
			char_to_moon_char(item->valuestring,p_client_env->client_platform);
		}
		item = cJSON_GetObjectItem(body,"opra_system_version");
		if (item != NULL && !stringIsEmpty(item->valuestring))
		{
			char_to_moon_char(item->valuestring,p_client_env->opra_system_version);
		}
		item = cJSON_GetObjectItem(body,"connect_sdk_token");
		if (item != NULL && !stringIsEmpty(item->valuestring))
		{
			char_to_moon_char(item->valuestring,p_client_env->connect_sdk_token);
		}
		item = cJSON_GetObjectItem(body,"client_id");
		if (item != NULL && !stringIsEmpty(item->valuestring))
		{
			char_to_moon_char(item->valuestring,p_client_env->client_id);
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
void parse_login_id(moon_char* msgData,_out_ moon_char* out_client_login_id)
{
	cJSON * root = NULL;
	cJSON * item = NULL;
	char* ascii_msg;
	int msg_body_length = 0;
	int length = moon_char_length(msgData);
	int size = (length + 1) * sizeof(moon_char);
	ascii_msg = (char*)moon_malloc(size);
	memset(ascii_msg,0,size);
	moon_ms_windows_utf8_to_ascii(msgData,ascii_msg);
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
			char_to_moon_char(item->valuestring,out_client_login_id);
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
	char cc_msg_id[50] = {0};
	moon_char cs_msg_id[50] = {0};
	char cs_msg_id_tmp[50] = {0};
	int sub_protocol = 0;
	if(success)
	{
		sub_protocol = MN_PROTOCOL_SUB_REPLY_OK;
	}
	else
	{
		sub_protocol = MN_PROTOCOL_SUB_REPLY_FAILED;
	}
	create_message_id(cs_msg_id);
	moon_char_to_char(msg_id,cc_msg_id);
	moon_char_to_char(cs_msg_id,cs_msg_id_tmp);
	sprintf(tmp_msg,msg,cs_msg_id_tmp,MN_PROTOCOL_MAIN_REPLY,sub_protocol,cc_msg_id);
	char_to_moon_char(tmp_msg,out_reply_msg);
}


/**
 * 函数说明：
 *    校验数据包是否有效
 * 参数说明：
 *    utf8_package：utf8的数据报文
 * 返回值：
 *    如果有效则返回true，如果无效则返回false
 */
bool check_data_package_valid(moon_char* utf8_package)
{
	unsigned int pkg_size = 0;

	moon_char head_size[5] = {0};
	char head_size_ch[5] = {0};

	unsigned int index = 0;

	int pos_pkg_head = PKG_HEAD_LENGTH;

	if(utf8_package == NULL || moon_char_length(utf8_package) < (PKG_TAIL_LENGTH + PKG_HEAD_LENGTH))
	{
		return false;
	}

	//前12个字节为包头
	if (!pkg_is_head(utf8_package)) return false;
	//后8个字节为包尾
	if(!pkg_is_tail(utf8_package)) return false;

	//取出包大小进行校验，防止数据包被串改，第8-11个字节为包大小
	head_size[0] = utf8_package[8];
	head_size[1] = utf8_package[9];
	head_size[2] = utf8_package[10];
	head_size[3] = utf8_package[11];

	moon_char_to_char(head_size,head_size_ch);

	if(!moon_string_parse_to_int(head_size_ch,&pkg_size))
	{
		return false;
	}
	//去掉包头和包尾
	memset(utf8_package + sizeof(moon_char) * moon_char_length(utf8_package) - PKG_TAIL_LENGTH,0,PKG_TAIL_LENGTH);
	for (index = 0; !moon_string_is_empty(utf8_package+pos_pkg_head);index++,pos_pkg_head++)
	{
		utf8_package[index] = utf8_package[pos_pkg_head];
	} 
	//然后将后面的清空
	memset(utf8_package + index,0,sizeof(moon_char) * moon_char_length(utf8_package) - index);
	if(sizeof(moon_char) *moon_char_length(utf8_package) != pkg_size) return false;
	return true;
}

/**
 * 函数说明：
 *   创建接受客户端连接的消息
 * 参数说明：
 *   client_id：客户端id
 *   out_accept_msg：输出接受客户端消息的json utf8字符串
 */
void create_message_accept_client(_in_ moon_char* client_id,_out_ moon_char* out_accept_msg)
{
	 moon_char msg_id[50] = {0};
	 moon_char msg_time[30] = {0};
	 create_message_id(msg_id);
	 moon_utf8_current_time(msg_time);
	 _create_moon_msg(msg_id,0,MN_PROTOCOL_MAIN_CONNECT_INIT,MN_PROTOCOL_SUB_SERVER_ACCEPT,0,msg_time,client_id,"",0,out_accept_msg);
}

#ifdef __cplusplus
}
#endif