
#include "ms_nt__iocp.h"
#include "moon_session_manager.h"
#include "ms_socket_context.h"
#include "../module/moon_base.h"
#include "../module/moon_string.h"
#include "../msg/moon_msg.h"
#include "../msg/moon_msg_handle.h"
#include <stdio.h>
#include "../collection/queue.h"
#include "moon_session.h"
#include "../module/moon_time.h"

#ifdef MS_WINDOWS
#include <wchar.h>
#include "ms_socket_context.h"
#include "../module/moon_memory_pool.h"
#endif

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
	//#define RELEASE(x)                      {if(x != NULL ){free(x);x=NULL;}}
	// the macro definition of release handle
#define RELEASE_HANDLE(x)               {if(x != NULL && x!=INVALID_HANDLE_VALUE){ CloseHandle(x);x = NULL;}}
	// the macro definition of release socket
#define RELEASE_SOCKET(x)               {if(x !=INVALID_SOCKET) { closesocket(x);x=INVALID_SOCKET;}}

	static HANDLE	g_hShutdownEvent;// the event of notifies the thread system to exit, in order to better exit the thread.
	static HANDLE g_hIOCompletionPort;//the handle of iocp
	static unsigned int g_nThreads;//the count of thread
	static HANDLE* g_phWorkerThreads;// the handle pointer of work thread
	static PMS_SOCKET_CONTEXT g_pListenContext = NULL;//listen context
	static LPFN_ACCEPTEX                g_lpfnAcceptEx;// the functions pointer of AcceptEx and GetAcceptExSockaddrs，in order to invoke them
	static LPFN_GETACCEPTEXSOCKADDRS    g_lpfnGetAcceptExSockAddrs; 
	extern Queue* p_global_receive_msg_queue;
	extern Moon_Server_Config* p_global_server_config;/*全局配置*/
	static HANDLE g_hAliveThread;//heartbeat detection thread

	/**
	* Heartbeat packet detection
	*/
	bool isSocketAlive(MS_SOCKET_CONTEXT * psocket_context)
	{
		int nByteSent = -1;
		if(psocket_context == NULL) return false;
		nByteSent = ms_iocp_send(psocket_context,"",0);//send a package
		if (-1 == nByteSent) return false;
		return true;
	}

	///////////////////////////////////////////////////////////////////
	// handles errors on the completion port.
	bool handleError( MS_SOCKET_CONTEXT *pContext,const DWORD dwErr )
	{
		char strMsg[256] = {0};
		moon_session* p_moon_session = NULL;
		p_moon_session = get_moon_session_by_client_id(pContext->m_client_id);
		//time out error
		if(WAIT_TIMEOUT == dwErr)  
		{  	
			//Whether the client links
			if(p_moon_session != NULL && !isSocketAlive(pContext))
			{
				remove_session(p_moon_session);
				pContext = NULL;
				return true;
			}
			else
			{
				return true;
			}
		}
		// The client aborted.
		else if(p_moon_session != NULL && ERROR_NETNAME_DELETED==dwErr )
		{
			remove_session(p_moon_session);
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
		OVERLAPPED *p_ol = &pIoContext->m_Overlapped;

		memset(pIoContext->m_szBuffer,0,PKG_BYTE_MAX_LENGTH);
		pIoContext->m_OpType = RECV_POSTED;
		
		//After the initialization is complete, the WSARecv request is delivered.
		nBytesRecv = WSARecv( pIoContext->m_sockAccept, &pIoContext->m_wsaBuf, 1, &dwBytes, &dwFlags, p_ol, NULL );

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
	bool doAccpet(PMS_IO_CONTEXT pIoContext,PMS_SOCKET_CONTEXT   pSocketContext)
	{
		SOCKADDR_IN* ClientAddr = NULL;
		SOCKADDR_IN* LocalAddr = NULL;  
		PMS_SOCKET_CONTEXT pNewSocketContext = NULL;
		PMS_IO_CONTEXT pNewIoContext = NULL;
		int remoteLen = sizeof(SOCKADDR_IN), localLen = sizeof(SOCKADDR_IN);
		//char package_data[PKG_BYTE_MAX_LENGTH] = {0};
		moon_char clientMsg[PKG_BYTE_MAX_LENGTH] = {0};
		int len = 0;
		Message* p_msg = NULL;
		client_environment* p_client_env = NULL;
		moon_session* p_moon_session = NULL;
		char current_datetime[40] = {0};//当前连接时间

		//判断客户端id是否存在，如果存在，那么不需要accept
		if(!moon_string_is_empty(pSocketContext->m_client_id))
		{
			return post_accept( pIoContext );
		}

		//判断连接是否超过了服务允许的最大值
		if(get_socket_context_count() >= p_global_server_config->client_count)
		{
			closesocket(pIoContext->m_sockAccept);
			return post_accept( pIoContext );
		}
		moon_write_debug_log("new connect is comming");

		///////////////////////////////////////////////////////////////////////////
		// 1. First obtain the address information of the client.
		// Not only can you get the client and local address information, but also the first set of data sent by the client.
		g_lpfnGetAcceptExSockAddrs(pIoContext->m_wsaBuf.buf, pIoContext->m_wsaBuf.len - ((sizeof(SOCKADDR_IN)+16)*2),  
			sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN)+16, (LPSOCKADDR*)&LocalAddr, &localLen, (LPSOCKADDR*)&ClientAddr, &remoteLen);

		//parse data packge,the first data must complete,otherwise close socket，网络上传输的数据编码为utf-8
		if(!pkg_is_head((moon_char*)pIoContext->m_szBuffer))
		{
			//close client socket
			closesocket(pIoContext->m_sockAccept);
			return post_accept( pIoContext );
		}
		//parse packge data
		len = parse_pkg((moon_char*)pIoContext->m_szBuffer,clientMsg);

		if(len == 0)
		{
			//close client socket
			closesocket(pIoContext->m_sockAccept);
			return post_accept( pIoContext );
		}


		p_client_env = (client_environment*)moon_malloc(sizeof(client_environment));
		if(p_client_env == NULL)
		{
			//close client socket
			closesocket(pIoContext->m_sockAccept);
			free_msg(p_msg);
			return post_accept( pIoContext );
		}
		memset(p_client_env,0,sizeof(client_environment));
		if(!parse_client_running_environment(clientMsg,p_client_env))
		{
			//close client socket
			closesocket(pIoContext->m_sockAccept);
			free_msg(p_msg);
			moon_free(p_client_env);
			return post_accept( pIoContext );
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////
		// 2.So, notice here, this is the Context in the listening socket, and this Context we also need to use to listen for the next connection.
		// I have to copy the Context on the listening Socket to create a new SocketContext for the new Socket.

		pNewSocketContext = create_new_socket_context();
		pNewSocketContext->m_socket           = pIoContext->m_sockAccept;
		memcpy(&(pNewSocketContext->m_client_addr), ClientAddr, sizeof(SOCKADDR_IN));

		moon_char_copy(pNewSocketContext->m_client_id, p_client_env->client_id);


		// After the parameters are set, the Socket is bound to the completion port.
		if( false==associateWithIOCP( pNewSocketContext ) )
		{
			moon_free(pNewSocketContext);
			return post_accept( pIoContext );
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
			return post_accept(pIoContext);
		}
		array_list_insert(pNewSocketContext->m_pListIOContext,pNewIoContext,-1);

		pNewIoContext = create_new_io_context();
		pNewIoContext->m_OpType       = SEND_POSTED;
		pNewIoContext->m_sockAccept   = pNewSocketContext->m_socket;
		array_list_insert(pNewSocketContext->m_pListIOContext,pNewIoContext,-1);
		/////////////////////////////////////////////////////////////////////////////////////////////////
		// 4. If the delivery is successful, then add this valid client information to the ContextList
		//    (which requires unified management and easy release of resources)
		p_moon_session = (moon_session*)moon_malloc(sizeof(moon_session));
		memset(p_moon_session,0,sizeof(moon_session));
		p_moon_session->p_client_environment = p_client_env;
		p_moon_session->p_socket_context = pNewSocketContext;
		moon_current_time(current_datetime);
		char_to_moon_char(current_datetime,p_moon_session->conn_datetime);
		add_session(p_moon_session);
		

		////////////////////////////////////////////////////////////////////////////////////////////////
		// 5. After use, reset the IoContext of the Listen Socket, and then prepare to deliver the new AcceptEx.
		memset(pIoContext->m_szBuffer,0,PKG_BYTE_MAX_LENGTH);

		return post_accept( pIoContext );
	}

	/////////////////////////////////////////////////////////////////
	// When the received data arrives, it is processed.
	bool doRecv(PMS_IO_CONTEXT pIoContext,PMS_SOCKET_CONTEXT pSocketContext)
	{	
		//recv message
		WSABUF buf;
		moon_char *p_utf8_msg = (moon_char*)pIoContext->m_wsaBuf.buf;
		int len = 0;
		int index = 0;
		int index_data = 0;
		moon_char* utf8_msg = NULL;
		int pos_pkg_head = -1;
		int pos_pkg_tail = -1;
		int pos_temp = -1;
		moon_char stand_pkg_head[9] = {0};
		moon_char stand_pkg_tail[9] = {0};
		char_to_moon_char(PKG_HEAD_FLAG,stand_pkg_head);
		char_to_moon_char(PKG_TAIL_FLAG,stand_pkg_tail);
		//如果不是包头，那么将数据加到当前的socket context缓存中
		//查找包头位置
		while (*p_utf8_msg != '\0') //开始遍历整个数据包，由包头开始
		{
			pos_pkg_head = moon_char_index_of(p_utf8_msg,stand_pkg_head);
			if(pos_pkg_head == -1)//表示不存在包头的数据包，全部写入缓存
			{
				len = moon_char_length(p_utf8_msg);
				for (index = 0;index < len;index++)
				{
					if(pSocketContext->m_currentPkgSize >= PKG_BYTE_MAX_LENGTH) //判断缓存的数据包是否大于等于了最大包长度
					{
						//如果大于，那么将缓存的数据包解析放入消息队列
						pos_pkg_tail = moon_char_index_of(pSocketContext->m_completePkg,stand_pkg_tail);
						while(pos_pkg_tail != -1)
						{
							pos_pkg_tail += PKG_TAIL_LENGTH;
							pos_pkg_tail += 1;
							pos_temp = pos_pkg_tail;
							//将完整包放入消息队列
							utf8_msg = (moon_char*)moon_malloc(pos_pkg_tail * sizeof(moon_char));
							memset(utf8_msg,0,pos_pkg_tail);
							memcpy(utf8_msg,pSocketContext->m_completePkg,pos_pkg_tail);
							Queue_AddToHead(p_global_receive_msg_queue,utf8_msg);
							pSocketContext->m_currentPkgSize = pSocketContext->m_currentPkgSize - pos_pkg_tail + 1;
							//缓存数据移动到最开始位置
							for (index_data = 0;index_data < pSocketContext->m_completePkg[pos_temp] != '\0';index_data++,pos_temp++)
							{
								pSocketContext->m_completePkg[index_data] = pSocketContext->m_completePkg[pos_temp];
							}
							//然后将后面的清空
							memset(pSocketContext->m_completePkg + index_data,0,PKG_BYTE_MAX_LENGTH - index_data);
							pos_pkg_tail = moon_char_index_of(pSocketContext->m_completePkg,stand_pkg_tail);
						}
						
					}
					pSocketContext->m_completePkg[pSocketContext->m_currentPkgSize] = p_utf8_msg[index];
					pSocketContext->m_currentPkgSize++;
				}
				p_utf8_msg += index;
			}
			else
			{
				if (pos_pkg_head > 0)//非包头开始
				{
					for (index = 0;index < len;index++)
					{
						if(pSocketContext->m_currentPkgSize >= PKG_BYTE_MAX_LENGTH) //判断缓存的数据包是否大于等于了最大包长度
						{
							//如果大于，那么将缓存的数据包解析放入消息队列
							pos_pkg_tail = moon_char_index_of(pSocketContext->m_completePkg,stand_pkg_tail);
							while(pos_pkg_tail != -1)
							{
								pos_pkg_tail += PKG_TAIL_LENGTH;
								pos_pkg_tail += 1;
								pos_temp = pos_pkg_tail;
								//将完整包放入消息队列
								utf8_msg = (moon_char*)moon_malloc(pos_pkg_tail * sizeof(moon_char));
								memset(utf8_msg,0,pos_pkg_tail);
								memcpy(utf8_msg,pSocketContext->m_completePkg,pos_pkg_tail);
								Queue_AddToHead(p_global_receive_msg_queue,utf8_msg);
								pSocketContext->m_currentPkgSize = pSocketContext->m_currentPkgSize - pos_pkg_tail + 1;
								//缓存数据移动到最开始位置
								for (index_data = 0;index_data < pSocketContext->m_completePkg[pos_temp] != '\0';index_data++,pos_temp++)
								{
									pSocketContext->m_completePkg[index_data] = pSocketContext->m_completePkg[pos_temp];
								}
								//然后将后面的清空
								memset(pSocketContext->m_completePkg + index_data,0,PKG_BYTE_MAX_LENGTH - index_data);
								pos_pkg_tail = moon_char_index_of(pSocketContext->m_completePkg,stand_pkg_tail);
							}
						}
						pSocketContext->m_completePkg[pSocketContext->m_currentPkgSize] = p_utf8_msg[index];
						pSocketContext->m_currentPkgSize++;
					}
					p_utf8_msg += index;
				}
				//只读取包头
				for (index = 0;index < PKG_HEAD_LENGTH;index++)
				{
					pSocketContext->m_completePkg[pSocketContext->m_currentPkgSize] = p_utf8_msg[index];
					pSocketContext->m_currentPkgSize++;
				}
				p_utf8_msg += index;
			}
			//
			
		}
		//判断缓存的数据中是否存在完整包
		//如果大于，那么将缓存的数据包解析放入消息队列
		pos_pkg_tail = moon_char_index_of(pSocketContext->m_completePkg,stand_pkg_tail);
		while(pos_pkg_tail != -1)
		{
			pos_pkg_tail += PKG_TAIL_LENGTH;
			pos_pkg_tail += 1;
			pos_temp = pos_pkg_tail;
			//将完整包放入消息队列
			utf8_msg = (moon_char*)moon_malloc(pos_pkg_tail * sizeof(moon_char));
			memset(utf8_msg,0,pos_pkg_tail);
			memcpy(utf8_msg,pSocketContext->m_completePkg,pos_pkg_tail);
			Queue_AddToHead(p_global_receive_msg_queue,utf8_msg);
		/*	utf8_msg = (moon_char*)Queue_GetFromTail(p_global_receive_msg_queue);
			if (utf8_msg != NULL)
			{
				moon_free(utf8_msg);
			}*/
			pSocketContext->m_currentPkgSize = pSocketContext->m_currentPkgSize - pos_pkg_tail + 1;
			//缓存数据移动到最开始位置
			for (index_data = 0;pSocketContext->m_completePkg[pos_temp] != '\0';index_data++,pos_temp++)
			{
				pSocketContext->m_completePkg[index_data] = pSocketContext->m_completePkg[pos_temp];
			}
			//然后将后面的清空
			memset(pSocketContext->m_completePkg + index_data,0,PKG_BYTE_MAX_LENGTH - index_data);
			pos_pkg_tail = moon_char_index_of(pSocketContext->m_completePkg,stand_pkg_tail);
		}
		//清空缓存
		memset(pIoContext->m_wsaBuf.buf,0,pIoContext->m_wsaBuf.len);

		return postRecv(pIoContext);
	}

	//////////////////////////////////////////////////////////////////////////
	// post a request for sending data.
	bool doSend(PMS_IO_CONTEXT pIoContext,MS_SOCKET_CONTEXT * psocket_context)
	{
		//记录发送的消息
		//释放缓冲区资源
		if(!stringIsEmpty(pIoContext->m_wsaBuf.buf))
		{
			moon_free(pIoContext->m_wsaBuf.buf);
			pIoContext->m_wsaBuf.buf = NULL;
		}
		return true;
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
		int errnum = -1;

		// The loop processes the request until the Shutdown information is received.
		while (WAIT_OBJECT_0 != WaitForSingleObject(g_hShutdownEvent, 0))
		{
			BOOL bReturn = GetQueuedCompletionStatus(
				g_hIOCompletionPort,
				&dwBytesTransfered,
				(PULONG_PTR)&pSocketContext,
				&pOverlapped,
				INFINITE);
			if (pSocketContext != NULL && dwBytesTransfered != 0)
			{
				_EnterCriticalSection(pSocketContext);
			}
			else
			{
				continue;
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
				_LeaveCriticalSection(pSocketContext);
				if( !handleError(pSocketContext,dwErr))
				{
					break;
				}
				pSocketContext = NULL;
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
					_LeaveCriticalSection(pSocketContext);
					//close socket
					closesocket(pSocketContext->m_socket);
					remove_session(get_moon_session_by_client_id(pSocketContext->m_client_id));
					pSocketContext = NULL;
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
							doAccpet(pIoContext,pSocketContext);
						}
						break;

						// RECV
					case RECV_POSTED:
						{
							// In order to increase the readability of the code, a special _DoRecv function is used to process the receive request.
							pIoContext->m_wsaBuf.len = strlen(pIoContext->m_wsaBuf.buf);
							doRecv(pIoContext,pSocketContext);
						}
						break;

						// SEND
					case SEND_POSTED:
						{
							//当消息成功发送的时候，会调用到这里，这里只是记录消息日志，或者可以不用处理
							doSend(pIoContext,pSocketContext);
						}
						break;
					default:
						//永远不会走到这里，除非有异常
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
		moon_session* p_moon_session = NULL;
		int count_of_sessions = 0;
		while (WAIT_OBJECT_0 != WaitForSingleObject(g_hShutdownEvent, 0))
		{
			count_of_sessions = get_socket_context_count();
			for (i = 0;i < count_of_sessions;i++)
			{
				p_moon_session = (moon_session*)get_moon_session_by_index(i);
				if(p_moon_session != NULL && !isSocketAlive(p_moon_session->p_socket_context))
				{
					remove_session(p_moon_session);
					i--;
				}
			}
			Sleep(500);
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
		g_phWorkerThreads = (HANDLE*)moon_malloc(sizeof(HANDLE) * g_nThreads);

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
		int count_of_sessions = 0;
		// Close the system to exit the event handle.
		RELEASE_HANDLE(g_hShutdownEvent);

		// Release the worker thread handle pointer.
		for(i=0;i<g_nThreads;i++ )
		{
			RELEASE_HANDLE(g_phWorkerThreads[i]);
		}

		moon_free(g_phWorkerThreads);

		//The thread handle that releases the heartbeat packet.
		RELEASE_HANDLE(g_hAliveThread);

		// Close the IOCP handle.
		RELEASE_HANDLE(g_hIOCompletionPort);

		// Close listening Socket
		free_socket_context(g_pListenContext);

		count_of_sessions = get_socket_context_count();
		//Releases the client context collection
		for (i = 0; i < count_of_sessions;)
		{
			remove_session(get_moon_session_by_index(i));
			i = 0;
		}
	}

	/////////////////////////////////////////////////////////////////
	// init Socket
	bool ms_init_listen_socket(const Moon_Server_Config* global_server_config)
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
		ServerAddress.sin_addr.s_addr = inet_addr(global_server_config->server_ip);         
		ServerAddress.sin_port = htons(global_server_config->server_port);                          

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
		//Initialize the socket context manager
		init_session_manager();
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
	* @param size:the data memory length
	* @return the sent-data length，faild return -1
	**/
	int ms_iocp_send(MS_SOCKET_CONTEXT * psocket_context,moon_char * utf8_send_buf,int size)
	{
		DWORD dwFlags = 0;
		DWORD dwBytes = 0;
		int nBytesRecv = 0;
		int index = 0;
		char strMsg[255] = {0};
		PMS_IO_CONTEXT send_pms_io_context = NULL;
		if(psocket_context == NULL) return -1;
		_EnterCriticalSection(psocket_context);
		for(index = 0;psocket_context->m_pListIOContext->length;index++)
		{
			send_pms_io_context = (PMS_IO_CONTEXT)array_list_getAt(psocket_context->m_pListIOContext,index);
			if(send_pms_io_context != NULL && send_pms_io_context->m_OpType == SEND_POSTED)
				break;
			send_pms_io_context = NULL;
		}
		memset(&(send_pms_io_context->m_Overlapped),0,sizeof(OVERLAPPED));
		send_pms_io_context->m_wsaBuf.buf = utf8_send_buf;
		send_pms_io_context->m_wsaBuf.len = size;
		// After the initialization is complete, the request for sending the data is sent to the WSARecv request.
		nBytesRecv = WSASend(psocket_context->m_socket, &(send_pms_io_context->m_wsaBuf), 1, &dwBytes, dwFlags,&(send_pms_io_context->m_Overlapped) , NULL );

		// If the return value error and the error code is not Pending, then this overlapping request fails.
		if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()))
		{
			_LeaveCriticalSection(psocket_context);
			sprintf(strMsg,"post message error,error code:%d",WSAGetLastError());
			moon_write_error_log(strMsg);
			return -1;
		}
		_LeaveCriticalSection(psocket_context);
		return size;
	}
#ifdef __cplusplus
}
#endif
#endif