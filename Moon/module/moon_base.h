/************************************************************************
 * this file used to define type
 * coding by: haoran dai
 * time:2018-5-1 21:56                               
 ***********************************************************************/
#pragma once
#ifndef _MOON_CHAR_H
#define _MOON_CHAR_H
#include "../cfg/environment.h"
#ifdef MS_WINDOWS
#include <wchar.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ����ʹ�õ�utf8���͵�moon_char��ʹ��moon_char��������utf-8���ַ�
 */
typedef char moon_char;

/**
 * ����moon_char���ֽڳ��ȣ��ֽڳ���Ϊutf8�ı���ռ���ֽڴ�С
 */
#define MOON_CHAR_SIZE 3

/**
 * moon char copy
 */
void moon_char_copy(moon_char* destStr,const moon_char* sourceStr);

/**
 * moon char concat
 */
void moon_char_concat(moon_char* destStr,const moon_char* sourceStr);

/**
 * function desc:
 *  make char string to moon char string
 * params:
 *	sourceStr:char string
 *	destStr:out moon char string
 */
void char_to_moon_char(const char* sourceStr,_out_ moon_char* destStr);

/**
 * @desc make moon char to char string
 * @param source_str:moon char string
 * @param dest_str:char string after convertting
 **/
void moon_char_to_char(const moon_char* source_str,_out_ char* dest_str);

/**
 * ����˵����
 *  �Ƚ������ַ����Ƿ����
 * ������
 *  source_str��Դ�ַ���
 *  desc_str��Ŀ���ַ���
 * ����ֵ��
 *  ��ȷ���true,����ȷ���false
 */
bool moon_char_equals(const moon_char* source_str,const moon_char* desc_str);


/**
 * ����˵����
 *  ʵ���ַ�����ʽ��
 * ������
 *  buf����ʽ������ĵ�ַ
 *  fmt����ʽ���ַ���
 *  ...����������
 */
int moon_sprintf(moon_char *buf, const moon_char *fmt, ...);



#ifdef __cplusplus
}
#endif

#endif