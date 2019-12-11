#include "ms_nt__iocp.h"
#include "ms_socket_context.h"
#include "../module/moon_char.h"
#include "../module/moon_string.h"
#include "../msg/moon_msg.h"
#include <stdio.h>

#ifdef MS_WINDOWS
#include <wchar.h>
#endif
#include "../module/moon_memory_pool.h"

#ifdef MS_WINDOWS
#ifdef __cplusplus
extern "C" {
#endif

//how many threads are generated on each cpu processor.(To maximize server performance)
#define WORKER_THREADS_PER_PROCESSOR 2
//The number of Accept requests at the same time
#define MAX_POST_ACCEPT              10
// The exit signal passed to the Worker thread.
#define EXIT_CODE                    NULL


// the macro definition of release pointer
#define RELEASE(x)                      {if(x != NULL ){free(x);x=NULL;}}
// the macro definition of release handle
#define RELEASE_HANDLE(x)               {if(x != NULL && x!=INVALID_HANDLE_VALUE){ CloseHandle(x);x = NULL;}}
// the macro definition of release socket
#define RELEASE_SOCKET(x)               {if(x !=INVALID_SOCKET) { closesocket(x);x=INVALID_SOCKET;}}
//Enter critical region
#define _EnterCriticalSection(x) {if( x != NULL){EnterCriticalSection(&x->SockCritSec);}}
//exit critical region
#define _LeaveCriticalSection(x) {if( x != NULL){LeaveCriticalSection(&x->SockCritSec);}}

static HANDLE	g_hShutdownEvent;// the event of notifies the thread system to exit, in order to better exit the thread.
static HANDLE g_hIOCompletionPort;//the handle of iocp
static unsigned int g_nThreads;//the count of thread
static HANDLE* g_phWorkerThreads;// the handle pointer of work thread
static PMS_SOCKET_CONTEXT g_pListenContext = NULL;//listen context
static LPFN_ACCEPTEX                g_lpfnAcceptEx;// the functions pointer of AcceptEx and GetAcceptExSockaddrs，in order to invoke them
static LPFN_GETACCEPTEXSOCKADDRS    g_lpfnGetAcceptExSockAddrs; 
static Array_List* g_pListMSClientSocketContext;//the context of client socket
static HANDLE g_hAliveThread;//heartbeat detection thread


/**
 * @desc deal with client message
 * @param p_msg:the Message struct pointer
 **/
void client_login_handle(PMS_IO_CONTEXT pIoContext,Message* p_msg)
{
	char client_login_id[32] = {0};
	moon_char ret_json_data[512] = {0};
	char utf8_ret_json_data[512] = {0};
	//MS_IO_CONTEXT *p_msio;
	int i = 0;
	if (p_msg == NULL)
	{
		return;
	}
	switch (p_msg->p_message_head->sub_msg_num)
	{
	case SYS_SUB_PROTOCOL_LOGIN_FIRST://client first login
		{
			if(p_msg->p_message_body->p_content == NULL)
			{
				memset(ret_json_data,0,512);
				memset(utf8_ret_json_data,0,512);
				create_client_login_failed_msg("client login id is not null",ret_json_data);
				moon_ms_windows_unicode_to_utf8(ret_json_data,utf8_ret_json_data);
				ms_iocp_send(pIoContext->m_sockAccept,utf8_ret_json_data,strlen(utf8_ret_json_data));
				return;
			}
			parse_login_id(p_msg->p_message_body->p_content,client_login_id);
			if (stringIsEmpty(client_login_id))
			{
				//p_msio = (MS_IO_CONTEXT*)moon_malloc(sizeof(MS_IO_CONTEXT));
				//client login id is null,and send a login-faild message to client
				memset(ret_json_data,0,512);
				memset(utf8_ret_json_data,0,512);
				create_client_login_failed_msg("client login id is not null",ret_json_data);
				moon_ms_windows_unicode_to_utf8(ret_json_data,utf8_ret_json_data);
				/*p_msio->m_OpType = RECV_POSTED;
				memset(&(p_msio->m_Overlapped), 0, sizeof(OVERLAPPED));
				p_msio->m_sockAccept = pIoContext->m_sockAccept;
				p_msio->m_wsaBuf.buf = utf8_ret_json_data;
				strcpy(p_msio->m_szBuffer,utf8_ret_json_data);
				p_msio->m_wsaBuf.len = strlen(utf8_ret_json_data);
				doSend(p_msio);
				moon_free(p_msio);
				p_msio = NULL;*/
				ms_iocp_send(pIoContext->m_sockAccept,utf8_ret_json_data,strlen(utf8_ret_json_data));
				return;
			}
			//verify login-id is valid
			//send login-successful message
			//p_msio = (MS_IO_CONTEXT*)moon_malloc(sizeof(MS_IO_CONTEXT));
			memset(ret_json_data,0,512);
			memset(utf8_ret_json_data,0,512);
			create_client_login_success_msg(ret_json_data);
			moon_ms_windows_unicode_to_utf8(ret_json_data,utf8_ret_json_data);
			/*p_msio->m_OpType = SEND_POSTED;
			memset(&(p_msio->m_Overlapped), 0, sizeof(OVERLAPPED)); 
			p_msio->m_sockAccept = pIoContext->m_sockAccept;
			p_msio->m_wsaBuf.buf = utf8_ret_json_data;
			strcpy(p_msio->m_szBuffer,utf8_ret_json_data);
			p_msio->m_wsaBuf.len = strlen(utf8_ret_json_data);
			doSend(p_msio);*/
			ms_iocp_send(pIoContext->m_sockAccept,utf8_ret_json_data,strlen(utf8_ret_json_data));
		}
		break;
	default:
		break;
	}
}

/**
 * @desc deal with network message
 * @param p_msg:the Message struct pointer
 **/
void msg_handle(PMS_IO_CONTEXT pIoContext,Message* p_msg)
{
	if (p_msg == NULL)
	{
		return;
	}
	switch (p_msg->p_message_head->main_msg_num)
	{
	case SYS_MAIN_PROTOCOL_LOGIN: //login message
		client_login_handle(pIoContext,p_msg);
		break;
	default:
		break;
	}
}

/**
 * Heartbeat packet detection
 */
bool isSocketAlive(SOCKET s)
{
	int nByteSent=send(s,"",0,0);//send a package
	if (-1 == nByteSent) return false;
	return true;
}

///////////////////////////////////////////////////////////////////
// handles errors on the completion port.
bool handleError( MS_SOCKET_CONTEXT *pContext,const DWORD dwErr )
{
	char strMsg[256] = {0};
	//time out error
	if(WAIT_TIMEOUT == dwErr)  
	{  	
		//Whether the client links
		if( !isSocketAlive( pContext->m_socket) )
		{
			array_list_remove(g_pListMSClientSocketContext,pContext);
			free_socket_context(pContext);
			pContext = NULL;
			return true;
		}
		else
		{
			return true;
		}
	}
	// The client aborted.
	else if( ERROR_NETNAME_DELETED==dwErr )
	{
		array_list_remove(g_pListMSClientSocketContext,pContext);
		return true;
	}
	else
	{
		sprintf(strMsg,"completion port error,error code:%d",dwErr);
		moon_write_error_log(strMsg);
		return false;
	}
}

/////////////////////////////////////////////////////
// Bind the handle (Socket) to the completion port.
bool associateWithIOCP( PMS_SOCKET_CONTEXT pContext )
{
	char strMsg[256] = {0};
	// The SOCKET used to communicate with the client is bound to the completion port.
	HANDLE hTemp = CreateIoCompletionPort((HANDLE)pContext->m_socket, g_hIOCompletionPort, (DWORD)pContext, 0);

	if (NULL == hTemp)
	{
		sprintf(strMsg,"execute CreateIoCompletionPort() function error,error code:%d",GetLastError());
		moon_write_error_log(strMsg);
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////
// post the received data request.
bool postRecv( PMS_IO_CONTEXT pIoContext )
{
	DWORD dwFlags = 0;
	DWORD dwBytes = 0;
	int nBytesRecv = 0;
	char strMsg[256] = {0};
	WSABUF *p_wbuf   = &pIoContext->m_wsaBuf;
	OVERLAPPED *p_ol = &pIoContext->m_Overlapped;

	memset(pIoContext->m_szBuffer,0,MAX_BUFFER_LEN);
	pIoContext->m_OpType = RECV_POSTED;

	//After the initialization is complete, the WSARecv request is delivered.
	nBytesRecv = WSARecv( pIoContext->m_sockAccept, p_wbuf, 1, &dwBytes, &dwFlags, p_ol, NULL );

	//If the return value error and the error code is not Pending, then this overlapping request fails.
	if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()))
	{
		sprintf(strMsg,"post message error,error code:%d",WSAGetLastError());
		moon_write_error_log(strMsg);
		return false;
	}
	return true;
}

//====================================================================================
//
//				    Post completion port request.
//
//====================================================================================
//////////////////////////////////////////////////////////////////
// post an Accept request
bool post_accept( PMS_IO_CONTEXT pAcceptIoContext )
{
	DWORD dwBytes = 0;  
	char strMsg[256] = {0};
	WSABUF *p_wbuf   = &pAcceptIoContext->m_wsaBuf;
	OVERLAPPED *p_ol = &pAcceptIoContext->m_Overlapped;
	pAcceptIoContext->m_OpType = ACCEPT_POSTED;

	//Prepare the Socket for the new incoming client (this is the biggest difference from the traditional accept)
	pAcceptIoContext->m_sockAccept  = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if(INVALID_SOCKET==pAcceptIoContext->m_sockAccept)  
	{  
		sprintf(strMsg,"create accept socket failed,error code:%d",WSAGetLastError());
		moon_write_error_log(strMsg);
		return false;
	} 

	// post AcceptEx
	if(FALSE == g_lpfnAcceptEx( g_pListenContext->m_socket, pAcceptIoContext->m_sockAccept, p_wbuf->buf, p_wbuf->len - ((sizeof(SOCKADDR_IN)+16)*2),   
		sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN)+16, &dwBytes, p_ol))  
	{  
		if(WSA_IO_PENDING != WSAGetLastError())  
		{  
			sprintf(strMsg,"post accept request failed,error code:%d",WSAGetLastError());
			moon_write_error_log(strMsg);
			return false;
		}  
	}
	return true;
}

