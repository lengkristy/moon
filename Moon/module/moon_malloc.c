#include "../cfg/environment.h"
#include <malloc.h>
#ifdef __cplusplus
extern "C" {
#endif

void* moon_malloc(unsigned int num_bytes)/*allocate memory*/
{
	/*以后需要增加内存池的管理*/
	void* pMemory = malloc(num_bytes);
	memset(pMemory,0,num_bytes);//初始化为0
	return pMemory;
}

void moon_free(void* memory)/*free point*/
{
	if(memory != NULL)
	{
		free(memory);
	}
}

#ifdef __cplusplus
}
#endif