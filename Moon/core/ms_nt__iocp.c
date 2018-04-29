#include "ms_nt__iocp.h"
#include "ms_socket_context.h"
#include "../module/moon_char.h"
#include <stdio.h>
#include "../module/moon_string.h"

#ifdef MS_WINDOWS
#include <wchar.h>
#endif

#ifdef MS_WINDOWS
#ifdef __cplusplus
extern "C" {
#endif

// 每一个处理器上产生多少个线程(为了最大限度的提升服务器性能，详见配套文档)
#define WORKER_THREADS_PER_PROCESSOR 2
// 同时投递的Accept请求的数量(这个要根据实际的情况灵活设置)
#define MAX_POST_ACCEPT              10
// 传递给Worker线程的退出信号
#define EXIT_CODE                    NULL


// 释放指针和句柄资源的宏

// 释放指针宏
#define RELEASE(x)                      {if(x != NULL ){free(x);x=NULL;}}
// 释放句柄宏
#define RELEASE_HANDLE(x)               {if(x != NULL && x!=INVALID_HANDLE_VALUE){ CloseHandle(x);x = NULL;}}
// 释放Socket宏
#define RELEASE_SOCKET(x)               {if(x !=INVALID_SOCKET) { closesocket(x);x=INVALID_SOCKET;}}
//进入临界区
#define _EnterCriticalSection(x) {if( x != NULL){EnterCriticalSection(&x->SockCritSec);}}
//退出临界区
#define _LeaveCriticalSection(x) {if( x != NULL){LeaveCriticalSection(&x->SockCritSec);}}

static HANDLE	g_hShutdownEvent;// 用来通知线程系统退出的事件，为了能够更好的退出线程
static HANDLE g_hIOCompletionPort;//完成端口的句柄
static unsigned int g_nThreads;//生成线程的数量
static HANDLE* g_phWorkerThreads;// 工作者线程的句柄指针
static PMS_SOCKET_CONTEXT g_pListenContext = NULL;//监听的上下文
static LPFN_ACCEPTEX                g_lpfnAcceptEx;                // AcceptEx 和 GetAcceptExSockaddrs 的函数指针，用于调用这两个扩展函数
static LPFN_GETACCEPTEXSOCKADDRS    g_lpfnGetAcceptExSockAddrs; 
static Array_List* g_pListMSClientSocketContext;//客户端Socket的Context信息
static HANDLE g_hAliveThread;//心跳检测线程



/////////////////////////////////////////////////////////////////////
// 判断客户端Socket是否已经断开，否则在一个无效的Socket上投递WSARecv操作会出现异常
// 使用的方法是尝试向这个socket发送数据，判断这个socket调用的返回值
// 因为如果客户端网络异常断开(例如客户端崩溃或者拔掉网线等)的时候，服务器端是无法收到客户端断开的通知的

bool isSocketAlive(SOCKET s)
{
	int nByteSent=send(s,"",0,0);//发送一个心跳包
	if (-1 == nByteSent) return false;
	return true;
}

///////////////////////////////////////////////////////////////////
// 显示并处理完成端口上的错误
bool handleError( MS_SOCKET_CONTEXT *pContext,const DWORD dwErr )
{
	char strMsg[256] = {0};
	// 如果是超时了，就再继续等吧  
	if(WAIT_TIMEOUT == dwErr)  
	{  	
		// 确认客户端是否还活着...
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
	// 可能是客户端异常退出了
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
// 将句柄(Socket)绑定到完成端口中
bool associateWithIOCP( PMS_SOCKET_CONTEXT pContext )
{
	char strMsg[256] = {0};
	// 将用于和客户端通信的SOCKET绑定到完成端口中
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
// 投递接收数据请求
bool postRecv( PMS_IO_CONTEXT pIoContext )
{
	// 初始化变量
	DWORD dwFlags = 0;
	DWORD dwBytes = 0;
	int nBytesRecv = 0;
	char strMsg[256] = {0};
	WSABUF *p_wbuf   = &pIoContext->m_wsaBuf;
	OVERLAPPED *p_ol = &pIoContext->m_Overlapped;

	memset(pIoContext->m_szBuffer,0,MAX_BUFFER_LEN);
	pIoContext->m_OpType = RECV_POSTED;

	// 初始化完成后，，投递WSARecv请求
	nBytesRecv = WSARecv( pIoContext->m_sockAccept, p_wbuf, 1, &dwBytes, &dwFlags, p_ol, NULL );

	// 如果返回值错误，并且错误的代码并非是Pending的话，那就说明这个重叠请求失败了
	if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()))
	{
		sprintf(strMsg,"post first message error,error code:%d",WSAGetLastError());
		moon_write_error_log(strMsg);
		return false;
	}
	return true;
}

//====================================================================================
//
//				    投递完成端口请求
//
//====================================================================================
//////////////////////////////////////////////////////////////////
// 投递Accept请求
bool post_accept( PMS_IO_CONTEXT pAcceptIoContext )
{
	// 准备参数
	DWORD dwBytes = 0;  
	char strMsg[256] = {0};
	WSABUF *p_wbuf   = &pAcceptIoContext->m_wsaBuf;
	OVERLAPPED *p_ol = &pAcceptIoContext->m_Overlapped;
	pAcceptIoContext->m_OpType = ACCEPT_POSTED;

	// 为以后新连入的客户端先准备好Socket( 这个是与传统accept最大的区别 ) 
	pAcceptIoContext->m_sockAccept  = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if(INVALID_SOCKET==pAcceptIoContext->m_sockAccept)  
	{  
		sprintf(strMsg,"create accept socket failed,error code:%d",WSAGetLastError());
		moon_write_error_log(strMsg);
		return false;
	} 

	// 投递AcceptEx
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
// 在有客户端连入的时候，进行处理
// 流程有点复杂，你要是看不懂的话，就看配套的文档吧....
// 如果能理解这里的话，完成端口的机制你就消化了一大半了

// 总之你要知道，传入的是ListenSocket的Context，我们需要复制一份出来给新连入的Socket用
// 原来的Context还是要在上面继续投递下一个Accept请求
//
bool doAccpet( PMS_SOCKET_CONTEXT pSocketContext, PMS_IO_CONTEXT pIoContext )
{
	
	SOCKADDR_IN* ClientAddr = NULL;
	SOCKADDR_IN* LocalAddr = NULL;  
	PMS_SOCKET_CONTEXT pNewSocketContext = NULL;
	PMS_IO_CONTEXT pNewIoContext = NULL;
	int remoteLen = sizeof(SOCKADDR_IN), localLen = sizeof(SOCKADDR_IN);
	moon_char clientMsg[MAX_BUFFER_LEN] = {0};
	int len = 0;
	
	///////////////////////////////////////////////////////////////////////////
	// 1. 首先取得连入客户端的地址信息
	// 这个 m_lpfnGetAcceptExSockAddrs 不得了啊~~~~~~
	// 不但可以取得客户端和本地端的地址信息，还能顺便取出客户端发来的第一组数据，老强大了...
	g_lpfnGetAcceptExSockAddrs(pIoContext->m_wsaBuf.buf, pIoContext->m_wsaBuf.len - ((sizeof(SOCKADDR_IN)+16)*2),  
		sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN)+16, (LPSOCKADDR*)&LocalAddr, &localLen, (LPSOCKADDR*)&ClientAddr, &remoteLen);
	//解析客户端第一次传来的信息
	len = moon_ms_windows_utf8_to_unicode(pIoContext->m_szBuffer,clientMsg);//将收到的utf-8的字节序转化为unicode

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// 2. 这里需要注意，这里传入的这个是ListenSocket上的Context，这个Context我们还需要用于监听下一个连接
	// 所以我还得要将ListenSocket上的Context复制出来一份为新连入的Socket新建一个SocketContext

	pNewSocketContext = create_new_socket_context();
	pNewSocketContext->m_socket           = pIoContext->m_sockAccept;
	memcpy(&(pNewSocketContext->m_client_addr), ClientAddr, sizeof(SOCKADDR_IN));

	// 参数设置完毕，将这个Socket和完成端口绑定(这也是一个关键步骤)
	if( false==associateWithIOCP( pNewSocketContext ) )
	{
		RELEASE(pNewSocketContext);
		return false;
	} 

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// 3. 继续，建立其下的IoContext，用于在这个Socket上投递第一个Recv数据请求
	pNewIoContext = create_new_io_context();
	pNewIoContext->m_OpType       = RECV_POSTED;
	pNewIoContext->m_sockAccept   = pNewSocketContext->m_socket;
	// 如果Buffer需要保留，就自己拷贝一份出来
	//memcpy( pNewIoContext->m_szBuffer,pIoContext->m_szBuffer,MAX_BUFFER_LEN );

	// 绑定完毕之后，就可以开始在这个Socket上投递完成请求了
	if( false==postRecv( pNewIoContext) )
	{
		array_list_remove(pNewSocketContext->m_pListIOContext,pNewIoContext);
		free_io_context(pNewIoContext);
		free_socket_context(pNewSocketContext);
		return false;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////
	// 4. 如果投递成功，那么就把这个有效的客户端信息，加入到ContextList中去(需要统一管理，方便释放资源)
	array_list_insert(g_pListMSClientSocketContext,pNewSocketContext,-1);

	////////////////////////////////////////////////////////////////////////////////////////////////
	// 5. 使用完毕之后，把Listen Socket的那个IoContext重置，然后准备投递新的AcceptEx
	memset(pIoContext->m_szBuffer,0,MAX_BUFFER_LEN);
	return post_accept( pIoContext );
}

/////////////////////////////////////////////////////////////////
// 在有接收的数据到达的时候，进行处理
bool doRecv( PMS_SOCKET_CONTEXT pSocketContext, PMS_IO_CONTEXT pIoContext )
{
	// 先把上一次的数据显示出现，然后就重置状态，发出下一个Recv请求
	//SOCKADDR_IN* ClientAddr = &pSocketContext->m_client_addr;
	//this->_ShowMessage( _T("收到  %s:%d 信息：%s"),inet_ntoa(ClientAddr->sin_addr), ntohs(ClientAddr->sin_port),pIoContext->m_wsaBuf.buf );

	// 然后开始投递下一个WSARecv请求
	return postRecv( pIoContext );
}

//////////////////////////////////////////////////////////////////////////
// 投递发送数据的请求
bool postSend(PMS_IO_CONTEXT pIoContext)
{
	// 初始化变量
	DWORD dwFlags = 0;
	DWORD dwBytes = 0;
	int nBytesRecv = 0;
	char strMsg[256] = {0};
	WSABUF *p_wbuf   = &pIoContext->m_wsaBuf;
	OVERLAPPED *p_ol = &pIoContext->m_Overlapped;
	pIoContext->m_OpType = SEND_POSTED;
	//memset(pIoContext->m_szBuffer,0,MAX_BUFFER_LEN);
	p_wbuf->len = strlen(p_wbuf->buf);
	// 初始化完成后，，投递WSARecv请求
	nBytesRecv = WSASend( pIoContext->m_sockAccept, p_wbuf, 1, &dwBytes, dwFlags, p_ol, NULL );

	// 如果返回值错误，并且错误的代码并非是Pending的话，那就说明这个重叠请求失败了
	if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()))
	{
		//this->_ShowMessage("投递第一个WSARecv失败！");
		sprintf(strMsg,"post first message error,error code:%d",WSAGetLastError());
		moon_write_error_log(strMsg);
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
//在有发送的数据的时候进行处理
bool doSend(PMS_SOCKET_CONTEXT pSocketContext, PMS_IO_CONTEXT pIoContext)
{
	return postSend(pIoContext);
}

///////////////////////////////////////////////////////////////////
// 工作者线程：  为IOCP请求服务的工作者线程
//         也就是每当完成端口上出现了完成数据包，就将之取出来进行处理的线程
///////////////////////////////////////////////////////////////////
DWORD static WINAPI worker_thread(LPVOID lpParam)
{
	OVERLAPPED           *pOverlapped = NULL;
	PMS_SOCKET_CONTEXT   pSocketContext = NULL;
	DWORD                dwBytesTransfered = 0;

	// 循环处理请求，知道接收到Shutdown信息为止
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
		// 如果收到的是退出标志，则直接退出
		if ( EXIT_CODE==(DWORD)pSocketContext )
		{
			if (pSocketContext != NULL)
			{
				_LeaveCriticalSection(pSocketContext);
			}
			break;
		}

		// 判断是否出现了错误
		if( !bReturn )  
		{  
			DWORD dwErr = GetLastError();

			// 显示一下提示信息
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
			// 读取传入的参数
			MS_IO_CONTEXT* pIoContext = CONTAINING_RECORD(pOverlapped, MS_IO_CONTEXT, m_Overlapped);  

			// 判断是否有客户端断开了
			if((0 == dwBytesTransfered) && ( RECV_POSTED==pIoContext->m_OpType || SEND_POSTED==pIoContext->m_OpType))  
			{  
				// 释放掉对应的资源
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
						// 为了增加代码可读性，这里用专门的_DoAccept函数进行处理连入请求
						doAccpet( pSocketContext, pIoContext );
					}
					break;

					// RECV
				case RECV_POSTED:
					{
						// 为了增加代码可读性，这里用专门的_DoRecv函数进行处理接收请求
						doRecv( pSocketContext,pIoContext );
						//接收到消息之后给客户端回传消息
						strcpy(pIoContext->m_wsaBuf.buf,"server");
						doSend(pSocketContext,pIoContext);
					}
					break;

					// SEND
				case SEND_POSTED:
					{
						//moon_write_info_log("开始发送消息:");
						//moon_write_info_log(pIoContext->m_szBuffer);
					}
					break;
				default:
					// 不应该执行到这里
					//TRACE(_T("_WorkThread中的 pIoContext->m_OpType 参数异常.\n"));
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
 *  心跳包检测线程
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
			if(!isSocketAlive(pSocketContext->m_socket))
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
// 初始化WinSock 2.2
bool static load_socket_lib()
{    
	WSADATA wsaData;
	int nResult;
	char strMsg[256] = {0};
	nResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	// 错误(一般都不可能出现)
	if (NO_ERROR != nResult)
	{
		sprintf(strMsg,"init winsock 2.2 falied,error code:%d",WSAGetLastError());
		moon_write_error_log(strMsg);
		return false; 
	}

	return true;
}

///////////////////////////////////////////////////////////////////
// 获得本机中处理器的数量
int getNoOfProcessors()
{
	SYSTEM_INFO si;

	GetSystemInfo(&si);

	return si.dwNumberOfProcessors;
}

////////////////////////////////
// 初始化完成端口
bool ms_iocp_init()
{
	int i = 0;
	DWORD nThreadID;
	char strMsg[256] = {0};
	// 建立第一个完成端口
	g_hIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0 );

	if ( NULL == g_hIOCompletionPort)
	{
		sprintf(strMsg,"create Completion port falied,error code:%d",WSAGetLastError());
		moon_write_error_log(strMsg);
		return false;
	}

	// 根据本机中的处理器数量，建立对应的线程数
	g_nThreads = WORKER_THREADS_PER_PROCESSOR * getNoOfProcessors();

	// 为工作者线程初始化句柄
	g_phWorkerThreads = (HANDLE*)malloc(sizeof(HANDLE) * g_nThreads);

	// 根据计算出来的数量建立工作者线程
	for (i=0; i < g_nThreads; i++)
	{
		g_phWorkerThreads[i] = CreateThread(0, 0, worker_thread, NULL, 0, &nThreadID);
	}

	return true;
}

////////////////////////////////////////////////////////////
//	最后释放掉所有资源
void dispose()
{
	int i = 0;
	// 关闭系统退出事件句柄
	RELEASE_HANDLE(g_hShutdownEvent);

	// 释放工作者线程句柄指针
	for(i=0;i<g_nThreads;i++ )
	{
		RELEASE_HANDLE(g_phWorkerThreads[i]);
	}

	RELEASE(g_phWorkerThreads);

	//释放心跳包的线程句柄
	RELEASE_HANDLE(g_hAliveThread);

	// 关闭IOCP句柄
	RELEASE_HANDLE(g_hIOCompletionPort);

	// 关闭监听Socket
	free_socket_context(g_pListenContext);

	//释放客户端上下文集合
	for (i = 0; i < g_pListMSClientSocketContext->length;)
	{
		free_socket_context((PMS_SOCKET_CONTEXT)array_list_getAt(g_pListMSClientSocketContext,i));
		i = 0;
	}
	array_list_free(g_pListMSClientSocketContext);
}

/////////////////////////////////////////////////////////////////
// 初始化Socket
bool ms_init_listen_socket(const Moon_Server_Config* p_global_server_config)
{
	// AcceptEx 和 GetAcceptExSockaddrs 的GUID，用于导出函数指针
	GUID GuidAcceptEx = WSAID_ACCEPTEX;  
	GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS; 
	char strMsg[256] = {0};
	PMS_SOCKET_CONTEXT pNewSocketContext = NULL;
	PMS_IO_CONTEXT pNewIoContext = NULL;
	int i = 0 ;
	int addrlen = 0;
	DWORD dwBytes = 0; 
	DWORD	Flags = 0;
	// 服务器地址信息，用于绑定Socket
	struct sockaddr_in ServerAddress;

	// 生成用于监听的Socket的信息
	g_pListenContext = create_new_socket_context();

	// 需要使用重叠IO，必须得使用WSASocket来建立Socket，才可以支持重叠IO操作
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

	// 将Listen Socket绑定至完成端口中
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

	// 填充地址信息
	ZeroMemory((char *)&ServerAddress, sizeof(ServerAddress));
	ServerAddress.sin_family = AF_INET;
	// 这里可以绑定任何可用的IP地址，或者绑定一个指定的IP地址 
	//ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);                      
	ServerAddress.sin_addr.s_addr = inet_addr(p_global_server_config->server_ip);         
	ServerAddress.sin_port = htons(p_global_server_config->server_port);                          

	// 绑定地址和端口
	if (SOCKET_ERROR == bind(g_pListenContext->m_socket, (struct sockaddr *) &ServerAddress, sizeof(ServerAddress))) 
	{
		sprintf(strMsg,"bind() execute failed,error code:%d",WSAGetLastError());
		moon_write_error_log(strMsg); 
		RELEASE_SOCKET( g_pListenContext->m_socket );
		return false;
	}

	// 开始进行监听
	if (SOCKET_ERROR == listen(g_pListenContext->m_socket,SOMAXCONN))
	{
		sprintf(strMsg,"listen() execute failed,error code:%d",WSAGetLastError());
		moon_write_error_log(strMsg); 
		RELEASE_SOCKET( g_pListenContext->m_socket );
		return false;
	}

	// 使用AcceptEx函数，因为这个是属于WinSock2规范之外的微软另外提供的扩展函数
	// 所以需要额外获取一下函数的指针，
	// 获取AcceptEx函数指针
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

	// 获取GetAcceptExSockAddrs函数指针，也是同理
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


	// 为AcceptEx 准备参数，然后投递AcceptEx I/O请求
	for(i=0;i < MAX_POST_ACCEPT;i++ )
	{
		// 新建一个IO_CONTEXT
		PMS_IO_CONTEXT pAcceptIoContext = create_new_io_context();
		if( false==post_accept( pAcceptIoContext ) )
		{
			free_io_context(pAcceptIoContext);
			return false;
		}
		//将接收的IO上下文放入监听列表
		array_list_insert(g_pListenContext->m_pListIOContext,pAcceptIoContext,-1);
	}

	//使用第二种接收方式(阻塞)
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
	// 建立系统退出的事件通知
	g_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	//初始化客户端集合
	g_pListMSClientSocketContext = array_list_init();
	// 初始化IOCP
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
	//开启心跳检测线程
	g_hAliveThread = CreateThread(0, 0, alive_thread, NULL, 0, &nThreadID);
	if (g_hAliveThread == NULL)
	{
		moon_write_error_log("create alive thread failed");
		ms_iocp_server_stop();
		return false;
	}
	return true;
}

void ms_iocp_server_stop()/*停止IOCP服务*/
{
	int i = 0;
	if( g_pListenContext!=NULL && g_pListenContext->m_socket!=INVALID_SOCKET )
	{
		// 激活关闭消息通知
		SetEvent(g_hShutdownEvent);
		for (i = 0; i < g_nThreads; i++)
		{
			// 通知所有的完成端口操作退出
			PostQueuedCompletionStatus(g_hIOCompletionPort, 0, (DWORD)EXIT_CODE, NULL);
		}
		// 等待所有的客户端资源退出
		WaitForMultipleObjects(g_nThreads, g_phWorkerThreads, TRUE, INFINITE);
		// 等待心跳检测线程退出
		WaitForSingleObject(g_hAliveThread,INFINITE);
		// 释放其他资源
		dispose();
	}	
}
#ifdef __cplusplus
}
#endif
#endif