////////////////////////////////////////////////////////////
// Processing when the client is connected.
//
bool doAccpet(PMS_IO_CONTEXT pIoContext )
{
	
	SOCKADDR_IN* ClientAddr = NULL;
	SOCKADDR_IN* LocalAddr = NULL;  
	PMS_SOCKET_CONTEXT pNewSocketContext = NULL;
	PMS_IO_CONTEXT pNewIoContext = NULL;
	int remoteLen = sizeof(SOCKADDR_IN), localLen = sizeof(SOCKADDR_IN);
	moon_char clientMsg[MAX_BUFFER_LEN] = {0};
	int len = 0;
	Message* p_msg = NULL;
	ClientEnvironment* p_client_env = NULL;
	
	moon_write_debug_log("new connect is comming");

	///////////////////////////////////////////////////////////////////////////
	// 1. First obtain the address information of the client.
	// Not only can you get the client and local address information, but also the first set of data sent by the client.
	g_lpfnGetAcceptExSockAddrs(pIoContext->m_wsaBuf.buf, pIoContext->m_wsaBuf.len - ((sizeof(SOCKADDR_IN)+16)*2),  
		sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN)+16, (LPSOCKADDR*)&LocalAddr, &localLen, (LPSOCKADDR*)&ClientAddr, &remoteLen);
	//Parsing the first message from the client.
	len = moon_ms_windows_utf8_to_unicode(pIoContext->m_szBuffer,clientMsg);//将收到的utf-8的字节序转化为unicode
	//parse message
	p_msg = parse_msg_head(clientMsg);
	//the first message is about client running environment,if not this,the server will refuse
	if (p_msg == NULL)
	{
		//close client socket
		closesocket(pIoContext->m_sockAccept);
		free_msg(p_msg);
		return false;
	}
	p_client_env = parse_client_running_environment(clientMsg);
	if (p_client_env = NULL)
	{
		//close client socket
		closesocket(pIoContext->m_sockAccept);
		free_msg(p_msg);
		return false;
	}
	free_msg(p_msg);
	p_msg = NULL;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// 2.So, notice here, this is the Context in the listening socket, and this Context we also need to use to listen for the next connection.
	// I have to copy the Context on the listening Socket to create a new SocketContext for the new Socket.

	pNewSocketContext = create_new_socket_context();
	pNewSocketContext->m_socket           = pIoContext->m_sockAccept;
	pNewSocketContext->mp_client_environment = p_client_env;
	memcpy(&(pNewSocketContext->m_client_addr), ClientAddr, sizeof(SOCKADDR_IN));

	// After the parameters are set, the Socket is bound to the completion port.
	if( false==associateWithIOCP( pNewSocketContext ) )
	{
		RELEASE(pNewSocketContext);
		return false;
	} 

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// 3. The IoContext is established to deliver the first Recv data request on this Socket.
	pNewIoContext = create_new_io_context();
	pNewIoContext->m_OpType       = RECV_POSTED;
	pNewIoContext->m_sockAccept   = pNewSocketContext->m_socket;

	//memcpy( pNewIoContext->m_szBuffer,pIoContext->m_szBuffer,MAX_BUFFER_LEN );

	// Once the binding is complete, you can begin to deliver the completed request on this Socket.
	if( false==postRecv( pNewIoContext) )
	{
		array_list_remove(pNewSocketContext->m_pListIOContext,pNewIoContext);
		free_io_context(pNewIoContext);
		free_socket_context(pNewSocketContext);
		return false;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////
	// 4. If the delivery is successful, then add this valid client information to the ContextList
	//    (which requires unified management and easy release of resources)
	array_list_insert(g_pListMSClientSocketContext,pNewSocketContext,-1);
	array_list_insert(pNewSocketContext->m_pListIOContext,pNewIoContext,-1);

	////////////////////////////////////////////////////////////////////////////////////////////////
	// 5. After use, reset the IoContext of the Listen Socket, and then prepare to deliver the new AcceptEx.
	memset(pIoContext->m_szBuffer,0,MAX_BUFFER_LEN);
	return post_accept( pIoContext );
}

