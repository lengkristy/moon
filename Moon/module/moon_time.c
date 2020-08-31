#include "moon_time.h"
#include <time.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * ����˵����
	 *    ��ȡ��ǰʱ��
	 * ������
	 *    time�����ʱ���ַ���
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

	/**
	 * ����˵����
	 *   ��ȡ��ǰutf8ʱ��
	 * ������
	 *   time_str�����ʱ���ַ���
	 */
	void moon_utf8_current_time(_out_ moon_char* time_str)
	{
		char time[30] = {0};
		moon_current_time(time);
		char_to_moon_char(time,time_str);
	}
#ifdef __cplusplus
}
#endif