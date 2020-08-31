#ifndef _MOON_TIME_H
#define _MOON_TIME_H
#include "../cfg/environment.h"
#include "moon_base.h"

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * ����˵����
	 *    ��ȡ��ǰʱ��
	 * ������
	 *    time_str�����ʱ���ַ���
	 */
	void moon_current_time(_out_ char* time_str);

	/**
	 * ����˵����
	 *   ��ȡ��ǰutf8ʱ��
	 * ������
	 *   time_str�����ʱ���ַ���
	 */
	void moon_utf8_current_time(_out_ moon_char* time_str);

#ifdef __cplusplus
}
#endif

#endif