/////////////////////////////////////////////////////////////////
// When the received data arrives, it is processed.
bool doRecv(PMS_IO_CONTEXT pIoContext)
{
	// First display the last data, then reset the state and issue the next Recv request.
	//SOCKADDR_IN* ClientAddr = &pSocketContext->m_client_addr;
	//this->_ShowMessage( _T("receive  %s:%d message：%s"),inet_ntoa(ClientAddr->sin_addr), ntohs(ClientAddr->sin_port),pIoContext->m_wsaBuf.buf );

	// Then start delivering the next WSARecv request
	
	//recv message
	WSABUF buf;
	char *p_utf8_msg = pIoContext->m_wsaBuf.buf;
	moon_char* client_msg = NULL;
	int len = 0;
	unsigned long size = 0;
	Message* p_msg = NULL;
	size = pIoContext->m_wsaBuf.len + 2;
	client_msg = (moon_char*)moon_malloc(size);
	if (client_msg == NULL)
	{
		return postRecv( pIoContext );
	}
	memset(client_msg,0,size);
	len = moon_ms_windows_utf8_to_unicode(p_utf8_msg,client_msg);//将收到的utf-8的字节序转化为moon_char
	p_msg = parse_msg_head(client_msg);
	if (p_msg == NULL)
	{
		postRecv(pIoContext);
	}
	//deal with message
	msg_handle(pIoContext,p_msg);
	free_msg(p_msg);
	p_msg = NULL;
	moon_free(client_msg);
	client_msg = NULL;

	//
	return postRecv(pIoContext);
}

