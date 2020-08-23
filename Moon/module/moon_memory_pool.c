#include "../cfg/environment.h"
#include "../collection/array_list.h"
#include "moon_memory_pool.h"
#include "moon_base.h"
#include "moon_string.h"
#include <malloc.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MS_WINDOWS
#include <windows.h>
#endif

#define MP_CHUNKHEADER sizeof(struct _mp_chunk)
#define MP_CHUNKEND sizeof(struct _mp_chunk*)

/**
 * 计算4字节对齐，如果分配的内存不是4字节的倍数，那么将补充到4字节的倍数
 */
#define MP_ALIGN_SIZE(_n) (_n + sizeof(long) - ((sizeof(long) - 1) & _n))

#define MP_INIT_MEMORY_STRUCT(mm, mem_sz)       \
    do {                                        \
        mm->mem_pool_size = mem_sz;             \
        mm->alloc_mem = 0;                      \
        mm->alloc_prog_mem = 0;                 \
        mm->free_list = (_MP_Chunk*) mm->start; \
        mm->free_list->is_free = 1;             \
        mm->free_list->alloc_mem = mem_sz;      \
        mm->free_list->prev = NULL;             \
        mm->free_list->next = NULL;             \
        mm->alloc_list = NULL;                  \
    } while (0)

#define MP_DLINKLIST_INS_FRT(head, x) \
    do {                              \
        x->prev = NULL;               \
        x->next = head;               \
        if (head) head->prev = x;     \
        head = x;                     \
    } while (0)

#define MP_DLINKLIST_DEL(head, x)                 \
    do {                                          \
        if (!x->prev) {                           \
            head = x->next;                       \
            if (x->next) x->next->prev = NULL;    \
        } else {                                  \
            x->prev->next = x->next;              \
            if (x->next) x->next->prev = x->prev; \
        }                                         \
    } while (0)

void* moon_malloc(unsigned int num_bytes)/*allocate memory*/
{
	void* pMemory = malloc(num_bytes);
	if(pMemory != NULL)
	{
		memset(pMemory,0,num_bytes);
	}
	return pMemory;
}

void* moon_realloc(void* memory,unsigned int num_bytes)//在memory基础上重新分配内存
{
	if(memory == NULL)
	{
		return NULL;
	}
	if(num_bytes <= 0)
	{
		return memory;
	}
	return realloc(memory,num_bytes);
}

void moon_free(void* memory)/*free point*/
{
	if(memory != NULL)
	{
		free(memory);
	}
}

void moon_memory_zero(void* memory,unsigned int num_bytes)/*memory zero*/
{
	if(memory == NULL)
	{
		return;
	}
	memset(memory,0,num_bytes);
}

//加锁
void MP_LOCK(MemoryPool* mp)
{
    do 
	{                                    
#ifdef MS_WINDOWS
		WaitForSingleObject(mp->lock, INFINITE);
#endif
    } while (0);
}

//解锁
void MP_UNLOCK(MemoryPool* mp)                    
{
    do
	{   
#ifdef MS_WINDOWS
       SetEvent(mp->lock);
#endif
    } while (0);
}

void get_memory_list_count(MemoryPool* mp, mem_size_t* mlist_len) {
	
    mem_size_t mlist_l = 0;
    _MP_Memory* mm = NULL;
	MP_LOCK(mp);
	mm = mp->mlist;
    while (mm) {
        mlist_l++;
        mm = mm->next;
    }
    *mlist_len = mlist_l;
	MP_UNLOCK(mp);
}

void get_memory_info(MemoryPool* mp,
                     _MP_Memory* mm,
                     mem_size_t* free_list_len,
                     mem_size_t* alloc_list_len) {
    mem_size_t free_l = 0, alloc_l = 0;
    _MP_Chunk* p = NULL;
	MP_LOCK(mp);
	p = mm->free_list;
    while (p) {
        free_l++;
        p = p->next;
    }

    p = mm->alloc_list;
    while (p) {
        alloc_l++;
        p = p->next;
    }
    *free_list_len = free_l;
    *alloc_list_len = alloc_l;
	MP_UNLOCK(mp);
}


int get_memory_id(_MP_Memory* mm) {
    return mm->id;
}


static _MP_Memory* extend_memory_list(MemoryPool* mp, mem_size_t new_mem_sz) {
    char* s = (char*) malloc(sizeof(_MP_Memory) + new_mem_sz * sizeof(char));
	_MP_Memory* mm = (_MP_Memory*) s;
    if (!s) return NULL;

    mm->start = s + sizeof(_MP_Memory);

    MP_INIT_MEMORY_STRUCT(mm, new_mem_sz);
    mm->id = mp->last_id++;
    mm->next = mp->mlist;
    mp->mlist = mm;
    return mm;
}


