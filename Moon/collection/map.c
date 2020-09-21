#include "map.h"
#include <malloc.h>
#include <memory.h>
#include <time.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * 函数功能：
	 *   计算hashcode
	 * 参数：
	 *   key：生成hashcode的源字符串
	 * 返回值：
	 *   返回hashcode
	 */
	static unsigned int map_hash_code(char* key)
	{
		unsigned int h = 0;
		int index = 0;
		if (key == NULL)
		{
			return 0;
		}
		for (index = 0;index < strlen(key);index++)
		{
			h = 31 * h + key[index];
		}
		return h;
	}

	/**
	 * 函数说明：
	 *    map初始化，初始化500个数组进行存放，如果500个数组全部使用完成，那么看链表的长度是否达到阈值进行动态扩容
	 * 返回值：
	 *    返回map指针
	 */
	map* map_init()
	{
		map *p_map;
		unsigned int ini_size = 500;
		p_map = (map*)malloc(sizeof(map));
		if(p_map == NULL) return NULL;
		p_map->node_size = ini_size;
		p_map->p_nodes = (map_node*)malloc(sizeof(map_node) * ini_size);
		p_map->length = 0;
		memset(p_map->p_nodes,0,(sizeof(map_node) * ini_size));
		return p_map;
	}

	/**
	 * 函数说明：
	 *    向map中放一个key、val项
	 * 参数：
	 *    p_map:map指针
	 *    p_key：key，不能为空，并且不能重复，如果重复那么将会覆盖
	 *    p_val：值
	 */
	void map_put(map *p_map,char *p_key,void *p_val)
	{
		//取指针指向的第一个字节的数据的二进制的值计算hashcode，然后进行取余运算
		map_item *p_map_item = NULL;
		map_node* p_map_node = NULL;
		unsigned int index = 0;
		if (p_map == NULL || p_key == NULL)
		{
			return;
		}
		index = map_hash_code(p_key);
		//进行取余
		index = index % p_map->node_size;
		//余数作为map数组的索引，判断该索引上面是否存在，如果存在那么存到该索引的链表上
		if (p_map->p_nodes[index].p_cur_item == NULL)
		{
			//直接添加
			p_map_item = (map_item*)malloc(sizeof(map_item));
			p_map_item->p_key = p_key;
			p_map_item->p_val = p_val;
			p_map->p_nodes[index].p_cur_item = p_map_item;
			p_map->length++;
			return;
		}
		//如果当前索引有值，那么在链表上追加
		p_map_node = &(p_map->p_nodes[index]);
		while(p_map_node->p_next_item != NULL)
		{
			//判断key是否重复，如果重复那么直接覆盖val
			if (strcmp(p_map_node->p_cur_item->p_key,p_key) == 0)
			{
				p_map_node->p_cur_item->p_val = p_val;
				return;
			}
			p_map_node = p_map_node->p_next_item;
		}
		//创建节点
		p_map_node->p_next_item = (map_node*)malloc(sizeof(map_node));
		memset(p_map_node->p_next_item,0,sizeof(map_node));
		p_map_item = (map_item*)malloc(sizeof(map_item));
		p_map_item->p_key = p_key;
		p_map_item->p_val = p_val;
		p_map_node->p_cur_item = p_map_item;
		p_map->length++;
	}

	/**
	 * 函数说明：
	 *   获取元素
	 * 参数：
	 *   p_map：map指针
	 *   p_key：key
	 * 返回值：
	 *   返回数据指针
	 */
	void* map_get(map* p_map,char *p_key)
	{
		map_item *p_map_item = NULL;
		map_node* p_map_node = NULL;
		int index = 0;
		if (p_map == NULL || p_key == NULL)
		{
			return NULL;
		}
		//index = (int)p_key;
		index = map_hash_code(p_key);
		//计算索引
		index = index % p_map->node_size;
		p_map_node = &(p_map->p_nodes[index]);
		while(p_map_node != NULL && p_map_node->p_cur_item != NULL){
			if (strcmp(p_map_node->p_cur_item->p_key,p_key) == 0)
			{
				return p_map_node->p_cur_item->p_val;
			}
			//继续查找
			p_map_node = p_map_node->p_next_item;
		}
	}

	/**
	 * 函数说明：
	 *   获取所有的key列表
	 * 参数：
	 *   p_map：map指针
	 * 返回值：
	 *   返回map中的所有key列表
	 */
	Array_List* map_keys(map* p_map)
	{
		int index = 0;
		void *p_key = NULL;
		Array_List *p_array_keys = NULL;
		map_node *p_map_node = NULL;
		p_array_keys = array_list_init();
		//开始遍历
		for (index = 0;index < p_map->node_size;index++)
		{
			if (p_map->p_nodes[index].p_cur_item != NULL)
			{
				p_map_node = &(p_map->p_nodes[index]);
				while(p_map_node != NULL)
				{
					p_map_node->p_cur_item->p_key;
					array_list_insert(p_array_keys,p_key,-1);
					if (p_map_node->p_next_item == NULL)
					{
						break;
					}
					p_map_node = p_map_node->p_next_item;
				}
			}
		}
		return p_array_keys;
	}

	/**
	 * 函数说明：
	 *   map释放
	 */
	void map_free(map* p_map)
	{
		int index = 0;
		map_node *p_map_node = NULL;
		map_node *p_temp_map_node = NULL;
		if (p_map != NULL)
		{
			//释放链表节点
			for (index = 0;index < p_map->node_size;index++)
			{
				p_map_node = &(p_map->p_nodes[index]);
				while(p_map_node != NULL && p_map_node->p_next_item != NULL)
				{
					p_temp_map_node = p_map_node->p_next_item;
					p_map_node = p_temp_map_node->p_next_item;
					free(p_temp_map_node->p_cur_item);
					free(p_temp_map_node);
				}
			}
			//释放数组节点
			free(p_map->p_nodes);
			free(p_map);
		}
	}

	/**
	 * map测试
	 */
	void map_test()
	{
		map* p_map = NULL;
		Array_List *p_list = NULL;
		char* find_key = NULL;
		char* key = NULL;
		char* val = NULL;
		char* tmp_val;
		int index = 0;
		clock_t start, finish;
		double total_time;
		p_map = map_init();
		start = clock();

		//存储100万个元素
		for (index = 0; index < 1000000;index++)
		{
			key = (char*)malloc(100);
			val = (char*)malloc(100);
			sprintf(key,"key%d",index);
			sprintf(val,"val%d",index);
			if (index == 234232)
			{
				find_key = key;
			}
			map_put(p_map,key,val);
		}
		finish = clock();
		total_time = (double)(finish - start);
		printf("\n插入100万个元素所需时间：%0.3f毫秒 \n", total_time);
		//查找某个元素，并且计算时间
		start = clock();
		tmp_val = (char*)map_get(p_map,find_key);
		finish = clock();
		total_time = (double)(finish - start);
		printf("\n查找元素%s，元素值：%s，所需时间：%0.3f毫秒 \n",find_key, tmp_val,total_time);
		//遍历key
		start = clock();
		p_list = map_keys(p_map);
		finish = clock();
		total_time = (double)(finish - start);
		printf("\n遍历key所需时间：%0.3f毫秒 \n", total_time);

		//释放资源
		for (index = 0; index < p_list->length;index++)
		{
			key = (char*)array_list_getAt(p_list,index);
			if (key != NULL)
			{
				val = (char*)map_get(p_map,key);
				if(val != NULL){
					free(key);
					free(val);
				}
			}
		}
		map_free(p_map);
	}

#ifdef __cplusplus
}
#endif