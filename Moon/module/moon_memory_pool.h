/************************************************************************
 * 
 * file desc:
 *      this file contain moon memery manage.
 * coding by: haoran dai
 * time:2018-5-1 21:56                               
 ***********************************************************************/
#pragma once
#ifndef _MOON_MALLOC_H
#define _MOON_MALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

extern void* moon_malloc(unsigned int num_bytes);/*allocate memory*/

extern void moon_free(void* memory);/*free point*/

extern void* moon_realloc(void* memory,unsigned int num_bytes);//��memory���������·����ڴ�

extern void moon_memory_zero(void* memory,unsigned int num_bytes);/*memory zero*/

#ifdef __cplusplus
}
#endif

#endif