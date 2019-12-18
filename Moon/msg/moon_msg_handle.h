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

#ifdef MS_WINDOWS
	/**
	 * 函数说明：
	 *    消息队列处理线程
	 */
	DWORD WINAPI msg_handle_thread(LPVOID lpParam);
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

#ifdef __cplusplus
}
#endif