static _MP_Memory* find_memory_list(MemoryPool* mp, void* p) {
    _MP_Memory* tmp = mp->mlist;
    while (tmp) {
        if (tmp->start <= (char*) p &&
            tmp->start + mp->mem_pool_size > (char*) p)
            break;
        tmp = tmp->next;
    }

    return tmp;
}


static int merge_free_chunk(MemoryPool* mp, _MP_Memory* mm, _MP_Chunk* c) {
    _MP_Chunk *p0 = c, *p1 = c;
    while (p0->is_free) {
        p1 = p0;
        if ((char*) p0 - MP_CHUNKEND - MP_CHUNKHEADER <= mm->start) break;
        p0 = *(_MP_Chunk**) ((char*) p0 - MP_CHUNKEND);
    }

    p0 = (_MP_Chunk*) ((char*) p1 + p1->alloc_mem);
    while ((char*) p0 < mm->start + mp->mem_pool_size && p0->is_free) {
        MP_DLINKLIST_DEL(mm->free_list, p0);
        p1->alloc_mem += p0->alloc_mem;
        p0 = (_MP_Chunk*) ((char*) p0 + p0->alloc_mem);
    }

    *(_MP_Chunk**) ((char*) p1 + p1->alloc_mem - MP_CHUNKEND) = p1;
	MP_UNLOCK(mp);
    return 0;
}


MemoryPool* MemoryPoolInit(mem_size_t maxmempoolsize, mem_size_t mempoolsize) {
	MemoryPool* mp = NULL;
	char* s  = NULL;
    if (mempoolsize > maxmempoolsize) {
        return NULL;
    }

    mp = (MemoryPool*) malloc(sizeof(MemoryPool));
    if (!mp) return NULL;
    mp->last_id = 0;
    if (mempoolsize < maxmempoolsize) mp->auto_extend = 1;
    mp->max_mem_pool_size = maxmempoolsize;/*分配最大内存大小*/
    mp->total_mem_pool_size = mp->mem_pool_size = mempoolsize;/*当前内存池大小*/

	/*开始内存分配*/
    s = (char*) malloc(sizeof(_MP_Memory) +
                             sizeof(char) * mp->mem_pool_size);
    if (!s) return NULL;

    mp->mlist = (_MP_Memory*) s;
	/*设置开始可用的内存区域*/
    mp->mlist->start = s + sizeof(_MP_Memory);
	/*初始化内存*/
    MP_INIT_MEMORY_STRUCT(mp->mlist, mp->mem_pool_size);
    mp->mlist->next = NULL;
    mp->mlist->id = mp->last_id++;
#ifdef MS_WINDOWS
	mp->lock = CreateEvent(NULL, FALSE, TRUE, TEXT("MOON_MEM_POOL"));
	if (mp->lock == NULL)
	{
		MemoryPoolFree(mp, NULL);
		MemoryPoolClear(mp);
		MemoryPoolDestroy(mp);
		return NULL;
	}
#endif
    return mp;
}


void* MemoryPoolAlloc(MemoryPool* mp, mem_size_t wantsize) {
	_MP_Memory* mm = NULL;
	_MP_Memory* mm1 = NULL;
	_MP_Chunk* _free = NULL;
	_MP_Chunk*_not_free = NULL;
	mem_size_t total_needed_size;
	mem_size_t add_mem_sz;
	MP_LOCK(mp);
	if (wantsize <= 0) return NULL;
	/*计算总共需要分配的空间，按照4字节对齐*/
    total_needed_size = MP_ALIGN_SIZE(wantsize + MP_CHUNKHEADER + MP_CHUNKEND);
    if (total_needed_size > mp->mem_pool_size) return NULL;
FIND_FREE_CHUNK:
    mm = mp->mlist;
    while (mm) {
        if (mp->mem_pool_size - mm->alloc_mem < total_needed_size) {
            mm = mm->next;
            continue;
        }

        _free = mm->free_list;
        _not_free = NULL;

        while (_free) {
            if (_free->alloc_mem >= total_needed_size) {
                // 如果free块分割后剩余内存足够大 则进行分割
                if (_free->alloc_mem - total_needed_size >
                    MP_CHUNKHEADER + MP_CHUNKEND) {
                    // 从free块头开始分割出alloc块
                    _not_free = _free;

                    _free = (_MP_Chunk*) ((char*) _not_free +
                                          total_needed_size);
                    *_free = *_not_free;
                    _free->alloc_mem -= total_needed_size;
                    *(_MP_Chunk**) ((char*) _free + _free->alloc_mem -
                                    MP_CHUNKEND) = _free;

                    // update free_list
                    if (!_free->prev) {
                        mm->free_list = _free;
                    } else {
                        _free->prev->next = _free;
                    }
                    if (_free->next) _free->next->prev = _free;

                    _not_free->is_free = 0;
                    _not_free->alloc_mem = total_needed_size;

                    *(_MP_Chunk**) ((char*) _not_free + total_needed_size -
                                    MP_CHUNKEND) = _not_free;
                }
                // 不够 则整块分配为alloc
                else {
                    _not_free = _free;
                    MP_DLINKLIST_DEL(mm->free_list, _not_free);
                    _not_free->is_free = 0;
                }
                MP_DLINKLIST_INS_FRT(mm->alloc_list, _not_free);

                mm->alloc_mem += _not_free->alloc_mem;
                mm->alloc_prog_mem +=
                        (_not_free->alloc_mem - MP_CHUNKHEADER - MP_CHUNKEND);
				MP_UNLOCK(mp);
                return (void*) ((char*) _not_free + MP_CHUNKHEADER);
            }
            _free = _free->next;
        }

        mm = mm->next;
    }

    if (mp->auto_extend) {
        // 超过总内存限制
        if (mp->total_mem_pool_size >= mp->max_mem_pool_size) {
            goto err_out;
        }
        add_mem_sz = mp->max_mem_pool_size - mp->mem_pool_size;
        add_mem_sz = add_mem_sz >= mp->mem_pool_size ? mp->mem_pool_size
                                                     : add_mem_sz;
        mm1 = extend_memory_list(mp, add_mem_sz);
        if (!mm1) {
            goto err_out;
        }
        mp->total_mem_pool_size += add_mem_sz;

        goto FIND_FREE_CHUNK;
    }
err_out:
	MP_UNLOCK(mp);
    return NULL;
}


