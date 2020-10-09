/**********************************************************************************
 * ˵�������ļ�Ϊ��Ϣ��·�ɷ��񽻻�ģ�飬����Ϣ·�ɷ�������Ϣ�ͽ���·�ɷ������Ϣ
 *  
 **********************************************************************************/
#ifndef _MOON_MESSAGE_ROUTER_H
#define _MOON_MESSAGE_ROUTER_H
#include "../module/moon_config_struct.h"
#include "../module/moon_base.h"

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * ����˵����
	 *    ������Ϣ·�ɷ���
	 * ����˵����
	 *    p_server_config����������
	 */
	bool start_message_router_service(moon_server_config *p_server_config);

	/**
	 * ����˵����
	 *    ֹͣ��Ϣ·�ɷ���
	 */
	void stop_message_router_service();

	/**
	 * ����˵����
	 *   ��router��������Ϣ
	 * ����˵����
	 *   utf8_send_buf��utf8��Ϣ
	 *   size:������Ϣ���ֽڴ�С
	 * ����ֵ��
	 *   ����ɹ��򷵻�ʵ�ʷ��͵��ֽڳ��ȣ�ʧ�ܷ���-1
	 */
	int send_message_to_router(moon_char * utf8_send_buf,int size);

#ifdef __cplusplus
}
#endif

#endif