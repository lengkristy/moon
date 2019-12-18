/************************************************************************
 * 该文件主要是用于管理所有客户端的socket上下文. 对客户端上下文增删改查
 * coding by: haoran dai
 * time:2019-12-13 16:52                               
 ***********************************************************************/
#ifdef MS_WINDOWS
#include "ms_socket_context.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 函数说明：
 *	初始化socket上下文管理器
 */
void init_socket_context_manager();

/**
 * 函数说明：
 *	添加客户端socket上下文
 * 参数：
 *	socket上下文的结构体指针，如果是windows的话，那么就是：MS_SOCKET_CONTEXT *
 */
void add_socket_context(void* psocket_context);


/**
 * 函数说明：
 *	删除客户端socket上下文
 * 参数：
 *	socket上下文的结构体指针，如果是windows的话，那么就是：MS_SOCKET_CONTEXT *
 */
void remove_socket_context(void* psocket_context);

#ifdef MS_WINDOWS
/**
 * 函数说明：
 *   通过客户端id查找socket context
 * 参数：
 *   client_id：客户端id
 * 返回值：
 *   如果查询到则返回socket context指针，没有查询到返回NULL
 */
MS_SOCKET_CONTEXT * get_socket_context_by_client_id(moon_char* client_id);
#endif

/**
 * 函数说明：
 *   获取当前所有客户端数量
 * 返回值：
 *   返回数量
 */
int get_socket_context_count();

#ifdef MS_WINDOWS
/**
 * 函数说明：
 *   通过索引获取socket context
 * 参数：
 *   index：索引
 * 返回值：
 *   如果查询到则返回socket context指针，没有查询到返回NULL
 */
MS_SOCKET_CONTEXT * get_socket_context_by_index(int index);
#endif

#ifdef __cplusplus
}
#endif