#ifndef _COMMAND_H
#define _COMMAND_H

#ifdef __cplusplus
extern "C" {
#endif

#define COMMAND_BUF_SIZE 128 //������������Ļ�������С


	/**
	 * ����˵����
	 *   ������ȴ������û���������ֱ�������˳�����
	 *   ��ص�������Ҫ�������£�
	 *   1��show moon #��ʾ��ǰmoonͨ�ŷ������ڵ������
	 *   2��stop #ֹͣ����
	 *   
	 */
	void process_command();

#ifdef __cplusplus
}
#endif

#endif