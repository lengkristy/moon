
/**************************************
 * 该文件为环境配置文件
 * 
 **************************************/
#ifndef _ENVIRONMENT_H
#define _ENVIRONMENT_H

#ifdef _MSC_VER/* only support win32 and greater. */
#define MS_WINDOWS
#endif

/*define NULL */
#ifndef NULL
#define NULL 0
#endif

/*define bool*/
#ifndef bool
#define bool unsigned int
#define true 1
#define false 0
#endif

#define MOON_CONF_PATH ".\\config" /*define moon configuration dir*/

#define MOON_SERVER_CONF_FILE ".\\config\\moon.cfg" /* server configuration file path*/

#define MOON_CONF_FILE_SIZE 1024*1024 /*define config file size*/

#define MOON_LOG_FILE_PATH ".\\log\\moon.log" /*server log path*/

#endif