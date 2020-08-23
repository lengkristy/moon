/************************************************************************
 * ���ļ���Ҫ�����ڹ������пͻ��˵�socket������. �Կͻ�����������ɾ�Ĳ�
 * coding by: haoran dai
 * time:2019-12-13 16:52                               
 ***********************************************************************/

#ifndef _MOON_SESSION_MANAGER_H
#define _MOON_SESSION_MANAGER_H

#include "moon_session.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ����˵����
 *	��ʼ��session������
 */
void init_session_manager();

/**
 * ����˵����
 *	��ӿͻ���session
 * ������
 *	p_moon_session���ͻ���sessionָ��
 */
void add_session(moon_session* p_moon_session);


/**
 * ����˵����
 *	ɾ���ͻ���session
 * ������
 *	p_moon_session���ͻ���sessionָ��
 */
void remove_session(moon_session* p_moon_session);

/**
 * ����˵����
 *   ͨ���ͻ���id����socket context
 * ������
 *   client_id���ͻ���id
 * ����ֵ��
 *   ���ؿͻ���sessionָ�룬���ʧ�ܷ���NULL
 */
moon_session* get_moon_session_by_client_id(moon_char* client_id);

/**
 * ����˵����
 *   ��ȡ��ǰ���пͻ�������
 * ����ֵ��
 *   ��������
 */
int get_socket_context_count();

/**
 * ����˵����
 *   ͨ��������ȡsocket context
 * ������
 *   index������
 * ����ֵ��
 *   ���ؿͻ���sessionָ�룬���ʧ�ܷ���NULL
 */
moon_session* get_moon_session_by_index(int index);

#ifdef __cplusplus
}
#endif
#endif