#include "moon_message_router.h"
#include "../module/module_log.h"

#ifdef __cplusplus
extern "C" {
#endif


	/**
	 * ����˵����
	 *    ������Ϣ·�ɷ���
	 * ����˵����
	 *    p_server_config����������
	 */
	void start_message_router_service(moon_server_config *p_server_config)
	{
		moon_write_info_log("start message router service...");

		//���������˶��ٸ���Ϣ·�ɷ�����
		char **p_config = NULL;
		
	}

	/**
	 * ����˵����
	 *    ֹͣ��Ϣ·�ɷ���
	 */
	void stop_message_router_service()
	{

	}


	/**
	 * ����˵����
	 *   ��router��������Ϣ
	 * ����˵����
	 *   utf8_send_buf��utf8��Ϣ
	 *   size:������Ϣ���ֽڴ�С
	 * ����ֵ��
	 *   ����ɹ��򷵻�ʵ�ʷ��͵��ֽڳ��ȣ�ʧ�ܷ���-1
	 */
	int send_message_to_router(moon_char * utf8_send_buf,int size)
	{

	}

#ifdef __cplusplus
}
#endif