#include "moon_time.h"
#include <time.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * 函数说明：
	 *    获取当前时间
	 * 参数：
	 *    time：输出时间字符串
	 */
	void moon_current_time(_out_ char* time_str)
	{
		time_t rawtime; 
		struct tm * timeinfo; 
		time(&rawtime); 
		timeinfo = localtime( &rawtime ); 
		sprintf (time_str,"%d/%d/%d %d:%d:%d ",
				timeinfo->tm_year + 1900,timeinfo->tm_mon + 1,timeinfo->tm_mday,
				timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
	}

#ifdef __cplusplus
}
#endif