#ifndef _MOON_PROTOCOL_H
#define _MOON_PROTOCOL_H
/************************************************************************/
/* 该文件定义通信的协议                                                 */
/************************************************************************/

/**************************定义系统协议**********************************/


//////////////////////定义网络上流转的消息报文///////////////////////////////////
/// 消息报文格式定义：
/// 消息的前4个字节为消息的头部定义：MNPH
/// 消息的第5-8字节为消息报文的大小，该消息报文的大小并不包括消息头部定义大小
/// 报文头部后面的数据体才是Message结构体
#define PKG_HEAD_LENGTH 8 //定义包头的长度，8个字节，前四字节为标识，后4字节为数据体长度

#define DATA_BYTE_MAX_LENGTH  7000 //数据实体字节最大长度

#define PKG_BYTE_MAX_LENGTH  8192 //包字节最大长度

#define PKG_HEAD_FLAG "MNPH" //包头前4字节定义标识


///////////////////////////收到客户端消息回复协议//////////////////////////////////////////////////////////
#define SYS_MAIN_PROTOCOL_REPLY  10003  //服务端回复主协议代码
#define SYS_SUB_PROTOCOL_REPLY_OK 1000301 //服务端收到消息成功的子协议代码
#define SYS_SUB_PROTOCOL_REPLY_FAILD 1000302 //服务端收到消息失败的子协议代码

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