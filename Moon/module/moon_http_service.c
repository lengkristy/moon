#include "../cfg/environment.h"
#include "moon_http_service.h"
#include "moon_config_struct.h"
#include "moon_thread_pool.h"
#include "module_log.h"
#include "moon_string.h"
#include "moon_memory_pool.h"
#include "../core/socket_context_manager.h"
#include <stdio.h>
#ifdef MS_WINDOWS
#include <direct.h>
#include <Strsafe.h>
#include <winbase.h>
#endif
#ifdef __cplusplus
extern "C" {
#endif

#define RECV_BUF_LENGTH 1024 //define receipt data package buffer size
#define MIN_BUF 128 //min buffer size
#define RESPONSE_BUF_LENGTH 1024*10 //response buffer size
#define SEND_FILE_SIZE 1024 //send a single file size

	extern Moon_Server_Config* p_global_server_config;//global configuration

	static Array_List* pThreads = NULL;//thread list

	static bool g_bEndListen = false;//has end listen

	
	/************************************************************************/
	/* 获取请求成功的协议头字符串                                           */
	/*	参数：pOutHead 输出的字符串											*/
	/*		  sendContextLength 发送文本大小								*/
	/*	返回值：返回输出字符串的大小										*/
	/************************************************************************/
	int moon_http_request_success_200(char* pOutHead,long sendContextLength)
	{
		sprintf(pOutHead,"HTTP/1.1 200 OK\nConnection: keep-alive\nServer: Moon_Http_Service/1.1\nContent-Length: %ld\nContent-Type: text/html\n\n",sendContextLength);
		return moon_get_string_length(pOutHead);
	}

	/************************************************************************/
	/* 获取请求成功的协议头，用于图片请求                                           */
	/*	参数：pOutHead 输出的字符串											*/
	/*		  sendContextLength 发送文本大小								*/
	/*	返回值：返回输出字符串的大小										*/
	/************************************************************************/
	int moon_http_response_head_image(char* pOutHead,long sendContextLength,char* extension)
	{
		char tmp[20] = {0};
		moon_to_capital(extension,tmp);
		if (strcmp("JPG",tmp) == 0 || strcmp("JPEG",tmp) == 0)
		{
			sprintf(pOutHead,"HTTP/1.1 200 OK\nConnection: keep-alive\nServer: Moon_Http_Service/1.1\nContent-Transfer-Encoding: binary\nContent-Length: %ld\nContent-Type: image/jpeg\n\n",sendContextLength);
		}
		else if (strcmp("PNG",tmp) == 0)
		{
			sprintf(pOutHead,"HTTP/1.1 200 OK\nConnection: keep-alive\nServer: Moon_Http_Service/1.1\nContent-Transfer-Encoding: binary\nContent-Length: %ld\nContent-Type: image/PNG\n\n",sendContextLength);
		}
		else if (strcmp("BMP",tmp) == 0)
		{
			sprintf(pOutHead,"HTTP/1.1 200 OK\nConnection: keep-alive\nServer: Moon_Http_Service/1.1\nContent-Transfer-Encoding: binary\nContent-Length: %ld\nContent-Type: image/BMP\n\n",sendContextLength);
		}
		return moon_get_string_length(pOutHead);
	}



/*********************************************************************************************************
	 以下是关于web浏览器网页请求处理
**********************************************************************************************************/

	/************************************************************************/
	/*浏览器请求http服务发生错误                                            */
	/*	pOutContent:输出内容												*/
	/*	errmsg：错误内容													*/
	/************************************************************************/
	int moon_http_browser_request_error(char* pOutContent,char* errmsg)
	{
		char tmpHtml[1024] = {0};
		sprintf(pOutContent,"<html xmlns=http://www.w3.org/1999/xhtml>");
		strcat(pOutContent,"<head><title>moon http server</title></head>");
		strcat(pOutContent,"<body>");
		strcat(pOutContent,"<div align='center'><h1>moon server has error</h1></div>");
		sprintf(tmpHtml,"<div align='center'>%s</div>",errmsg);
		strcat(pOutContent,tmpHtml);
		strcat(pOutContent,"</body>");
		strcat(pOutContent,"</html>");
		return moon_get_string_length(pOutContent);
	}

	/************************************************************************/
	/* 浏览器首页请求                                                       */
	/*	参数：pOutContent 输出内容											*/
	/*	返回值：返回输出内容长度											*/
	/************************************************************************/
	int moon_http_browser_request_index(char* pOutContent)
	{
		char tmpHtml[1024] = {0};
		sprintf(pOutContent,"<html xmlns=http://www.w3.org/1999/xhtml>");
		strcat(pOutContent,"<head><title>moon http server</title>");
		strcat(pOutContent,"<style type=\"text/css\">");
		strcat(pOutContent,"table.altrowstable { ");
		strcat(pOutContent,"    font-family: verdana,arial,sans-serif;");
		strcat(pOutContent,"    font-size:11px;");
		strcat(pOutContent,"    color:#333333;");
		strcat(pOutContent,"    border-width: 1px;");
		strcat(pOutContent,"    border-color: #a9c6c9;");
		strcat(pOutContent,"    border-collapse: collapse;");
		strcat(pOutContent,"    margin: auto;");
		strcat(pOutContent,"}");
		strcat(pOutContent,"table.altrowstable th { ");
		strcat(pOutContent,"    border-width: 1px;");
		strcat(pOutContent,"    padding: 8px;");
		strcat(pOutContent,"    border-style: solid;");
		strcat(pOutContent,"    border-color: #a9c6c9;");
		strcat(pOutContent,"}");
		strcat(pOutContent,"table.altrowstable td { ");
		strcat(pOutContent,"    border-width: 1px;");
		strcat(pOutContent,"    padding: 8px;");
		strcat(pOutContent,"    border-style: solid;");
		strcat(pOutContent,"    border-color: #a9c6c9;");
		strcat(pOutContent,"}");
		strcat(pOutContent,".oddrowcolor{ ");
		strcat(pOutContent,"    background-color:#d4e3e5;");
		strcat(pOutContent,"}");
		strcat(pOutContent,".evenrowcolor{ ");
		strcat(pOutContent,"    background-color:#c3dde0;");
		strcat(pOutContent,"}");
		strcat(pOutContent,"</style>");
		strcat(pOutContent,"</head>");
		strcat(pOutContent,"<body bgcolor='#3299CC'>");
		strcat(pOutContent,"<div align='center'><h1>Moon Server Node Info</h1></div>");
		memset(tmpHtml,0,1024);
		sprintf(tmpHtml,"<div><image style='width:100px;height:100px;' src='http://%s:%d/image/748x578.jpg?platform=b'/></div>",p_global_server_config->server_ip,p_global_server_config->http_port);
		strcat(pOutContent,tmpHtml);
		sprintf(tmpHtml,"<div><table  class=\"altrowstable\">                \
			                  <tr><th>name</th><th>value</th></tr>              \
			                  <tr><td>Moon Server IP</td><td>%s</td>         \
							  <tr><td>Moon Port</td><td>%d</td>              \
							  <tr><td>current client count</td><td>%d</td>   \
					    </div>",p_global_server_config->server_ip,p_global_server_config->server_port,get_socket_context_count());
		strcat(pOutContent,tmpHtml);
		memset(tmpHtml,0,1024);
		strcat(pOutContent,"</body>");
		strcat(pOutContent,"</html>");
		return moon_get_string_length(pOutContent);
	}


/*****************************************************************************************************************/


	/************************************************************************/
	/* 将请求的url的路径和参数分隔开                                        */
	/*   参数：srcUrl:请求的url完整字符串									*/
	/*		   pOutUrlPath:输出的路径信息									*/
	/*		   pOutParam：输出的参数信息									*/
	/************************************************************************/
	void moon_http_splite_url_and_param(char* srcUrl,char* pOutUrlPath,char* pOutParam)
	{
		int i = 0;
		int iParam = 0;
		if (srcUrl == NULL)
		{
			return;
		}
		while (srcUrl[i] != '\0' && srcUrl[i] != '?')//截取urlpath
		{
			pOutUrlPath[i] = srcUrl[i];
			i++;
		}
		//填充\0
		pOutUrlPath[i] = '\0';
		while (srcUrl[i] != '\0')//截取参数
		{
			if (srcUrl[i] != '?')
			{
				pOutParam[iParam] = srcUrl[i];
				iParam++;
			}
			i++;
		}
		pOutParam[iParam] = '\0';
	}

#ifdef MS_WINDOWS

	static HANDLE g_hListener = NULL;//监听线程句柄

	static DWORD g_dListenerId = 0;//监听线程ID

	static SOCKET g_sListener=NULL;//监听套接字

	/************************************************************************/
	/* 浏览器请求文件（图片等信息）                                         */
	/*	参数：pOutContent:输出图片流										*/
	/*		  path:请求的图片路径(相对路径)  								*/
	/*		  clientSock:发送数据包			  								*/
	/************************************************************************/
	long moon_win32_http_browser_request_file(char* pOutContent,char* path,SOCKET clientSock)
	{
		int responseHeadLength = 0;
		char responseHead[1024] = {0};
		long imageLength = 0;
		char imagePath[1024] = {0};
		FILE *resource = NULL;
		char errmsg[126] = {0};
		char tmpPath[1024] = {0};//存放大写路径，用于寻找后缀
		char imageExt[20] = {0};//图片扩展名
		char send_buf[SEND_FILE_SIZE] = {0};//每次发送文件的大小
		// 将请求的url路径转换为本地路径
		_getcwd(imagePath,_MAX_PATH);
		strcat(imagePath,path);
		// 打开本地路径下的文件，网络传输中用r文本方式打开会出错
		resource = fopen(imagePath,"rb");
		//如果图片不存在，则返回错误信息
		if(resource==NULL)
		{
			return moon_http_browser_request_error(pOutContent,"request file not found");
		}
		//求文件长度
		fseek(resource,0,SEEK_SET);
		fseek(resource,0,SEEK_END);
		imageLength = ftell(resource);
		fseek(resource,0,SEEK_SET);//重置文件指针
		//判断图片格式
		moon_to_capital(imagePath,tmpPath);
		if (strstr(imagePath,"JPG") != NULL || strstr(imagePath,"JPEG") != NULL)
		{
			strcpy(imageExt,"JPG");
		}
		else if (strstr(imagePath,"PNG") != NULL)
		{
			strcpy(imageExt,"PNG");
		}
		else if (strstr(imagePath,"BMP") != NULL)
		{
			strcpy(imageExt,"BMP");
		}
		responseHeadLength = moon_http_response_head_image(responseHead,imageLength,imageExt);//发送成功的响应头部
		send(clientSock,responseHead,responseHeadLength,0);//头部
		//开始发送图片文件
		while (1)
		{
			memset(send_buf,0,SEND_FILE_SIZE); //缓存清0
			imageLength = fread(send_buf,1,SEND_FILE_SIZE,resource);
			if (SOCKET_ERROR == send(clientSock, send_buf, imageLength, 0))
			{
				sprintf(errmsg,"send() Failed:%d",WSAGetLastError());
				moon_write_error_log(errmsg);
				break;
			}
			if(feof(resource))
				break;
		}   
		if (resource != NULL)
		{
			fclose(resource);
		}
		return 0;
	}

	/************************************************************************/
	/* 处理http响应                                                         */
	/*	参数：url 请求的url字符串											*/
	/*		  pOutContent 响应的内容										*/
	/*		  clientSock 用于发送文件等大数据								*/
	/*	返回值：返回响应内容字符串的长度									*/
	/************************************************************************/
	long moon_win32_http_response(char* url,char* pOutContent,SOCKET clientSock)
	{
		char requestPath[1024] = {0};//请求路径
		char requestParam[1024] = {0};//请求参数
		//将url转成大写
		char urlTmp[MIN_BUF] = {0};
		char logMsg[1200] = {0};//日志信息
		sprintf(logMsg,"current request url path:%s",url);
		moon_write_debug_log(logMsg);
		if (strcmp("\\",url) == 0)//表示浏览器请求首页
		{
			return moon_http_browser_request_index(pOutContent);
		}
		else //解析浏览器参数
		{
			moon_http_splite_url_and_param(url,requestPath,requestParam);
			memset(urlTmp,0,MIN_BUF);
			moon_to_capital(requestParam,urlTmp);//将请求的url路径转成大写，好做判断
			//判断参数是否为空，或者请求平台为空
			if(stringIsEmpty(requestParam) || strstr(urlTmp,"PLATFORM=") == NULL)
			{
				return moon_http_browser_request_error(pOutContent,"request url platform is null");
			}
			memset(urlTmp,0,MIN_BUF);
			moon_to_capital(requestPath,urlTmp);//将请求的url路径转成大写，好做判断
			if(strstr(urlTmp,".JPG") != NULL || strstr(urlTmp,".PNG") != NULL || strstr(urlTmp,".BMP") != NULL|| strstr(urlTmp,".JPEG") != NULL)//如果请求图片
			{
				return moon_win32_http_browser_request_file(pOutContent,urlTmp,clientSock);
			}
		}
		return moon_get_string_length(pOutContent);
	}

	//请求处理
	VOID CALLBACK moon_win32_http_request(PTP_CALLBACK_INSTANCE instance,PVOID context,PTP_WORK work)  
	{  
		moon_http_context* pContext = (moon_http_context*)context;
		SOCKET sAccept = pContext->m_socket;
		char recv_buf[RECV_BUF_LENGTH] = {0}; 
		char method[MIN_BUF] = {0};
		char url[MIN_BUF] = {0};
		char strMsg[256] = {0};
		int i, j;
		char responseHead[1024] = {0};//响应头部
		int responseHeadLength = 0;//响应头部长度
		char responseError[1024] = {0};//响应错误
		int responseErrorLength = 0;//响应错误长度
		char* pResponseContent = NULL;//响应内容
		long responseLength = 0;//响应内容长度
		if (recv(sAccept,recv_buf,sizeof(recv_buf),0) == SOCKET_ERROR)   //接收错误
		{
			sprintf(strMsg,"http service request at recv() Failed:%d",WSAGetLastError());
			moon_write_error_log(strMsg);
			return;
		}
		i = 0; j = 0;
		
		// 取出第一个单词，一般为HEAD、GET、POST
		while (!(' ' == recv_buf[j]) && (i < sizeof(method) - 1))
		{
			method[i] = recv_buf[j];
			i++; j++;
		}
		method[i] = '\0';   // 结束符，这里也是初学者很容易忽视的地方
		// 如果不是GET或HEAD方法，则直接断开本次连接
		// 如果想做的规范些可以返回浏览器一个501未实现的报头和页面
		if (stricmp(method, "GET") && stricmp(method, "HEAD"))
		{
			if (sAccept != NULL)
			{
				closesocket(sAccept); //释放连接套接字，结束与该客户的通信
				sAccept = NULL;
			}
			moon_write_debug_log("http request is not get or head method.close ok.");
			return;
		}

		// 提取出第二个单词(url文件路径，空格结束)，并把'/'改为windows下的路径分隔符'\'
		// 这里只考虑静态请求(比如url中出现'?'表示非静态，需要调用CGI脚本，'?'后面的字符串表示参数，多个参数用'+'隔开
		// 例如：www.csr.com/cgi_bin/cgi?arg1+arg2 该方法有时也叫查询，早期常用于搜索)
		i = 0;
		while ((' ' == recv_buf[j]) && (j < sizeof(recv_buf)))
			j++;
		while (!(' ' == recv_buf[j]) && (i < sizeof(recv_buf) - 1) && (j < sizeof(recv_buf)))
		{
			if (recv_buf[j] == '/')
				url[i] = '\\';
			else if(recv_buf[j] == ' ')
				break;
			else
				url[i] = recv_buf[j];
			i++; j++;
		}
		url[i] = '\0';
		
		if (pResponseContent == NULL)
		{
			pResponseContent = (char*)moon_malloc(RESPONSE_BUF_LENGTH * sizeof(char));
			if (pResponseContent == NULL)
			{
				moon_write_error_log("moon_malloc falied");
				//向客户端发送服务器错误的信息
				//判断是浏览器请求还是moonManager请求
			}
		}
		responseLength = moon_win32_http_response(url,pResponseContent,sAccept);//如果有响应信息才发送数据包
		if (responseLength > 0)
		{
			responseHeadLength = moon_http_request_success_200(responseHead,responseLength);
			send(sAccept,responseHead,responseHeadLength,0);//发送200，请求成功的头部
			send(sAccept,pResponseContent,responseLength, 0);//发送响应内容
		}
		closesocket(sAccept); //释放连接套接字，结束与该客户的通信
		sAccept = NULL;
		moon_free(pResponseContent);
		pResponseContent = NULL;
	} 


	//监听线程
	DWORD WINAPI moon_win32_http_listen(LPVOID lpParameter)
	{
		char strMsg[256] = {0};
		WSADATA wsaData;
		SOCKET sAccept;        //服务器监听套接字，连接套接字
		moon_http_context context;//http上下文
		int serverport=p_global_server_config->http_port;   //服务器端口号
		struct sockaddr_in ser,cli;   //服务器地址，客户端地址
		int iLen;
		PTP_POOL tPool;
		TP_CALLBACK_ENVIRON pcbe;
		//第一步：加载协议栈
		if (WSAStartup(MAKEWORD(2,2),&wsaData) !=0)
		{
			moon_write_error_log("Failed to load Winsock.");
			return -1;
		}
		//第二步：创建监听套接字，用于监听客户请求
		g_sListener =socket(AF_INET,SOCK_STREAM,0);
		if (g_sListener == INVALID_SOCKET)
		{
			sprintf(strMsg,"http service socket() Failed:%d",WSAGetLastError());
			moon_write_error_log(strMsg);
			return -1;
		}
		//创建服务器地址：IP+端口号
		ser.sin_family=AF_INET;
		ser.sin_port=htons(serverport);               //服务器端口号
		ser.sin_addr.s_addr=inet_addr(p_global_server_config->server_ip);   //服务器IP地址

		//第三步：绑定监听套接字和服务器地址
		if (bind(g_sListener,(LPSOCKADDR)&ser,sizeof(ser))==SOCKET_ERROR)
		{
			sprintf(strMsg,"http service blind() Failed:%d",WSAGetLastError());
			moon_write_error_log(strMsg);
			return -1;
		}
		//第五步：通过监听套接字进行监听
		if (listen(g_sListener,5)==SOCKET_ERROR)
		{
			sprintf(strMsg,"http service listen() Failed:%d",WSAGetLastError());
			moon_write_error_log(strMsg);
			return -1;
		}
		//创建线程池
		tPool = CreateThreadpool(NULL);
		//设置线程池最大最小的线程数量
		SetThreadpoolThreadMinimum(tPool,5);
		SetThreadpoolThreadMaximum(tPool,10);
		//初始化线程池环境变量
		InitializeThreadpoolEnvironment(&pcbe);
		//为线程池设置线程池环境变量
		SetThreadpoolCallbackPool(&pcbe,tPool);
		while (!g_bEndListen)  //循环等待客户的请求
		{
			//第六步：接受客户端的连接请求，返回与该客户建立的连接套接字
			iLen=sizeof(cli);
			sAccept=accept(g_sListener,(struct sockaddr*)&cli,&iLen);
			if (sAccept==INVALID_SOCKET)
			{
				sprintf(strMsg,"http service accept() Failed:%d",WSAGetLastError());
				moon_write_error_log(strMsg);
				break;
			}
			//第七步，创建任务接受浏览器请求
			//DWORD ThreadID;
			//CreateThread(NULL,0,SimpleHTTPServer,(LPVOID)sAccept,0,&ThreadID);
			//单次工作提交
			context.m_socket = sAccept;
			//TrySubmitThreadpoolCallback(moon_win32_http_request,&context,&pcbe);
			SubmitThreadpoolWork(CreateThreadpoolWork(moon_win32_http_request, &context,&pcbe));
		}
		//清理线程池的环境变量
		DestroyThreadpoolEnvironment(&pcbe);
		//关闭线程池
		CloseThreadpool(tPool);
		return 0;
	}
#endif

	/************************************************************************/
	/* 启动http服务                                                         */
	/************************************************************************/
	bool lauch_http_service()
	{
		//创建线程池
#ifdef MS_WINDOWS
		//创建监听线程
		g_bEndListen = false;
		g_hListener = CreateThread(NULL,0,moon_win32_http_listen,NULL,0,&g_dListenerId);
		if (g_hListener == NULL)
		{
			moon_write_error_log("create http_listenning_server thread failed");
			return false;
		}
		//关闭监听线程句柄
		CloseHandle(g_hListener);
		//pThreads = moon_create_thread_pool(10,moon_win32_http_request,NULL);
#endif
		moon_write_info_log("moon http service started");
	}

	/************************************************************************/
	/* 停止http服务                                                         */
	/************************************************************************/
	bool end_http_service()
	{
		moon_write_info_log("close http server");
		g_bEndListen = true;
		//等待监听线程退出
#ifdef MS_WINDOWS
		WaitForSingleObject(g_hListener,1000);
		closesocket(g_sListener);
		WSACleanup();
#endif
	}

#ifdef __cplusplus
}
#endif