//////////////////////////////////////////////////////////////////////////
// post a request for sending data.
bool postSend(PMS_IO_CONTEXT pIoContext)
{
	DWORD dwFlags = 0;
	DWORD dwBytes = 0;
	int nBytesRecv = 0;
	char strMsg[256] = {0};
	WSABUF *p_wbuf   = &pIoContext->m_wsaBuf;
	OVERLAPPED *p_ol = &pIoContext->m_Overlapped;
	pIoContext->m_OpType = SEND_POSTED;
	//memset(pIoContext->m_szBuffer,0,MAX_BUFFER_LEN);
	p_wbuf->len = strlen(p_wbuf->buf);
	// After the initialization is complete, the request for sending the data is sent to the WSARecv request.
	nBytesRecv = WSASend( pIoContext->m_sockAccept, p_wbuf, 1, &dwBytes, dwFlags, p_ol, NULL );

	// If the return value error and the error code is not Pending, then this overlapping request fails.
	if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()))
	{
		sprintf(strMsg,"post message error,error code:%d",WSAGetLastError());
		moon_write_error_log(strMsg);
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
//Processing when data is sent.
bool doSend(PMS_IO_CONTEXT pIoContext)
{
	return postSend(pIoContext);
}

///////////////////////////////////////////////////////////////////
// work thread：  Worker threads for the IOCP request service
// That is, whenever a packet is completed on the completion port, it is taken out and processed.
///////////////////////////////////////////////////////////////////
DWORD static WINAPI worker_thread(LPVOID lpParam)
{
	OVERLAPPED           *pOverlapped = NULL;
	PMS_SOCKET_CONTEXT   pSocketContext = NULL;
	DWORD                dwBytesTransfered = 0;

	// The loop processes the request until the Shutdown information is received.
	while (WAIT_OBJECT_0 != WaitForSingleObject(g_hShutdownEvent, 0))
	{
		BOOL bReturn = GetQueuedCompletionStatus(
			g_hIOCompletionPort,
			&dwBytesTransfered,
			(PULONG_PTR)&pSocketContext,
			&pOverlapped,
			INFINITE);
		if (pSocketContext != NULL)
		{
			_EnterCriticalSection(pSocketContext);
		}
		//If you receive an exit sign, exit directly.
		if ( EXIT_CODE==(DWORD)pSocketContext )
		{
			if (pSocketContext != NULL)
			{
				_LeaveCriticalSection(pSocketContext);
			}
			break;
		}

		// There was a mistake
		if( !bReturn )  
		{  
			DWORD dwErr = GetLastError();
			if( !handleError( pSocketContext,dwErr ) )
			{
				_LeaveCriticalSection(pSocketContext);
				break;
			}
			_LeaveCriticalSection(pSocketContext);
			continue;
		}  
		else  
		{  
			// Read the incoming parameters.
			MS_IO_CONTEXT* pIoContext = CONTAINING_RECORD(pOverlapped, MS_IO_CONTEXT, m_Overlapped);  

			// Is the client disconnected?
			if((0 == dwBytesTransfered) && ( RECV_POSTED==pIoContext->m_OpType || SEND_POSTED==pIoContext->m_OpType))  
			{  
				// release client resource
				array_list_remove(g_pListMSClientSocketContext,pSocketContext);
				free_socket_context(pSocketContext);
				pSocketContext = NULL;
				_LeaveCriticalSection(pSocketContext);
				continue;  
			}  
			else
			{
				switch( pIoContext->m_OpType )  
				{  
					// Accept  
				case ACCEPT_POSTED:
					{ 
						// To increase the readability of your code,use the special _DoAccept function to process the incoming requests.
						doAccpet(pIoContext);
					}
					break;

					// RECV
				case RECV_POSTED:
					{
						// In order to increase the readability of the code, a special _DoRecv function is used to process the receive request.
						doRecv(pIoContext);
						//make Io buf null
						memset(pIoContext->m_wsaBuf.buf,0,pIoContext->m_wsaBuf.len);
						pIoContext->m_wsaBuf.len = 0;
					}
					break;

					// SEND
				case SEND_POSTED:
					{
						moon_write_info_log("start send message:");
						moon_write_info_log(pIoContext->m_wsaBuf.buf);
					}
					break;
				default:
					// It shouldn't be done here.
					//TRACE(_T("_WorkThread中的 pIoContext->m_OpType exception.\n"));
					break;
				} //switch
			}//if
		}//if
		_LeaveCriticalSection(pSocketContext);
	}//while
	moon_write_info_log("the worker thread exit");
	return 0;
}

/*****************************************************************
 *  Heartbeat packet detection thread.
 *****************************************************************/
DWORD static WINAPI alive_thread(LPVOID lpParam)
{
	int i = 0;
	MS_SOCKET_CONTEXT* pSocketContext = NULL;
	while (WAIT_OBJECT_0 != WaitForSingleObject(g_hShutdownEvent, 0))
	{
		for (i = 0;i < g_pListMSClientSocketContext->length;i++)
		{
			pSocketContext = (MS_SOCKET_CONTEXT*)array_list_getAt(g_pListMSClientSocketContext,i);
			if(pSocketContext != NULL && !isSocketAlive(pSocketContext->m_socket))
			{
				array_list_remove(g_pListMSClientSocketContext,pSocketContext);
				pSocketContext = NULL;
				i--;
			}
		}
		Sleep(2000);
	}
	moon_write_info_log("alive thread exit");
	return 0;
}

////////////////////////////////////////////////////////////////////
// init WinSock 2.2
bool static load_socket_lib()
{    
	WSADATA wsaData;
	int nResult;
	char strMsg[256] = {0};
	nResult = WSAStartup(MAKEWORD(2,2), &wsaData);

	if (NO_ERROR != nResult)
	{
		sprintf(strMsg,"init winsock 2.2 falied,error code:%d",WSAGetLastError());
		moon_write_error_log(strMsg);
		return false; 
	}

	return true;
}

///////////////////////////////////////////////////////////////////
// Gets the number of processors in the machine.
int getNoOfProcessors()
{
	SYSTEM_INFO si;

	GetSystemInfo(&si);

	return si.dwNumberOfProcessors;
}

////////////////////////////////
// Initialize the completion port.
bool ms_iocp_init()
{
	int i = 0;
	DWORD nThreadID;
	char strMsg[256] = {0};
	// Build the first completion port.
	g_hIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0 );

	if ( NULL == g_hIOCompletionPort)
	{
		sprintf(strMsg,"create Completion port falied,error code:%d",WSAGetLastError());
		moon_write_error_log(strMsg);
		return false;
	}

	// According to the number of processors in the machine, the corresponding number of threads is established.
	g_nThreads = WORKER_THREADS_PER_PROCESSOR * getNoOfProcessors();

	// Initializes the handle for the worker thread.
	g_phWorkerThreads = (HANDLE*)malloc(sizeof(HANDLE) * g_nThreads);

	// Create worker threads based on the number of calculations.
	for (i=0; i < g_nThreads; i++)
	{
		g_phWorkerThreads[i] = CreateThread(0, 0, worker_thread, NULL, 0, &nThreadID);
	}

	return true;
}

