/************************************************************************
 * 该文件主要是用于管理所有客户端的socket上下文. 对客户端上下文增删改查
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
 * 函数说明：
 *	初始化session管理器
 */
void init_session_manager();

/**
 * 函数说明：
 *	添加客户端session
 * 参数：
 *	p_moon_session：客户端session指针
 */
void add_session(moon_session* p_moon_session);


/**
 * 函数说明：
 *	删除客户端session
 * 参数：
 *	p_moon_session：客户端session指针
 */
void remove_session(moon_session* p_moon_session);

/**
 * 函数说明：
 *   通过客户端id查找socket context
 * 参数：
 *   client_id：客户端id
 * 返回值：
 *   返回客户端session指针，如果失败返回NULL
 */
moon_session* get_moon_session_by_client_id(moon_char* client_id);

/**
 * 函数说明：
 *   获取当前所有客户端数量
 * 返回值：
 *   返回数量
 */
int get_socket_context_count();

/**
 * 函数说明：
 *   通过索引获取socket context
 * 参数：
 *   index：索引
 * 返回值：
 *   返回客户端session指针，如果失败返回NULL
 */
moon_session* get_moon_session_by_index(int index);

#ifdef __cplusplus
}
#endif
#endif