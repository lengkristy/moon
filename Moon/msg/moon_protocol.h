#ifndef _MOON_PROTOCOL_H
#define _MOON_PROTOCOL_H
/************************************************************************/
/* 该文件定义通信的协议                                                 */
/************************************************************************/

/**************************定义系统协议**********************************/


//////////////////////定义网络上流转的消息报文///////////////////////////////////
/// 消息报文格式定义：
/// 消息的前8个字节为消息的头部定义：\n\rMNPH\n\r
/// 消息的第8-11字节为消息报文的大小，该消息报文的大小并不包括消息头部定义大小
/// 报文头部后面的数据体才是Message结构体
#define PKG_HEAD_LENGTH 12 //定义包头的长度，12个字节，前八字节为标识，后4字节为数据体长度

#define PKG_TAIL_LENGTH 8 //定义包尾的长度，8个字节

#define DATA_BYTE_MAX_LENGTH  7000 //数据实体字节最大长度

#define PKG_BYTE_MAX_LENGTH  8192 //包字节单次发送的最大长度

#define PKG_HEAD_FLAG "\n\rMNPH\n\r" //包头前8字节定义标识

#define PKG_TAIL_FLAG "\n\rMNPT\n\r" //包尾后8个字节定义标识


///////////////////////////收到客户端消息回复协议//////////////////////////////////////////////////////////
#define MN_PROTOCOL_MAIN_REPLY  10003  //服务端回复主协议代码
#define MN_PROTOCOL_SUB_REPLY_OK 1000301 //服务端收到消息处理成功的子协议代码
#define MN_PROTOCOL_SUB_REPLY_FAILED 1000302 //服务端收到消息处理失败的子协议代码
#define MN_PROTOCOL_SUB_CLIENT_REPLY_OK 1000303 //客户端收到客户端消息并处理成功的子协议代码，用于点对点消息发送
#define MN_PROTOCOL_SUB_CLIENT_REPLY_FAILED 1000304 //客户端收到客户端消息并处理失败的子协议代码，用于点对点消息发送


///////////////////////////Connection initialization protocol////////////////////////////////
#define MN_PROTOCOL_MAIN_CONNECT_INIT 10001 //客户端连接初始化主协议号，客户端连接服务器成功后的第一条消息，将会把客户端的运行环境信息传递过来
#define MN_PROTOCOL_SUB_SERVER_CON 1000101 //server connection sub-Protocol(used to cluster)
#define MN_PROTOCOL_SUB_CLIENT_CON 1000102 //client connection sub-Protocol
#define MN_PROTOCOL_SUB_SERVER_ACCEPT 1000109 //服务端同意接受连接的子协议
////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////定义发送点对点的消息/////////////////////////////////////////////////////////
#define MN_PROTOCOL_MAIN_MSG_POINT_TO_POINT 10002 //点对点的主消息
#define MN_PROTOCOL_SUB_MSG_PTP_TEXT     1000201 //点对点的文本消息
#define MN_PROTOCOL_SUB_MSG_PTP_EMOTICON 1000202 //点对点的表情消息
#define MN_PROTOCOL_SUB_MSG_PTP_IMG      1000203 //点对点的图片消息
#define MN_PROTOCOL_SUB_MSG_PTP_VIDEO	  1000204 //点对点的短视频消息
#define MN_PROTOCOL_SUB_MSG_PTP_FILE	  1000205 //点对点的文件传输消息

/////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////获取服务器客户端信息协议/////////////////////////////////////////////////////
#define MN_PROTOCOL_MAIN_SCI 10004 //获取服务器客户端信息主协议代码
#define MN_PROTOCOL_SUB_ALL_CLIENT_LIST 1000401 //请求获取服务器中所有客户端列表
#define MN_PROTOCAL_SUB_ALL_CLIENBT_LIST_OK 1000402 //服务器成功返回客户端列表
////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////定义发送广播群聊消息////////////////////////////////////////////////////////
#define MN_PROTOCOL_MAIN_BROADCAST 10005 //广播消息的主协议代码
#define MN_PROTOCOL_SUB_SYS_BROADCAST 1000501 //系统广播消息
#define MN_PROTOCOL_SUB_USER_TEXT_BROADCAST 1000510 //发送用户群组消息
#define MN_PROTOCOL_SUB_USER_EMOTICON_BROADCAST 1000511 //发送用户群组表情消息
#define MN_PROTOCOL_SUB_USER_IMG_BROADCAST 1000512 //发送用户群组图片消息
#define MN_PROTOCOL_SUB_USER_VIDE_BROADCAST 1000513 //发送用户群组短视频消息
#define MN_PROTOCOL_SUB_USER_FILE_BROADCAT 1000514 //发送用户群组文件传输消息


#endif