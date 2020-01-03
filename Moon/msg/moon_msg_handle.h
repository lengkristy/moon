/************************************************************************
 * ���ļ���Ҫ�Ǵ�����Ϣ���Ը�����Ϣ�Ľ�������
 * coding by: haoran dai
 * time:2019-12-14 16:23                               
 ***********************************************************************/
#include "../core/ms_nt__iocp.h"
#include "../module/moon_char.h"

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////��Ϣ������/////////////////////////////////////////////////////////////////

	/**
	 * ����˵����
	 *   ������Ϣ���лص��¼�
	 */
	int send_msg_queue_event(void* queue,void* data);

	/**
	 * ����˵����
	 *   ������Ϣ���лص��¼�
	 */
    int receive_msg_queue_event(void* queue,void* data);

/////////////////////////////////////////////////////////////������Ϣ����//////////////////////////////////////////////////////////////////////////


#ifdef MS_WINDOWS
	/**
	 * ����˵����
	 *    ��Ϣ���ն��д����߳�
	 */
	DWORD WINAPI msg_receive_handle_thread(LPVOID lpParam);
#endif

	/**
	 * ����˵����
	 *    �Խ��յĿͻ��˵�JSON�����д���
	 * ������
	 *    client_id:�ͻ���id
	 *    utf8_package:JSON���ݰ�
	 *    size:����С
	 */
	void msg_handler(moon_char* client_id,char* utf8_package,int size);


///////////////////////////////////////////////////////////������Ϣ����////////////////////////////////////////////////////////////////////////////

#ifdef MS_WINDOWS
	/**
	 * ����˵����
	 *     ��Ϣ���Ͷ��д����߳�
	 */
	DWORD WINAPI msg_send_handle_thread(LPVOID lpParam);
#endif

#ifdef __cplusplus
}
#endif