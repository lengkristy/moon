#ifndef _MOON_PROTOCOL_H
#define _MOON_PROTOCOL_H
/************************************************************************/
/* ���ļ�����ͨ�ŵ�Э��                                                 */
/************************************************************************/

/**************************����ϵͳЭ��**********************************/

///////////////////////////Connection initialization protocol////////////////////////////////
#define  SYS_MAIN_PROTOCOL_CONNECT_INIT 10001 //�ͻ������ӳ�ʼ����Э��ţ��ͻ������ӷ������ɹ���ĵ�һ����Ϣ������ѿͻ��˵����л�����Ϣ���ݹ���

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