int MemoryPoolFree(MemoryPool* mp, void* p) {
	_MP_Chunk* ck = NULL;
	_MP_Memory* mm = NULL;
    if (p == NULL || mp == NULL) return 1;
	MP_LOCK(mp);
    mm = mp->mlist;
    if (mp->auto_extend) mm = find_memory_list(mp, p);

    ck = (_MP_Chunk*) ((char*) p - MP_CHUNKHEADER);

    MP_DLINKLIST_DEL(mm->alloc_list, ck);
    MP_DLINKLIST_INS_FRT(mm->free_list, ck);
    ck->is_free = 1;

    mm->alloc_mem -= ck->alloc_mem;
    mm->alloc_prog_mem -= (ck->alloc_mem - MP_CHUNKHEADER - MP_CHUNKEND);

    return merge_free_chunk(mp, mm, ck);
}

MemoryPool* MemoryPoolClear(MemoryPool* mp) {
	_MP_Memory* mm = NULL;
    if (!mp) return NULL;
	MP_LOCK(mp);
    mm = mp->mlist;
    while (mm) {
        MP_INIT_MEMORY_STRUCT(mm, mm->mem_pool_size);
        mm = mm->next;
    }
	MP_UNLOCK(mp);
    return mp;
}


int MemoryPoolDestroy(MemoryPool* mp) {
	_MP_Memory *mm = NULL;
	_MP_Memory *mm1 = NULL;
    if (mp == NULL) return 1;
	MP_LOCK(mp);
    mm = mp->mlist;
    while (mm) {
        mm1 = mm;
        mm = mm->next;
        free(mm1);
    }
	MP_UNLOCK(mp);
#ifdef MS_WINDOWS
	CloseHandle(mp->lock);
#endif
    free(mp);
    return 0;
}


mem_size_t GetTotalMemory(MemoryPool* mp) {
    return mp->total_mem_pool_size;
}


mem_size_t GetUsedMemory(MemoryPool* mp) {
    mem_size_t total_alloc = 0;
	_MP_Memory* mm = NULL;
	MP_LOCK(mp);
    mm = mp->mlist;
    while (mm) {
        total_alloc += mm->alloc_mem;
        mm = mm->next;
    }
	MP_UNLOCK(mp);
    return total_alloc;
}


mem_size_t GetProgMemory(MemoryPool* mp) {
    mem_size_t total_alloc_prog = 0;
	_MP_Memory* mm = NULL;
	MP_LOCK(mp);
    mm = mp->mlist;
    while (mm) {
        total_alloc_prog += mm->alloc_prog_mem;
        mm = mm->next;
    }
	MP_UNLOCK(mp);
    return total_alloc_prog;
}


float MemoryPoolGetUsage(MemoryPool* mp) {
    return (float) GetUsedMemory(mp) / GetTotalMemory(mp);
}


float MemoryPoolGetProgUsage(MemoryPool* mp) {
    return (float) GetProgMemory(mp) / GetTotalMemory(mp);
}

#undef MP_CHUNKHEADER
#undef MP_CHUNKEND
#undef MP_LOCK
#undef MP_ALIGN_SIZE
#undef MP_INIT_MEMORY_STRUCT
#undef MP_DLINKLIST_INS_FRT
#undef MP_DLINKLIST_DEL

#ifdef __cplusplus
}
#endif