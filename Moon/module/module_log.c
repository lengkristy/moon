#include "../cfg/environment.h"
#include "module_log.h"
#include "moon_string.h"
#include <time.h>
#include <stdio.h>
#ifdef MS_WINDOWS
#include <windows.h>
#include <process.h>
#endif
#include "moon_config_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

static FILE* pFile = NULL;/*文件指针*/

#ifdef MS_WINDOWS
static HANDLE g_hMutex;
#endif

extern Moon_Server_Config* p_global_server_config;//外部全局配置变量
extern bool b_config_load_finish;

/*控制台日志*/
void moon_console_print(const char *log) //overwrite printf function with line feed
{
	time_t rawtime; 
	struct tm * timeinfo; 
	time(&rawtime); 
	timeinfo = localtime( &rawtime ); 
	printf ("%d/%d/%d %d:%d:%d ",
			timeinfo->tm_year + 1900,timeinfo->tm_mon + 1,timeinfo->tm_mday,
			timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
	printf(log);
	printf("\n\r");
}

/*写文件日志*/
void moon_file_print(const char* log)//write file log,thread sync
{

	time_t rawtime; 
	struct tm * timeinfo; 
	char strTime[255] = {0};

//需要做线程同步
#ifdef MS_WINDOWS
	HANDLE hMutex = OpenMutex(SYNCHRONIZE , TRUE, TEXT(LOG_MUTEX));
	if(hMutex == NULL)
	{
		moon_console_print("can not OpenMutex");
		return ;
	}
	//WaitforsingleObject将等待指定的一个mutex，直至获取到拥有权
	//通过互斥锁保证除非输出工作全部完成，否则其他线程无法输出。
	WaitForSingleObject(hMutex, 1000);
#endif
	//临界区
	time(&rawtime); 
	timeinfo = localtime( &rawtime ); 
	sprintf (strTime,"%d/%d/%d %d:%d:%d ",
		timeinfo->tm_year + 1900,timeinfo->tm_mon + 1,timeinfo->tm_mday,
		timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
	fwrite(strTime, sizeof(char), moon_get_string_length(strTime),pFile);
	fwrite(log, sizeof(char), moon_get_string_length(log),pFile);
	fwrite("\r\n",sizeof(char),2,pFile);
	fflush(pFile);

#ifdef MS_WINDOWS
	ReleaseMutex(hMutex);
	CloseHandle(hMutex);
#endif
}

/************************************************************************/
/* 写日志环境初始化，主要是为了初始化日志文件以及线程互斥对象           */
/************************************************************************/
bool moon_log_init()
{
	bool bFlag = true;
	if( NULL == (pFile = fopen(MOON_LOG_FILE_PATH, "a+")))
	{
		return false;
	}
	//创建线程互斥体对象
#ifdef MS_WINDOWS
	g_hMutex = CreateMutex(NULL, FALSE,  TEXT(LOG_MUTEX));
	if (g_hMutex == NULL)
	{
		return false;
	}
#endif
	
	return bFlag;
}

/************************************************************************/
/* 关闭日志记录，关闭文件以及线程互斥对象                               */
/************************************************************************/
void moon_log_close()
{
	if(pFile != NULL)
	{
		fclose(pFile);
	}
	if (g_hMutex != NULL)
	{
		CloseHandle(g_hMutex);
	}
}

void moon_write_info_log(char* log)
{
	unsigned long length = 0;
	unsigned int infoLength = moon_get_string_length(LOG_INFO);
	char* pFormateLog = NULL;
	if (b_config_load_finish && p_global_server_config != NULL && 'Y' != p_global_server_config->log_level_info)//判断是否配置info日志级别,如果没有配置则不需要记录
		return;
	if (log == NULL)
	{
		return;
	}
	length = moon_get_string_length(log);
	pFormateLog = (char*)moon_malloc(length + infoLength + 4);
	sprintf(pFormateLog,"%s->%s",LOG_INFO,log);
	moon_console_print(pFormateLog);
	moon_file_print(pFormateLog);
	moon_free(pFormateLog);
}

void moon_write_error_log(char* log)
{
	unsigned long length = 0;
	unsigned int infoLength = moon_get_string_length(LOG_ERROR);
	char* pFormateLog = NULL;
	if (b_config_load_finish && p_global_server_config != NULL && 'Y' != p_global_server_config->log_level_error)
		return;
	if (log == NULL)
	{
		return;
	}
	length = moon_get_string_length(log);
	pFormateLog = (char*)moon_malloc(length + infoLength + 4);
	sprintf(pFormateLog,"%s->%s",LOG_ERROR,log);
	moon_console_print(pFormateLog);
	moon_file_print(pFormateLog);
	moon_free(pFormateLog);
}

void moon_write_warn_log(char* log)
{
	unsigned long length = 0;
	unsigned int infoLength = moon_get_string_length(LOG_WARN);
	char* pFormateLog = NULL;
	if (b_config_load_finish && p_global_server_config != NULL && 'Y' != p_global_server_config->log_level_warnning)
		return;
	if (log == NULL)
	{
		return;
	}
	length = moon_get_string_length(log);
	pFormateLog = (char*)moon_malloc(length + infoLength + 4);
	sprintf(pFormateLog,"%s->%s",LOG_WARN,log);
	moon_console_print(pFormateLog);
	moon_file_print(pFormateLog);
	moon_free(pFormateLog);
}

void moon_write_debug_log(char* log)
{
	unsigned long length = 0;
	unsigned int infoLength = moon_get_string_length(LOG_DEBUG);
	char* pFormateLog = NULL;
	if (b_config_load_finish && p_global_server_config != NULL && 'Y' != p_global_server_config->log_level_debug)
		return;
	if (log == NULL)
	{
		return;
	}
	length = moon_get_string_length(log);
	pFormateLog = (char*)moon_malloc(length + infoLength + 4);
	sprintf(pFormateLog,"%s->%s",LOG_DEBUG,log);
	moon_console_print(pFormateLog);
	moon_file_print(pFormateLog);
	moon_free(pFormateLog);
}

#ifdef __cplusplus
}
#endif