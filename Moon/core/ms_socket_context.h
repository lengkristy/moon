/************************************************************************/
/* 使用微软IOCP端口复用通信模型                                         */
/************************************************************************/
#pragma once
#ifndef _MS_SOCKET_CONTEXT
#define _MS_SOCKET_CONTEXT
#include "../collection/array_list.h"
#include "../cfg/environment.h"

#ifdef MS_WINDOWS
#include <winsock2.h>
#include <MSWSock.h>
#pragma comment(lib,"ws2_32.lib")
#endif

#ifdef __cplusplus
extern "C" {
#endif

// 缓冲区长度 (1024*8)
// 之所以为什么设置8K，也是一个江湖上的经验值
// 如果确实客户端发来的每组数据都比较少，那么就设置得小一些，省内存
#define MAX_BUFFER_LEN        8192

//////////////////////////////////////////////////////////////////
// 在完成端口上投递的I/O操作的类型
typedef enum _OPERATION_TYPE  
{  
	ACCEPT_POSTED,                     // 标志投递的Accept操作
	SEND_POSTED,                       // 标志投递的是发送操作
	RECV_POSTED,                       // 标志投递的是接收操作
	NULL_POSTED                        // 用于初始化，无意义

}OPERATION_TYPE;

//====================================================================================
//
//				单IO数据结构体定义(用于每一个重叠操作的参数)
//
//====================================================================================
typedef struct _MS_IO_CONTEXT
{
	OVERLAPPED     m_Overlapped;                               // 每一个重叠网络操作的重叠结构(针对每一个Socket的每一个操作，都要有一个)              
	SOCKET         m_sockAccept;                               // 这个网络操作所使用的Socket
	WSABUF         m_wsaBuf;                                   // WSA类型的缓冲区，用于给重叠操作传参数的
	char           m_szBuffer[MAX_BUFFER_LEN];                 // 这个是WSABUF里具体存字符的缓冲区
	OPERATION_TYPE m_OpType;                                   // 标识网络操作的类型(对应上面的枚举)

} MS_IO_CONTEXT, *PMS_IO_CONTEXT;

/*创建新的IO上下文*/
PMS_IO_CONTEXT create_new_io_context();
/*释放IO上下文*/
void free_io_context(PMS_IO_CONTEXT context);

/*单句柄数据结构体定义(用于每一个完成端口，也就是每一个Socket的参数)*/
typedef struct _MS_SOCKET_CONTEXT
{
	CRITICAL_SECTION     SockCritSec;//socket同步临界区对象
	SOCKET      m_socket;      // 每一个客户端连接的Socket
	SOCKADDR_IN m_client_addr; // 客户端的地址
	unsigned int m_client_port;// 每个客户端的端口								
	char m_client_id[255];//客户端ID(唯一标识)
	Array_List* m_pListIOContext;//// 客户端网络操作的上下文数据，也就是说对于每一个客户端Socket，是可以在上面同时投递多个IO请求的
}MS_SOCKET_CONTEXT,*PMS_SOCKET_CONTEXT;


/*创建一个新的socket上下文*/
PMS_SOCKET_CONTEXT create_new_socket_context();

/*释放socket上下文*/
void free_socket_context(PMS_SOCKET_CONTEXT context);

#ifdef __cplusplus
}
#endif
#endif