////////////////////////////////////////////////////////////
//	Finally, all resources are released.
void dispose()
{
	int i = 0;
	// Close the system to exit the event handle.
	RELEASE_HANDLE(g_hShutdownEvent);

	// Release the worker thread handle pointer.
	for(i=0;i<g_nThreads;i++ )
	{
		RELEASE_HANDLE(g_phWorkerThreads[i]);
	}

	RELEASE(g_phWorkerThreads);

	//The thread handle that releases the heartbeat packet.
	RELEASE_HANDLE(g_hAliveThread);

	// Close the IOCP handle.
	RELEASE_HANDLE(g_hIOCompletionPort);

	// Close listening Socket
	free_socket_context(g_pListenContext);

	//Releases the client context collection
	for (i = 0; i < g_pListMSClientSocketContext->length;)
	{
		free_socket_context((PMS_SOCKET_CONTEXT)array_list_getAt(g_pListMSClientSocketContext,i));
		i = 0;
	}
	array_list_free(g_pListMSClientSocketContext);
}

/////////////////////////////////////////////////////////////////
// init Socket
bool ms_init_listen_socket(const Moon_Server_Config* p_global_server_config)
{
	// The GUID of AcceptEx and GetAcceptExSockaddrs for exporting function Pointers.
	GUID GuidAcceptEx = WSAID_ACCEPTEX;  
	GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS; 
	char strMsg[256] = {0};
	//PMS_SOCKET_CONTEXT pNewSocketContext = NULL;
	//PMS_IO_CONTEXT pNewIoContext = NULL;
	PMS_IO_CONTEXT pAcceptIoContext = NULL;
	int i = 0 ;
	int addrlen = 0;
	DWORD dwBytes = 0; 
	DWORD	Flags = 0;
	// Server address information for binding sockets.
	struct sockaddr_in ServerAddress;

	// Generate the information for the Socket used for listening.
	g_pListenContext = create_new_socket_context();

	// Overlapped IO is required, and you must use WSASocket to set up sockets to support overlapping IO operations.
	g_pListenContext->m_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == g_pListenContext->m_socket) 
	{
		sprintf(strMsg,"init socket failed,error code:%d",WSAGetLastError());
		moon_write_error_log(strMsg);
		return false;
	}
	else
	{
		sprintf(strMsg,"init socket success");
		moon_write_info_log(strMsg);
		memset(strMsg,0,256);
	}

	// Bind the Listen Socket to the completion port.
	if( NULL== CreateIoCompletionPort( (HANDLE)g_pListenContext->m_socket, g_hIOCompletionPort,(DWORD)g_pListenContext, 0))  
	{  
		sprintf(strMsg,"bind listen socket to io completion port failed,error code:%d",WSAGetLastError());
		moon_write_error_log(strMsg); 
		RELEASE_SOCKET( g_pListenContext->m_socket );
		return false;
	}
	else
	{
		sprintf(strMsg,"listen socket binding success");
		moon_write_info_log(strMsg);
		memset(strMsg,0,256);
	}

	// Fill address information
	ZeroMemory((char *)&ServerAddress, sizeof(ServerAddress));
	ServerAddress.sin_family = AF_INET;
	// You can bind any available IP address, or bind a specified IP address.
	//ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);                      
	ServerAddress.sin_addr.s_addr = inet_addr(p_global_server_config->server_ip);         
	ServerAddress.sin_port = htons(p_global_server_config->server_port);                          

	// Bind the address and port.
	if (SOCKET_ERROR == bind(g_pListenContext->m_socket, (struct sockaddr *) &ServerAddress, sizeof(ServerAddress))) 
	{
		sprintf(strMsg,"bind() execute failed,error code:%d",WSAGetLastError());
		moon_write_error_log(strMsg); 
		RELEASE_SOCKET( g_pListenContext->m_socket );
		return false;
	}

	// Start listening
	if (SOCKET_ERROR == listen(g_pListenContext->m_socket,SOMAXCONN))
	{
		sprintf(strMsg,"listen() execute failed,error code:%d",WSAGetLastError());
		moon_write_error_log(strMsg); 
		RELEASE_SOCKET( g_pListenContext->m_socket );
		return false;
	}

	// The AcceptEx function is used because this is an extension function provided by Microsoft outside of the WinSock2 specification.
	// So you need to get a little bit of a pointer to the function,Gets the AcceptEx function pointer.
	if(SOCKET_ERROR == WSAIoctl(
		g_pListenContext->m_socket, 
		SIO_GET_EXTENSION_FUNCTION_POINTER, 
		&GuidAcceptEx, 
		sizeof(GuidAcceptEx), 
		&g_lpfnAcceptEx, 
		sizeof(g_lpfnAcceptEx), 
		&dwBytes, 
		NULL, 
		NULL))  
	{  
		sprintf(strMsg,"WSAIoctl can not get AcceptEx function point,error code:%d",WSAGetLastError());
		moon_write_error_log(strMsg); 
		dispose();
		return false;  
	}  

	// The same is true for GetAcceptExSockAddrs function Pointers.
	if(SOCKET_ERROR == WSAIoctl(
		g_pListenContext->m_socket, 
		SIO_GET_EXTENSION_FUNCTION_POINTER, 
		&GuidGetAcceptExSockAddrs,
		sizeof(GuidGetAcceptExSockAddrs), 
		&g_lpfnGetAcceptExSockAddrs, 
		sizeof(g_lpfnGetAcceptExSockAddrs),   
		&dwBytes, 
		NULL, 
		NULL))  
	{  
		sprintf(strMsg,"WSAIoctl can not get GuidGetAcceptExSockAddrs function point,error code:%d",WSAGetLastError());
		moon_write_error_log(strMsg); 
		dispose();
		return false;
	}  


	// Prepare the parameters for the AcceptEx and deliver the AcceptEx I/O request.
	for(i=0;i < MAX_POST_ACCEPT;i++ )
	{
		// 新建一个IO_CONTEXT
		pAcceptIoContext = create_new_io_context();
		if( false==post_accept( pAcceptIoContext ) )
		{
			free_io_context(pAcceptIoContext);
			return false;
		}
		//Put the received IO context into the listening list.
		array_list_insert(g_pListenContext->m_pListIOContext,pAcceptIoContext,-1);
	}

	//Using a second mode of reception (blocking)
	/*
	while(WAIT_TIMEOUT == WaitForSingleObject(g_hShutdownEvent, 0))	//when m_killevent is set , server shutdown
	{
		pNewSocketContext = create_new_socket_context();
		addrlen = sizeof(pNewSocketContext->m_client_addr);
		if ((pNewSocketContext->m_socket = WSAAccept(g_pListenContext->m_socket,(SOCKADDR *)&(pNewSocketContext->m_client_addr), &addrlen, NULL, 0)) == SOCKET_ERROR)
		{
			free_socket_context(pNewSocketContext);
			pNewSocketContext = NULL;
			sprintf(strMsg,"WSAAccept() failed with error %d", WSAGetLastError());
			moon_write_error_log(strMsg); 
			return false;
		}
		if(!associateWithIOCP(pNewSocketContext))//将客户端socket绑定到完成端口
		{
			free_socket_context(pNewSocketContext);
			pNewSocketContext = NULL;
			sprintf(strMsg,"WSAAccept() failed with error %d", WSAGetLastError());
			moon_write_error_log(strMsg); 
			return false;
		}
		array_list_insert(g_pListMSClientSocketContext,pNewSocketContext,-1);
		pNewIoContext = create_new_io_context();
		
		pNewIoContext->m_OpType = RECV_POSTED;
		pNewIoContext->m_sockAccept = pNewSocketContext->m_socket;

		//start receive ...
		if (WSARecv(pNewIoContext->m_sockAccept,&(pNewIoContext->m_wsaBuf),1, &dwBytes, &Flags,
			&(pNewIoContext->m_Overlapped), NULL) == SOCKET_ERROR)
		{
			if (WSAGetLastError() != ERROR_IO_PENDING)
			{
				free_io_context(pNewIoContext);
				pNewIoContext = NULL;
				sprintf(strMsg,"WSARecv() failed with error %d", WSAGetLastError());
				moon_write_error_log(strMsg); 
				return false;
			}
		}
		free_io_context(pNewIoContext);
		pNewIoContext = NULL;
	}*/
	return true;
}

