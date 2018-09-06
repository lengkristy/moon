/************************************************************************
 * this file using Microsoft IOCP port multiplexing communication model. 
 * coding by: haoran dai
 * time:2018-5-1 16:23                               
 ***********************************************************************/
#pragma once
#ifndef _MS_SOCKET_CONTEXT
#define _MS_SOCKET_CONTEXT
#include "../collection/array_list.h"
#include "../cfg/environment.h"
#include "../module/moon_client.h"

#ifdef MS_WINDOWS
#include <winsock2.h>
#include <MSWSock.h>
#pragma comment(lib,"ws2_32.lib")
#endif

#ifdef MS_WINDOWS

#ifdef __cplusplus
extern "C" {
#endif

// Buffer length (1024*8)
// If you do have fewer sets of data from the client, set it smaller.
#define MAX_BUFFER_LEN        8192

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
	char           m_szBuffer[MAX_BUFFER_LEN];                 // This is a buffer of specific characters in WSABUF.
	OPERATION_TYPE m_OpType;                                   // Identify the type of network operation (corresponding to the enumeration above)

} MS_IO_CONTEXT, *PMS_IO_CONTEXT;

/*Create a new IO context*/
PMS_IO_CONTEXT create_new_io_context();
/*Release IO context*/
void free_io_context(PMS_IO_CONTEXT context);

/*Single handle data structure definition (for each completion port, which is the parameter of each Socket)*/
typedef struct _MS_SOCKET_CONTEXT
{
	CRITICAL_SECTION     SockCritSec;//Socket synchronization critical area object.
	SOCKET      m_socket;      // Socket for each client connection.
	SOCKADDR_IN m_client_addr; // Client address
	unsigned int m_client_port;// Port for each client.						
	char m_client_id[255];//Client ID(unique ID)
	Array_List* m_pListIOContext;//// The context data for the client network operation, which means that for each client Socket, it is possible to deliver multiple IO requests simultaneously.
	ClientEnvironment* mp_client_environment;//client running environment
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