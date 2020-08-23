/************************************************************************
 * this file using Microsoft IOCP port multiplexing communication model. 
 * coding by: haoran dai
 * time:2018-5-1 16:23                               
 ***********************************************************************/
#ifndef _MS_SOCKET_CONTEXT_H
#define _MS_SOCKET_CONTEXT_H
#include "../cfg/environment.h"
#include "../msg/moon_protocol.h"
#include "../collection/array_list.h"
#include "../module/moon_base.h"

#ifdef MS_WINDOWS
#include <winsock2.h>
#include <MSWSock.h>
#pragma comment(lib,"ws2_32.lib")
#endif

#ifdef MS_WINDOWS

#ifdef __cplusplus
extern "C" {
#endif

//Enter critical region
#define _EnterCriticalSection(x) {if( x != NULL){EnterCriticalSection(&x->m_SockCritSec);}}
//exit critical region
#define _LeaveCriticalSection(x) {if( x != NULL){LeaveCriticalSection(&x->m_SockCritSec);}}

//////////////////////////////////////////////////////////////////
// Type of I/O operation delivered on the completion port.
typedef enum _OPERATION_TYPE  
{  
	ACCEPT_POSTED,                     // Accept operation
	SEND_POSTED,                       // send operation
	RECV_POSTED,                       // receive operation
	NULL_POSTED                        // NULL operation

}OPERATION_TYPE;

//====================================================================================
//
//		Single IO data structure definition (parameters for each overlapping operation)
//
//====================================================================================
typedef struct _MS_IO_CONTEXT
{
	OVERLAPPED     m_Overlapped;                               // Overlapping structures for each overlapping network operation (one for each operation of each Socket             
	SOCKET         m_sockAccept;                               // The Socket used by this network operation.
	WSABUF         m_wsaBuf;                                   // A buffer of type WSA used to pass parameters to overlapped operations.
	char           m_szBuffer[PKG_BYTE_MAX_LENGTH];            // This is a buffer of specific characters in WSABUF.
	OPERATION_TYPE m_OpType;                                   // Identify the type of network operation (corresponding to the enumeration above)
} MS_IO_CONTEXT, *PMS_IO_CONTEXT;

/*Create a new IO context*/
PMS_IO_CONTEXT create_new_io_context();
/*Release IO context*/
void free_io_context(PMS_IO_CONTEXT context);

/*Single handle data structure definition (for each completion port, which is the parameter of each Socket)*/
typedef struct _MS_SOCKET_CONTEXT
{
	CRITICAL_SECTION     m_SockCritSec;//Socket synchronization critical area object.
	SOCKET      m_socket;      // Socket for each client connection.
	SOCKADDR_IN m_client_addr; // Client address
	unsigned int m_client_port;// Port for each client.						
	moon_char m_client_id[255];//Client ID(unique ID)
	Array_List* m_pListIOContext;//// The context data for the client network operation, which means that for each client Socket, it is possible to deliver multiple IO requests simultaneously.
	//client_environment* mp_client_environment;//client running environment
	//当前接收的数据的完整包，如果接收的数据不完整，那么将继续接收数据直到下一个包头，如果接收的数据是包头，那么将当前的数据给业务处理器，然后将当前缓冲区置空，包的编码为utf8
	moon_char* m_completePkg;
	//数据包大小
	//int m_pkgSize;
	//当前接收的数据大小
	int m_currentPkgSize;
}MS_SOCKET_CONTEXT,*PMS_SOCKET_CONTEXT;


/*Create a new socket context.*/
PMS_SOCKET_CONTEXT create_new_socket_context();

/*Release the socket context*/
void free_socket_context(PMS_SOCKET_CONTEXT context);

#ifdef __cplusplus
}
#endif
#endif
#endif