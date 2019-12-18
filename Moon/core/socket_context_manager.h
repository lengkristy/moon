/************************************************************************
 * ���ļ���Ҫ�����ڹ������пͻ��˵�socket������. �Կͻ�����������ɾ�Ĳ�
 * coding by: haoran dai
 * time:2019-12-13 16:52                               
 ***********************************************************************/
#ifdef MS_WINDOWS
#include "ms_socket_context.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ����˵����
 *	��ʼ��socket�����Ĺ�����
 */
void init_socket_context_manager();

/**
 * ����˵����
 *	��ӿͻ���socket������
 * ������
 *	socket�����ĵĽṹ��ָ�룬�����windows�Ļ�����ô���ǣ�MS_SOCKET_CONTEXT *
 */
void add_socket_context(void* psocket_context);


/**
 * ����˵����
 *	ɾ���ͻ���socket������
 * ������
 *	socket�����ĵĽṹ��ָ�룬�����windows�Ļ�����ô���ǣ�MS_SOCKET_CONTEXT *
 */
void remove_socket_context(void* psocket_context);

#ifdef MS_WINDOWS
/**
 * ����˵����
 *   ͨ���ͻ���id����socket context
 * ������
 *   client_id���ͻ���id
 * ����ֵ��
 *   �����ѯ���򷵻�socket contextָ�룬û�в�ѯ������NULL
 */
MS_SOCKET_CONTEXT * get_socket_context_by_client_id(moon_char* client_id);
#endif

/**
 * ����˵����
 *   ��ȡ��ǰ���пͻ�������
 * ����ֵ��
 *   ��������
 */
int get_socket_context_count();

#ifdef MS_WINDOWS
/**
 * ����˵����
 *   ͨ��������ȡsocket context
 * ������
 *   index������
 * ����ֵ��
 *   �����ѯ���򷵻�socket contextָ�룬û�в�ѯ������NULL
 */
MS_SOCKET_CONTEXT * get_socket_context_by_index(int index);
#endif

#ifdef __cplusplus
}
#endif