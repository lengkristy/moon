#ifndef _MAP_H
#define _MAP_H

/**
 * 
 * 文件说明：
 *     纯C实现hashmap，由于限制，map中的key采用char*，如果key为其他类型的请先转换成char*
 * 文件创建时间：
 *     2020-09-16 13:58
 * hashmap实现原理描述：
 *     hashmap实现采用数组加链表，首先将key生成一个int类型的hashcode值，然后根据hashcode值进行数组的长度取余数，
 *   以余数作为存储值的索引，如果产生了hash碰撞，那么将在该索引数组上的链表进行值存储
 */

#include "array_list.h"

#ifdef _MSC_VER/* only support win32 and greater. */
#define MS_WINDOWS
#endif


#ifdef __cplusplus
extern "C" {
#endif

//定义bool类型
#ifndef bool
#define bool int
#endif
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

/*define NULL */
#ifndef NULL
#define NULL 0
#endif

	//定义key-value键值对
	typedef struct _map_item{
		char *p_key;//存放key
		void *p_val;//存放val
	}map_item;
	

	//定义map节点
	typedef struct _map_node{
		map_item *p_cur_item;//当前节点的项
		struct _map_node *p_next_item;//下一个节点项
	}map_node;

	//定义map
	typedef struct _map{
		map_node *p_nodes;//节点列表
		unsigned int node_size;//表示数组的大小，并不是元素的个数
		unsigned int length;//元素的个数
	}map;

	/**
	 * 函数说明：
	 *    map初始化，初始化500个数组进行存放，如果500个数组全部使用完成，那么看链表的长度是否达到阈值进行动态扩容
	 * 返回值：
	 *    返回map指针
	 */
	map* map_init();

	/**
	 * 函数说明：
	 *    向map中放一个key、val项
	 * 参数：
	 *    p_map：map指针
	 *    p_key：key，不能为空，并且不能重复，如果重复那么将会覆盖
	 *    p_val：值
	 */
	void map_put(map* p_map,char *p_key,void *p_val);

	/**
	 * 函数说明：
	 *   获取元素
	 * 参数：
	 *   p_map：map指针
	 *   p_key：key
	 * 返回值：
	 *   返回数据指针
	 */
	void* map_get(map* p_map,char *p_key);

	/**
	 * 函数说明：
	 *   获取所有的key列表
	 * 参数：
	 *   p_map：map指针
	 * 返回值：
	 *   返回map中的所有key列表
	 */
	Array_List* map_keys(map* p_map);

	/**
	 * 函数说明：
	 *   map释放，调用该函数之前，请将存储的数据项的数据手动释放
	 * 参数：
	 *   p_map：map指针
	 */
	void map_free(map* p_map);


	/**
	 * map测试
	 */
	void map_test();
	
#ifdef __cplusplus
}
#endif

#endif