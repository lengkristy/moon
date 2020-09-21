#ifndef _MOON_PROTOCOL_H
#define _MOON_PROTOCOL_H
/************************************************************************/
/* ���ļ�����ͨ�ŵ�Э��                                                 */
/************************************************************************/

/**************************����ϵͳЭ��**********************************/


//////////////////////������������ת����Ϣ����///////////////////////////////////
/// ��Ϣ���ĸ�ʽ���壺
/// ��Ϣ��ǰ8���ֽ�Ϊ��Ϣ��ͷ�����壺\n\rMNPH\n\r
/// ��Ϣ�ĵ�8-11�ֽ�Ϊ��Ϣ���ĵĴ�С������Ϣ���ĵĴ�С����������Ϣͷ�������С
/// ����ͷ����������������Message�ṹ��
#define PKG_HEAD_LENGTH 12 //�����ͷ�ĳ��ȣ�12���ֽڣ�ǰ���ֽ�Ϊ��ʶ����4�ֽ�Ϊ�����峤��

#define PKG_TAIL_LENGTH 8 //�����β�ĳ��ȣ�8���ֽ�

#define DATA_BYTE_MAX_LENGTH  7000 //����ʵ���ֽ���󳤶�

#define PKG_BYTE_MAX_LENGTH  8192 //���ֽڵ��η��͵���󳤶�

#define PKG_HEAD_FLAG "\n\rMNPH\n\r" //��ͷǰ8�ֽڶ����ʶ

#define PKG_TAIL_FLAG "\n\rMNPT\n\r" //��β��8���ֽڶ����ʶ


///////////////////////////�յ��ͻ�����Ϣ�ظ�Э��//////////////////////////////////////////////////////////
#define MN_PROTOCOL_MAIN_REPLY  10003  //����˻ظ���Э�����
#define MN_PROTOCOL_SUB_REPLY_OK 1000301 //������յ���Ϣ����ɹ�����Э�����
#define MN_PROTOCOL_SUB_REPLY_FAILED 1000302 //������յ���Ϣ����ʧ�ܵ���Э�����
#define MN_PROTOCOL_SUB_CLIENT_REPLY_OK 1000303 //�ͻ����յ��ͻ�����Ϣ������ɹ�����Э����룬���ڵ�Ե���Ϣ����
#define MN_PROTOCOL_SUB_CLIENT_REPLY_FAILED 1000304 //�ͻ����յ��ͻ�����Ϣ������ʧ�ܵ���Э����룬���ڵ�Ե���Ϣ����


///////////////////////////Connection initialization protocol////////////////////////////////
#define MN_PROTOCOL_MAIN_CONNECT_INIT 10001 //�ͻ������ӳ�ʼ����Э��ţ��ͻ������ӷ������ɹ���ĵ�һ����Ϣ������ѿͻ��˵����л�����Ϣ���ݹ���
#define MN_PROTOCOL_SUB_SERVER_CON 1000101 //server connection sub-Protocol(used to cluster)
#define MN_PROTOCOL_SUB_CLIENT_CON 1000102 //client connection sub-Protocol
#define MN_PROTOCOL_SUB_SERVER_ACCEPT 1000109 //�����ͬ��������ӵ���Э��
////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////���巢�͵�Ե����Ϣ/////////////////////////////////////////////////////////
#define MN_PROTOCOL_MAIN_MSG_POINT_TO_POINT 10002 //��Ե������Ϣ
#define MN_PROTOCOL_SUB_MSG_PTP_TEXT     1000201 //��Ե���ı���Ϣ
#define MN_PROTOCOL_SUB_MSG_PTP_EMOTICON 1000202 //��Ե�ı�����Ϣ
#define MN_PROTOCOL_SUB_MSG_PTP_IMG      1000203 //��Ե��ͼƬ��Ϣ
#define MN_PROTOCOL_SUB_MSG_PTP_VIDEO	  1000204 //��Ե�Ķ���Ƶ��Ϣ
#define MN_PROTOCOL_SUB_MSG_PTP_FILE	  1000205 //��Ե���ļ�������Ϣ

/////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////��ȡ�������ͻ�����ϢЭ��/////////////////////////////////////////////////////
#define MN_PROTOCOL_MAIN_SCI 10004 //��ȡ�������ͻ�����Ϣ��Э�����
#define MN_PROTOCOL_SUB_ALL_CLIENT_LIST 1000401 //�����ȡ�����������пͻ����б�
#define MN_PROTOCAL_SUB_ALL_CLIENBT_LIST_OK 1000402 //�������ɹ����ؿͻ����б�
////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////���巢�͹㲥Ⱥ����Ϣ////////////////////////////////////////////////////////
#define MN_PROTOCOL_MAIN_BROADCAST 10005 //�㲥��Ϣ����Э�����
#define MN_PROTOCOL_SUB_SYS_BROADCAST 1000501 //ϵͳ�㲥��Ϣ
#define MN_PROTOCOL_SUB_USER_TEXT_BROADCAST 1000510 //�����û�Ⱥ����Ϣ
#define MN_PROTOCOL_SUB_USER_EMOTICON_BROADCAST 1000511 //�����û�Ⱥ�������Ϣ
#define MN_PROTOCOL_SUB_USER_IMG_BROADCAST 1000512 //�����û�Ⱥ��ͼƬ��Ϣ
#define MN_PROTOCOL_SUB_USER_VIDE_BROADCAST 1000513 //�����û�Ⱥ�����Ƶ��Ϣ
#define MN_PROTOCOL_SUB_USER_FILE_BROADCAT 1000514 //�����û�Ⱥ���ļ�������Ϣ


#endif