bool ms_iocp_server_start(const Moon_Server_Config* p_global_server_config)/*启动IOCP服务*/
{
	DWORD nThreadID;
	if(!load_socket_lib())
	{
		return false;
	}
	// Establish notification of event notification of system exit.
	g_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	//Initialize the client collection.
	g_pListMSClientSocketContext = array_list_init();
	// init IOCP
	if (false == ms_iocp_init())
	{
		moon_write_error_log("init ms_nt_iocp falied");
		return false;
	}
	else
	{
		moon_write_info_log("init ms_nt_iocp finished");
	}
	if (false == ms_init_listen_socket(p_global_server_config))
	{
		moon_write_error_log("init ms_init_listen_socket falied");
		return false;
	}
	else
	{
		moon_write_info_log("init ms_init_listen_socket finished");
	}
	//Open the heartbeat detection thread.
	g_hAliveThread = CreateThread(0, 0, alive_thread, NULL, 0, &nThreadID);
	if (g_hAliveThread == NULL)
	{
		moon_write_error_log("create alive thread failed");
		ms_iocp_server_stop();
		return false;
	}
	return true;
}

/**
 * stop iocp service
 */
void ms_iocp_server_stop()
{
	int i = 0;
	if( g_pListenContext!=NULL && g_pListenContext->m_socket!=INVALID_SOCKET )
	{
		// Activate the shutdown message notification.
		SetEvent(g_hShutdownEvent);
		for (i = 0; i < g_nThreads; i++)
		{
			// Notifies all completion port operations to exit.
			PostQueuedCompletionStatus(g_hIOCompletionPort, 0, (DWORD)EXIT_CODE, NULL);
		}
		// Wait for all client resources to exit
		WaitForMultipleObjects(g_nThreads, g_phWorkerThreads, TRUE, INFINITE);
		// Wait for the heartbeat detection thread to exit.
		WaitForSingleObject(g_hAliveThread,INFINITE);
		// Release other resources
		dispose();
	}	
}

/**
 * @desc iocp send data to client
 * @param socket:client socket
 * @param send_buf:the data of will sent
 * @param len:the data length
 * @return the sent-data length
 **/
int ms_iocp_send(SOCKET socket,char * send_buf,int len)
{
	PMS_IO_CONTEXT pio = (PMS_IO_CONTEXT)moon_malloc(sizeof(MS_IO_CONTEXT));
	pio->m_OpType = SEND_POSTED;
	pio->m_sockAccept = socket;
	pio->m_wsaBuf.buf = send_buf;
	pio->m_wsaBuf.len = len;
	postSend(pio);
	//moon_free(pio);
}
#ifdef __cplusplus
}
#endif
#endif