#include "../cfg/environment.h"
#include "../collection/array_list.h"
#include <malloc.h>
#ifdef __cplusplus
extern "C" {
#endif

/*every memory block use size*/
static unsigned long moon_memory_block_size;

/**
 * define memory use record
 */
typedef struct _moon_memory_use_record{
	/*use size*/
	unsigned long size;
	/*use start address*/
	unsigned long start_address;
	/*belong to which memory block*/
	unsigned int memory_block_id;
}moon_memory_use_record;

/**
 * define moon memory struct
 */
typedef struct _moon_memory_block{
	/*the unique identifier of the current block*/
	unsigned int id;
	/*every memory block size,the size is calculated by different computers*/
	unsigned long size;
	/*the start address of the memory block*/
	unsigned long start_address;
	/**used size*/
	unsigned long used_size;
	/*current block use record*/
	Array_List* use_record_list;
}moon_memory_block;


/**
 * function desc:
 * 		initialize moon memory pool
 */
void moom_memory_pool_init()
{
	void* pBaseBlock = malloc(1024*1024);
}


void* moon_malloc(unsigned int num_bytes)/*allocate memory*/
{
	void* pMemory = malloc(num_bytes);
	memset(pMemory,0,num_bytes);
	return pMemory;
}

void moon_free(void* memory)/*free point*/
{
	if(memory != NULL)
	{
		free(memory);
	}
}

/**
 * function desc:
 * 		destory moon memory pool
 */
void moom_memory_pool_destory()
{

}

#ifdef __cplusplus
}
#endif