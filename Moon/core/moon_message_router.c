#include "moon_message_router.h"
#include "../module/module_log.h"
#include "../module/moon_memory_pool.h"
#include "../cfg/environment.h"

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * ·�ɵ�ַ�����б�
	 */
	_global_ char **p_global_router_addr_config = NULL;

	/**
	 * ���õ�·�ɵ�ַ����
	 */
	_global_ int global_router_addr_count = 0;


	/**
	 * ����˵����
	 *   ��������
	 * ������
	 *   p_global_server_config����������
	 * ����ֵ��
	 *   �ɹ�����true��ʧ�ܷ���false
	 */
	static bool _parse_config(moon_server_config *p_server_config)
	{
		//���������˶��ٸ���Ϣ·�ɷ�����
		int index = 0;
		int tmp_index = 0;
		int p_index = 0;
		char addr[20] = {0};
		while(p_server_config->router_server_ip[index] != '\0')
		{
			if (p_server_config->router_server_ip[index] == ',')
			{
				global_router_addr_count++;
			}
			index++;
		}
		global_router_addr_count++;
		p_global_router_addr_config = (char**)moon_malloc(sizeof(char *) * global_router_addr_count);
		index = 0;
		while(p_server_config->router_server_ip[index] != '\0')
		{
			if (p_server_config->router_server_ip[index] != ',')
			{
				addr[tmp_index] = p_server_config->router_server_ip[index];
				tmp_index++;
			}
			else
			{
				//�������һ��
				p_global_router_addr_config[p_index] = (char*)moon_malloc(20);
				memset((char*)p_global_router_addr_config[p_index],0,20);
				strcpy((char*)p_global_router_addr_config[p_index],addr);
				tmp_index = 0;
				memset(addr,0,20);
				p_index++;
			}
			index++;
		}

		//�����һ��ip��ӽ���
		p_global_router_addr_config[p_index] = (char*)moon_malloc(20);
		memset((char*)p_global_router_addr_config[p_index],0,20);
		strcpy((char*)p_global_router_addr_config[p_index],addr);
	}

	/**
	 * ����˵����
	 *    ������Ϣ·�ɷ���
	 * ����˵����
	 *    p_server_config����������
	 */
	void start_message_router_service(moon_server_config *p_server_config)
	{

		moon_write_info_log("start message router service...");
		if (!_parse_config(p_server_config))
		{
			moon_write_error_log("parse router config failed,please check router config");
			return;
		}
		//��ʼ����
		
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