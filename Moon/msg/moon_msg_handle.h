/************************************************************************
 * 该文件主要是处理消息，对各类消息的解析处理
 * coding by: haoran dai
 * time:2019-12-14 16:23                               
 ***********************************************************************/
#include "../core/ms_nt__iocp.h"
#include "../module/moon_char.h"

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////消息计数器/////////////////////////////////////////////////////////////////

	/**
	 * 函数说明：
	 *   发送消息队列回调事件
	 */
	int send_msg_queue_event(void* queue,void* data);

	/**
	 * 函数说明：
	 *   接收消息队列回调事件
	 */
    int receive_msg_queue_event(void* queue,void* data);

/////////////////////////////////////////////////////////////接收消息处理//////////////////////////////////////////////////////////////////////////


#ifdef MS_WINDOWS
	/**
	 * 函数说明：
	 *    消息接收队列处理线程
	 */
	DWORD WINAPI msg_receive_handle_thread(LPVOID lpParam);
#endif

	/**
	 * 函数说明：
	 *    对接收的客户端的JSON包进行处理
	 * 参数：
	 *    client_id:客户端id
	 *    utf8_package:JSON数据包
	 *    size:包大小
	 */
	void msg_handler(moon_char* client_id,char* utf8_package,int size);


///////////////////////////////////////////////////////////发送消息处理////////////////////////////////////////////////////////////////////////////

#ifdef MS_WINDOWS
	/**
	 * 函数说明：
	 *     消息发送队列处理线程
	 */
	DWORD WINAPI msg_send_handle_thread(LPVOID lpParam);
#endif

#ifdef __cplusplus
}
#endif