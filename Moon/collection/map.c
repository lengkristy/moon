#include "map.h"
#include <malloc.h>
#include <memory.h>
#include <time.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * �������ܣ�
	 *   ����hashcode
	 * ������
	 *   key������hashcode��Դ�ַ���
	 * ����ֵ��
	 *   ����hashcode
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
	 * ����˵����
	 *    map��ʼ������ʼ��500��������д�ţ����500������ȫ��ʹ����ɣ���ô������ĳ����Ƿ�ﵽ��ֵ���ж�̬����
	 * ����ֵ��
	 *    ����mapָ��
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
	 * ����˵����
	 *    ��map�з�һ��key��val��
	 * ������
	 *    p_map:mapָ��
	 *    p_key��key������Ϊ�գ����Ҳ����ظ�������ظ���ô���Ḳ��
	 *    p_val��ֵ
	 */
	void map_put(map *p_map,char *p_key,void *p_val)
	{
		//ȡָ��ָ��ĵ�һ���ֽڵ����ݵĶ����Ƶ�ֵ����hashcode��Ȼ�����ȡ������
		map_item *p_map_item = NULL;
		map_node* p_map_node = NULL;
		unsigned int index = 0;
		if (p_map == NULL || p_key == NULL)
		{
			return;
		}
		index = map_hash_code(p_key);
		//����ȡ��
		index = index % p_map->node_size;
		//������Ϊmap������������жϸ����������Ƿ���ڣ����������ô�浽��������������
		if (p_map->p_nodes[index].p_cur_item == NULL)
		{
			//ֱ�����
			p_map_item = (map_item*)malloc(sizeof(map_item));
			p_map_item->p_key = p_key;
			p_map_item->p_val = p_val;
			p_map->p_nodes[index].p_cur_item = p_map_item;
			p_map->length++;
			return;
		}
		//�����ǰ������ֵ����ô��������׷��
		p_map_node = &(p_map->p_nodes[index]);
		while(p_map_node->p_next_item != NULL)
		{
			//�ж�key�Ƿ��ظ�������ظ���ôֱ�Ӹ���val
			if (strcmp(p_map_node->p_cur_item->p_key,p_key) == 0)
			{
				p_map_node->p_cur_item->p_val = p_val;
				return;
			}
			p_map_node = p_map_node->p_next_item;
		}
		//�����ڵ�
		p_map_node->p_next_item = (map_node*)malloc(sizeof(map_node));
		memset(p_map_node->p_next_item,0,sizeof(map_node));
		p_map_item = (map_item*)malloc(sizeof(map_item));
		p_map_item->p_key = p_key;
		p_map_item->p_val = p_val;
		p_map_node->p_cur_item = p_map_item;
		p_map->length++;
	}

	/**
	 * ����˵����
	 *   ��ȡԪ��
	 * ������
	 *   p_map��mapָ��
	 *   p_key��key
	 * ����ֵ��
	 *   ��������ָ��
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
		//��������
		index = index % p_map->node_size;
		p_map_node = &(p_map->p_nodes[index]);
		while(p_map_node != NULL && p_map_node->p_cur_item != NULL){
			if (strcmp(p_map_node->p_cur_item->p_key,p_key) == 0)
			{
				return p_map_node->p_cur_item->p_val;
			}
			//��������
			p_map_node = p_map_node->p_next_item;
		}
	}

	/**
	 * ����˵����
	 *   ��ȡ���е�key�б�
	 * ������
	 *   p_map��mapָ��
	 * ����ֵ��
	 *   ����map�е�����key�б�
	 */
	Array_List* map_keys(map* p_map)
	{
		int index = 0;
		void *p_key = NULL;
		Array_List *p_array_keys = NULL;
		map_node *p_map_node = NULL;
		p_array_keys = array_list_init();
		//��ʼ����
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
	 * ����˵����
	 *   map�ͷ�
	 */
	void map_free(map* p_map)
	{
		int index = 0;
		map_node *p_map_node = NULL;
		map_node *p_temp_map_node = NULL;
		if (p_map != NULL)
		{
			//�ͷ�����ڵ�
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
			//�ͷ�����ڵ�
			free(p_map->p_nodes);
			free(p_map);
		}
	}

	/**
	 * map����
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

		//�洢100���Ԫ��
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
		printf("\n����100���Ԫ������ʱ�䣺%0.3f���� \n", total_time);
		//����ĳ��Ԫ�أ����Ҽ���ʱ��
		start = clock();
		tmp_val = (char*)map_get(p_map,find_key);
		finish = clock();
		total_time = (double)(finish - start);
		printf("\n����Ԫ��%s��Ԫ��ֵ��%s������ʱ�䣺%0.3f���� \n",find_key, tmp_val,total_time);
		//����key
		start = clock();
		p_list = map_keys(p_map);
		finish = clock();
		total_time = (double)(finish - start);
		printf("\n����key����ʱ�䣺%0.3f���� \n", total_time);

		//�ͷ���Դ
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