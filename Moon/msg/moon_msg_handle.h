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

#ifdef MS_WINDOWS
	/**
	 * ����˵����
	 *    ��Ϣ���д����߳�
	 */
	DWORD WINAPI msg_handle_thread(LPVOID lpParam);
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

#ifdef __cplusplus
}
#endif