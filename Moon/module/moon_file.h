/************************************************************************
 * this file used to process file
 * coding by: haoran dai
 * time:2018-5-11 23:17                               
 ***********************************************************************/
#ifndef _MOON_FILE_H
#define _MOON_FILE_H
#include "../cfg/environment.h"
#include <stdio.h>

/**
 * moon file struct define
 */
typedef struct _moon_file{
    /**
     * file size,byte (unit)
     */
    unsigned long file_size;
    /**
     * file path without path
     */
    unsigned char file_name[256];
    /**
     * file path with out file name
     */
    unsigned char file_path_name[1024];
    /**
     * file pointer
     */
    FILE* file_ptr;
}moon_file,*moon_file_ptr;

/**
 * function desc:
 *      Read only open existing file,and invoke moon_close_file function to close it after use.
 * params:
 *      p_moon_file:the pointer of _moon_file struct
 *      file_path_name:file full path name.
 * return:
 *      sucess return true and p_moon_file return the pointer of _moon_file struct,failed return false and p_moon_file is null
 */
bool moon_open_file_for_read(moon_file_ptr p_moon_file,char* file_path_name);

/**
 * function desc:
 *      close file that has be opened
 * params:
 *      p_moon_file:the pointer of _moon_file struct
 * return:
 *      sucess return true,failed return false
 */
bool moon_close_file(moon_file_ptr p_moon_file);

#endif