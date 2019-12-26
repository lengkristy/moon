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

#ifdef _MSC_VER/* only support win32 and greater. */
#define MS_WINDOWS
#endif

#ifdef __cplusplus
extern "C" {
#endif

void* moon_malloc(unsigned int num_bytes);/*allocate memory*/

void moon_free(void* memory);/*free point*/

void* moon_realloc(void* memory,unsigned int num_bytes);//在memory基础上重新分配内存

void moon_memory_zero(void* memory,unsigned int num_bytes);/*memory zero*/




#define mem_size_t unsigned long long
#define KB (mem_size_t)(1 << 10)
#define MB (mem_size_t)(1 << 20)
#define GB (mem_size_t)(1 << 30)

/**
 * 内存块
 */
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

/**
 * 内存池结构体
 */
typedef struct _mp_mem_pool {
    unsigned int last_id;
    int auto_extend;
    mem_size_t mem_pool_size, total_mem_pool_size, max_mem_pool_size;
    struct _mp_mem_pool_list* mlist;
    void* lock;
} MemoryPool;

/*
 *  内部工具函数(调试用)
 */

// 所有Memory的数量
void get_memory_list_count(MemoryPool* mp, mem_size_t* mlist_len);
// 每个Memory的统计信息
void get_memory_info(MemoryPool* mp,
                     _MP_Memory* mm,
                     mem_size_t* free_list_len,
                     mem_size_t* alloc_list_len);
int get_memory_id(_MP_Memory* mm);

/*
 *  内存池API
 */

/**
 * 初始化内存池
 */
MemoryPool* MemoryPoolInit(mem_size_t maxmempoolsize, mem_size_t mempoolsize);

/**
 * 分配内存
 */
void* MemoryPoolAlloc(MemoryPool* mp, mem_size_t wantsize);

/**
 * 释放内存
 */
int MemoryPoolFree(MemoryPool* mp, void* p);

/**
 * 清空内存池
 */
MemoryPool* MemoryPoolClear(MemoryPool* mp);

/**
 * 销毁内存池
 */
int MemoryPoolDestroy(MemoryPool* mp);

/*
 *  内存池信息API
 */

// 总空间
mem_size_t GetTotalMemory(MemoryPool* mp);
// 实际分配空间
mem_size_t GetUsedMemory(MemoryPool* mp);
float MemoryPoolGetUsage(MemoryPool* mp);
// 数据占用空间
mem_size_t GetProgMemory(MemoryPool* mp);
float MemoryPoolGetProgUsage(MemoryPool* mp);


#ifdef __cplusplus
}
#endif

#endif