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

static FILE* pFile = NULL;/*log file point*/

#ifdef MS_WINDOWS
static HANDLE g_hMutex;
#endif

extern Moon_Server_Config* p_global_server_config;//global configuration struct
extern bool b_config_load_finish;

/*console output log*/
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

/*write file log*/
void moon_file_print(const char* log)//write file log,thread sync
{

	time_t rawtime; 
	struct tm * timeinfo; 
	char strTime[255] = {0};
	if(pFile == NULL){/*file is NULL,and return*/
		return;
	}
//thread synchronous
#ifdef MS_WINDOWS
	HANDLE hMutex = OpenMutex(SYNCHRONIZE , TRUE, TEXT(LOG_MUTEX));
	if(hMutex == NULL)
	{
		moon_console_print("can not OpenMutex");
		return ;
	}
	//get mutexes object
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
/* log init													           */
/************************************************************************/
bool moon_log_init()
{
	bool bFlag = true;
	pFile = fopen(MOON_LOG_FILE_PATH, "a+");
	//pFile = fopen("/home/lengyue/workspace/cpp_work/moon/linux_debug/log/moon.log", "a+");
	if(NULL == pFile)
		return false;
	//create windows mutexes object
#ifdef MS_WINDOWS
	g_hMutex = CreateMutex(NULL, FALSE,  TEXT(LOG_MUTEX));
	if (g_hMutex == NULL)
	{
		return false;
	}
#endif
	moon_write_info_log("init log environment finished");
	return bFlag;
}

/************************************************************************/
/* close log record            			                               */
/************************************************************************/
void moon_log_close()
{
	if(pFile != NULL)
	{
		fclose(pFile);
	}
#ifdef MS_WINDOWS
	if (g_hMutex != NULL)
	{
		CloseHandle(g_hMutex);
	}
#endif
}

void moon_write_info_log(char* log)
{
	unsigned long length = 0;
	unsigned int infoLength = moon_get_string_length(LOG_INFO);
	char* pFormateLog = NULL;
	//info level log record
	if (b_config_load_finish && p_global_server_config != NULL && 'Y' != p_global_server_config->log_level_info)
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