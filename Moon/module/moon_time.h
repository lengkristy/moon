#ifndef _MOON_TIME_H
#define _MOON_TIME_H
#include "../cfg/environment.h"
#include "moon_base.h"

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * 函数说明：
	 *    获取当前时间
	 * 参数：
	 *    time_str：输出时间字符串
	 */
	void moon_current_time(_out_ char* time_str);

	/**
	 * 函数说明：
	 *   获取当前utf8时间
	 * 参数：
	 *   time_str：输出时间字符串
	 */
	void moon_utf8_current_time(_out_ moon_char* time_str);

#ifdef __cplusplus
}
#endif

#endif