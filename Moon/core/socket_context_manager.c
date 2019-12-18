#include "../collection/array_list.h"
#include "../cfg/environment.h"
#include "ms_socket_context.h"

#ifdef __cplusplus
extern "C" {
#endif

Array_List* g_pClientSocketContext = NULL;//the context of client socket

/**
 * ����˵����
 *	��ʼ��socket�����Ĺ�����
 */
void init_socket_context_manager()
{
	if(g_pClientSocketContext == NULL)
	{
		g_pClientSocketContext = array_list_init();
	}
}

/**
 * ����˵����
 *	��ӿͻ���socket������
 * ������
 *	socket�����ĵĽṹ��ָ�룬�����windows�Ļ�����ô���ǣ�MS_SOCKET_CONTEXT *
 */
void add_socket_context(void* psocket_context)
{
	array_list_insert(g_pClientSocketContext,psocket_context,-1);
}

/**
 * ����˵����
 *	ɾ���ͻ���socket������
 * ������
 *	socket�����ĵĽṹ��ָ�룬�����windows�Ļ�����ô���ǣ�MS_SOCKET_CONTEXT *
 */
void remove_socket_context(void* psocket_context)
{
	array_list_remove(g_pClientSocketContext,psocket_context);
#ifdef MS_WINDOWS
	free_socket_context((MS_SOCKET_CONTEXT *)psocket_context);
#endif
}

#ifdef MS_WINDOWS
/**
 * ����˵����
 *   ͨ���ͻ���id����socket context
 * ������
 *   client_id���ͻ���id
 * ����ֵ��
 *   �����ѯ���򷵻�socket contextָ�룬û�в�ѯ������NULL
 */
MS_SOCKET_CONTEXT * get_socket_context_by_client_id(moon_char* client_id)
{
	int index = 0;
	MS_SOCKET_CONTEXT* pSocketContext = NULL;
	for (index = 0;index < g_pClientSocketContext->length;index++)
	{
		pSocketContext = (MS_SOCKET_CONTEXT*)array_list_getAt(g_pClientSocketContext,index);
		if(moon_char_equals(client_id,pSocketContext->m_client_id))
		{
			return pSocketContext;
			break;
		}
	}
	return NULL;
}
#endif

/**
 * ����˵����
 *   ��ȡ��ǰ���пͻ�������
 * ����ֵ��
 *   ��������
 */
int get_socket_context_count()
{
	if(g_pClientSocketContext != NULL)
	{
		return g_pClientSocketContext->length;
	}
	return 0;
}


#ifdef MS_WINDOWS
/**
 * ����˵����
 *   ͨ��������ȡsocket context
 * ������
 *   index������
 * ����ֵ��
 *   �����ѯ���򷵻�socket contextָ�룬û�в�ѯ������NULL
 */
MS_SOCKET_CONTEXT * get_socket_context_by_index(int index)
{
	if(index < 0 || index > g_pClientSocketContext->length)
	{
		return NULL;
	}
	return (MS_SOCKET_CONTEXT *)array_list_getAt(g_pClientSocketContext,index);
}
#endif


#ifdef __cplusplus
}
#endif