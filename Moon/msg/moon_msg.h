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
typedef struct _message_head{
	moon_char msg_id[50];/*message id,represents the unique identity of the message throughout the message chain*/
	int msg_order;/**��Ϣ����*/
	int main_msg_num;/*main message number*/
	int sub_msg_num;/*sub message number*/
	int msg_size;/*��Ϣ���ݵĴ�С��MessageBody�е�p_content��С���������紫����Ϣ�����ᱻ�ضϣ�ͨ�����ֶν�����װ*/
	moon_char msg_time[30];/*��Ϣ����ʱ��*/
	moon_char client_id[50];/*client id*/
	int msg_end;//��Ϣ������ʶ�������������⣬����ͬһ����Ϣid����Ϣ�����Ϣû�д�����ɣ���ô��Ϊ1����Ϣ���������Ϊ0
}message_head;

//define message body
typedef struct _message_body{
	moon_char* p_content;/*message content*/
}message_body;


//define message struct
typedef struct _moon_message{
	message_head *p_message_head;/*message head*/
	message_body *p_message_body;/*message body*/
}moon_message;

//�����Ե���Ϣ�ṹ
typedef struct _ptp_message_body{
	//���ͷ��Ŀͻ���id
	moon_char from_client_id[50];
	//���շ��ͻ���id
	moon_char to_client_id[50];
	//��������
	moon_char* p_content;
}ptp_message_body;

//����㲥��Ϣ�Ľṹ
typedef struct _user_broadcast_message_body{
	//���𷽵Ŀͻ���id
	moon_char from_client_id[50];
	//���շ��Ŀͻ���id�б�
	Array_List* p_to_client_ids;
	//Ⱥid
	moon_char group_id[50];
	//��Ϣ����
	moon_char* p_content;
}user_broadcast_message_body;

/**
 * ���巢����Ϣ�Ľṹ��
 */
typedef struct _msg_send{
	moon_char send_client_id[50];/*���Ϳͻ���id*/
	moon_char* utf8_msg_buf;/*utf8��Ϣ������*/
	int size;/*������Ϣ�������ڴ��С*/
}msg_send;

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////

/**
 * ����˵����
 *   �ж��������ϻ�ȡ�����ݰ��Ƿ��ǰ�ͷ��ʼ�����ݱ��ĵ�ǰ8���ֽ�Ϊ��ͷ��ʶ
 * ����ֵ��
 *   ����Ƿ���true�����򷵻�false
 */
bool pkg_is_head(moon_char* pkg);

/**
 * ����˵����
 *   �ж��������ϻ�ȡ�����ݰ��Ƿ����԰�β���������ݱ��ĵĺ�8���ֽ�Ϊ��β��ʶ
 * ����ֵ��
 *   ����Ƿ���true�����򷵻�false
 */
bool pkg_is_tail(moon_char* pkg);

/**
 * ����˵����
 *   �������ݰ�������ͷ��������ݽ�������
 * ������
 *   src_pkg��ԭʼ����
 *   out_data��������õ���ҵ��JSON����
 * ����ֵ��
 *   ���ظð��Ĵ�С
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
moon_message* parse_msg_head(moon_char* str_message);


/**
 * release message memory
 */
void free_msg(moon_message* p_message);

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
 * ����˵����
 *   ����һ��������յ��ͻ�����Ϣ����Ӧ��
 * ������
 *   msg_id:��Ϣid
 *   success:�Ƿ���ɹ�
 * ����ֵ��
 *  ����json���ݰ�
 */
void create_server_receive_message_rely(moon_char* msg_id,bool success,_out_ moon_char* out_reply_msg);

/**
 * ����˵����
 *    У�����ݰ��Ƿ���Ч
 * ����˵����
 *    utf8_package��utf8�����ݱ���
 * ����ֵ��
 *    �����Ч�򷵻�true�������Ч�򷵻�false
 */
bool check_data_package_valid(moon_char* utf8_package);

/**
 * ����˵����
 *   �������ܿͻ������ӵ���Ϣ
 * ����˵����
 *   client_id���ͻ���id
 *   out_accept_msg��������ܿͻ�����Ϣ��json utf8�ַ���
 */
void create_message_accept_client(_in_ moon_char* client_id,_out_ moon_char* out_accept_msg);

/**
 * ����˵����
 *    ������Ե���Ϣ��
 * ����˵����
 *    p_msg��ԭʼutf8��Ϣ��
 *    p_ptp_message_body������������Ե���Ϣ�ṹ����Ϣ
 */
void parse_ptp_message_body(moon_char* p_msg,ptp_message_body* p_ptp_message_body);

/**
 * ����˵����
 *    �����û��㲥Ⱥ����Ϣ
 * ����˵����
 *    p_msg��ԭʼutf8��Ϣ��
 *    p_user_broadcast_msg_body�����������Ĺ㲥��Ϣ��
 */
void parse_broadcast_message_body(moon_char* p_msg,user_broadcast_message_body* p_user_broadcast_msg_body);

/**
 * ����˵����
 *   ����ע��·�ɷ�����Ϣ
 * ������
 *   p_out_msg�����utf8��Ϣ
 */
void create_regist_router_server_msg(_out_ moon_char* p_out_msg);

#ifdef __cplusplus
}
#endif

#endif