/***************************************************************************************
 * this file used to deal with the messsage on the network
 * coding by haoran dai
 * time:2018-09-04 11:15
 **************************************************************************************/
#ifndef _MOON_MSG_H
#define _MOON_MSG_H
#include "moon_protocol.h"
#include "../module/moon_base.h"
#include "../cfg/environment.h"
#include "../core/moon_session.h"

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////
///////////////define message struct///////////////////////////

//define receive message head
typedef struct _MessageHead{
	moon_char msg_id[50];/*message id,represents the unique identity of the message throughout the message chain*/
	int main_msg_num;/*main message number*/
	int sub_msg_num;/*sub message number*/
	int msg_size;/*消息内容的大小，MessageBody中的p_content大小，由于网络传输消息过长会被截断，通过该字段进行组装*/
	moon_char client_id[50];/*client id*/
}MessageHead;

//define message body
typedef struct _MessageBody{
	moon_char* p_content;/*message content*/
}MessageBody;


//define message struct
typedef struct _Message{
	MessageHead *p_message_head;/*message head*/
	MessageBody *p_message_body;/*message body*/
}Message;

/**
 * 定义发送消息的结构体
 */
typedef struct _SendMsg{
	moon_char send_client_id[50];/*发送客户端id*/
	moon_char* utf8_msg_buf;/*utf8消息缓冲区*/
	int size;/*发送消息缓存区内存大小*/
}SendMsg;

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////

/**
 * 函数说明：
 *   判断在网络上获取的数据包是否是包头开始，数据报文的前8个字节为包头标识
 * 返回值：
 *   如果是返回true，否则返回false
 */
bool pkg_is_head(moon_char* pkg);

/**
 * 函数说明：
 *   判断在网络上获取的数据包是否是以包尾结束，数据报文的后8个字节为包尾标识
 * 返回值：
 *   如果是返回true，否则返回false
 */
bool pkg_is_tail(moon_char* pkg);

/**
 * 函数说明：
 *   解析数据包，将包头后面的数据解析出来
 * 参数：
 *   src_pkg：原始数据
 *   out_data：解析后得到的业务JSON数据
 * 返回值：
 *   返回该包的大小
 */
int parse_pkg(moon_char* src_pkg,_out_ moon_char* out_data);



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
Message* parse_msg_head(moon_char* str_message);


/**
 * release message memory
 */
void free_msg(Message* p_message);

/**
 * function desc:
 *	create message id,the rule as server_node_name + 32bit random number
 * params:
 *	out_msg_id:return message id
 */
void create_message_id(_out_ moon_char* out_msg_id);


/**
 * @desc parse client running environment message
 * @param msgData:message body
 * @return if success return ClientEnvironment struct pointer,otherwise return NULL
 **/
bool parse_client_running_environment(moon_char* msgData,_out_ client_environment* p_client_env);

/**
 * @desc parse client login id
 * @param msgData:message body
 * @param out_client_login_id:return client id
 **/
void parse_login_id(moon_char* msgData,_out_ moon_char* out_client_login_id);

/**
 * @desc create a login-successful message json data
 * @param out_login_suc_msg:return json data
 **/
void create_client_login_success_msg(_out_ moon_char* out_login_suc_msg);


/**
 * 函数说明：
 *   创建一个服务端收到客户端消息的相应包
 * 参数：
 *   msg_id:消息id
 *   success:是否处理成功
 * 返回值：
 *  返回json数据包
 */
void create_server_receive_message_rely(moon_char* msg_id,bool success,_out_ moon_char* out_reply_msg);

/**
 * 函数说明：
 *    校验数据包是否有效
 * 参数说明：
 *    utf8_package：utf8的数据报文
 * 返回值：
 *    如果有效则返回true，如果无效则返回false
 */
bool check_data_package_valid(moon_char* utf8_package);

/**
 * 函数说明：
 *   创建接受客户端连接的消息
 * 参数说明：
 *   client_id：客户端id
 *   out_accept_msg：输出接受客户端消息的json utf8字符串
 */
void create_message_accept_client(_in_ moon_char* client_id,_out_ moon_char* out_accept_msg);

#ifdef __cplusplus
}
#endif

#endif