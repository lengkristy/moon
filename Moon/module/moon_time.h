#ifndef _MOON_TIME_H
#define _MOON_TIME_H
#include "../cfg/environment.h"

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * ����˵����
	 *    ��ȡ��ǰʱ��
	 * ������
	 *    time�����ʱ���ַ���
	 */
	void moon_current_time(_out_ char* time_str);

#ifdef __cplusplus
}
#endif

#endif