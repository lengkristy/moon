#ifndef _MOON_PROTOCOL_H
#define _MOON_PROTOCOL_H
/************************************************************************/
/* ���ļ�����ͨ�ŵ�Э��                                                 */
/************************************************************************/

/**************************����ϵͳЭ��**********************************/


//////////////////////������������ת����Ϣ����///////////////////////////////////
/// ��Ϣ���ĸ�ʽ���壺
/// ��Ϣ��ǰ4���ֽ�Ϊ��Ϣ��ͷ�����壺MNPH
/// ��Ϣ�ĵ�5-8�ֽ�Ϊ��Ϣ���ĵĴ�С������Ϣ���ĵĴ�С����������Ϣͷ�������С
/// ����ͷ����������������Message�ṹ��
#define PKG_HEAD_LENGTH 8 //�����ͷ�ĳ��ȣ�8���ֽڣ�ǰ���ֽ�Ϊ��ʶ����4�ֽ�Ϊ�����峤��

#define DATA_BYTE_MAX_LENGTH  7000 //����ʵ���ֽ���󳤶�

#define PKG_BYTE_MAX_LENGTH  8192 //���ֽ���󳤶�

#define PKG_HEAD_FLAG "MNPH" //��ͷǰ4�ֽڶ����ʶ


///////////////////////////�յ��ͻ�����Ϣ�ظ�Э��//////////////////////////////////////////////////////////
#define SYS_MAIN_PROTOCOL_REPLY  10003  //����˻ظ���Э�����
#define SYS_SUB_PROTOCOL_REPLY_OK 1000301 //������յ���Ϣ�ɹ�����Э�����
#define SYS_SUB_PROTOCOL_REPLY_FAILD 1000302 //������յ���Ϣʧ�ܵ���Э�����

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