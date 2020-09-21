#ifndef _MAP_H
#define _MAP_H

/**
 * 
 * �ļ�˵����
 *     ��Cʵ��hashmap���������ƣ�map�е�key����char*�����keyΪ�������͵�����ת����char*
 * �ļ�����ʱ�䣺
 *     2020-09-16 13:58
 * hashmapʵ��ԭ��������
 *     hashmapʵ�ֲ���������������Ƚ�key����һ��int���͵�hashcodeֵ��Ȼ�����hashcodeֵ��������ĳ���ȡ������
 *   ��������Ϊ�洢ֵ�����������������hash��ײ����ô���ڸ����������ϵ��������ֵ�洢
 */

#include "array_list.h"

#ifdef _MSC_VER/* only support win32 and greater. */
#define MS_WINDOWS
#endif


#ifdef __cplusplus
extern "C" {
#endif

//����bool����
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

	//����key-value��ֵ��
	typedef struct _map_item{
		char *p_key;//���key
		void *p_val;//���val
	}map_item;
	

	//����map�ڵ�
	typedef struct _map_node{
		map_item *p_cur_item;//��ǰ�ڵ����
		struct _map_node *p_next_item;//��һ���ڵ���
	}map_node;

	//����map
	typedef struct _map{
		map_node *p_nodes;//�ڵ��б�
		unsigned int node_size;//��ʾ����Ĵ�С��������Ԫ�صĸ���
		unsigned int length;//Ԫ�صĸ���
	}map;

	/**
	 * ����˵����
	 *    map��ʼ������ʼ��500��������д�ţ����500������ȫ��ʹ����ɣ���ô������ĳ����Ƿ�ﵽ��ֵ���ж�̬����
	 * ����ֵ��
	 *    ����mapָ��
	 */
	map* map_init();

	/**
	 * ����˵����
	 *    ��map�з�һ��key��val��
	 * ������
	 *    p_map��mapָ��
	 *    p_key��key������Ϊ�գ����Ҳ����ظ�������ظ���ô���Ḳ��
	 *    p_val��ֵ
	 */
	void map_put(map* p_map,char *p_key,void *p_val);

	/**
	 * ����˵����
	 *   ��ȡԪ��
	 * ������
	 *   p_map��mapָ��
	 *   p_key��key
	 * ����ֵ��
	 *   ��������ָ��
	 */
	void* map_get(map* p_map,char *p_key);

	/**
	 * ����˵����
	 *   ��ȡ���е�key�б�
	 * ������
	 *   p_map��mapָ��
	 * ����ֵ��
	 *   ����map�е�����key�б�
	 */
	Array_List* map_keys(map* p_map);

	/**
	 * ����˵����
	 *   map�ͷţ����øú���֮ǰ���뽫�洢��������������ֶ��ͷ�
	 * ������
	 *   p_map��mapָ��
	 */
	void map_free(map* p_map);


	/**
	 * map����
	 */
	void map_test();
	
#ifdef __cplusplus
}
#endif

#endif