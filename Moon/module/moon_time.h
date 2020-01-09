#ifndef _MOON_TIME_H
#define _MOON_TIME_H
#include "../cfg/environment.h"

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * 函数说明：
	 *    获取当前时间
	 * 参数：
	 *    time：输出时间字符串
	 */
	void moon_current_time(_out_ char* time_str);

#ifdef __cplusplus
}
#endif

#endif