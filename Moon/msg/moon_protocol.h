#ifndef _MOON_PROTOCOL_H
#define _MOON_PROTOCOL_H
/************************************************************************/
/* 该文件定义通信的协议                                                 */
/************************************************************************/

/**************************定义系统协议**********************************/

///////////////////////////Connection initialization protocol////////////////////////////////
#define  SYS_MAIN_PROTOCOL_CONNECT_INIT 10001 //客户端连接初始化主协议号，客户端连接服务器成功后的第一条消息，将会把客户端的运行环境信息传递过来

#define SYS_SUB_PROTOCOL_SERVER_CON 1000101 //server connection sub-Protocol(used to cluster)

#define SYS_SUB_PROTOCOL_CLIENT_CON 1000102 //client connection sub-Protocol
////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////client login protocol////////////////////////////////////////////
#define SYS_MAIN_PROTOCOL_LOGIN 10002 //client login main-protocol

#define SYS_SUB_PROTOCOL_LOGIN_FIRST 1000201//client first login sub-protocol

#define SYS_SUB_PROTOCOL_LOGIN_SUCCESS 1000202//client login successful sub-protocol

#define SYS_SUB_PROTOCOL_LOGIN_FAILD 1000203//client login failed sub-protocol

#define SYS_SUB_PROTOCOL_LOGIN_OUT 1000204//client login out sub-protocol

#endif