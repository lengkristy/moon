/************************************************************************
 * 
 * file desc:
 *      this file contain moon memery manage.
 * coding by: haoran dai
 * time:2018-5-1 21:56                               
 ***********************************************************************/
#include <stdlib.h>
#include <string.h>

#pragma once
#ifndef _MOON_MALLOC_H
#define _MOON_MALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

void* moon_malloc(unsigned int num_bytes);/*allocate memory*/

void moon_free(void* memory);/*free point*/

void* moon_realloc(void* memory,unsigned int num_bytes);//��memory���������·����ڴ�

void moon_memory_zero(void* memory,unsigned int num_bytes);/*memory zero*/




#define mem_size_t unsigned long long
#define KB (mem_size_t)(1 << 10)
#define MB (mem_size_t)(1 << 20)
#define GB (mem_size_t)(1 << 30)

typedef struct _mp_chunk {
    mem_size_t alloc_mem;
    struct _mp_chunk *prev, *next;
    int is_free;
} _MP_Chunk;

typedef struct _mp_mem_pool_list {
    char* start;
    unsigned int id;
    mem_size_t mem_pool_size;
    mem_size_t alloc_mem;
    mem_size_t alloc_prog_mem;
    _MP_Chunk *free_list, *alloc_list;
    struct _mp_mem_pool_list* next;
} _MP_Memory;

typedef struct _mp_mem_pool {
    unsigned int last_id;
    int auto_extend;
    mem_size_t mem_pool_size, total_mem_pool_size, max_mem_pool_size;
    struct _mp_mem_pool_list* mlist;
#ifdef _Z_MEMORYPOOL_THREAD_
    pthread_mutex_t lock;
#endif
} MemoryPool;

/*
 *  �ڲ����ߺ���(������)
 */

// ����Memory������
void get_memory_list_count(MemoryPool* mp, mem_size_t* mlist_len);
// ÿ��Memory��ͳ����Ϣ
void get_memory_info(MemoryPool* mp,
                     _MP_Memory* mm,
                     mem_size_t* free_list_len,
                     mem_size_t* alloc_list_len);
int get_memory_id(_MP_Memory* mm);

/*
 *  �ڴ��API
 */

MemoryPool* MemoryPoolInit(mem_size_t maxmempoolsize, mem_size_t mempoolsize);
void* MemoryPoolAlloc(MemoryPool* mp, mem_size_t wantsize);
int MemoryPoolFree(MemoryPool* mp, void* p);
MemoryPool* MemoryPoolClear(MemoryPool* mp);
int MemoryPoolDestroy(MemoryPool* mp);
int MemoryPoolSetThreadSafe(MemoryPool* mp, int thread_safe);

/*
 *  �ڴ����ϢAPI
 */

// �ܿռ�
mem_size_t GetTotalMemory(MemoryPool* mp);
// ʵ�ʷ���ռ�
mem_size_t GetUsedMemory(MemoryPool* mp);
float MemoryPoolGetUsage(MemoryPool* mp);
// ����ռ�ÿռ�
mem_size_t GetProgMemory(MemoryPool* mp);
float MemoryPoolGetProgUsage(MemoryPool* mp);


#ifdef __cplusplus
}
#endif

#endif