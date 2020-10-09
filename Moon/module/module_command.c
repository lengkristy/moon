#include "module_command.h"
#include "moon_string.h"
#include "../cfg/environment.h"


#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif

	extern void moon_stop();

	/**
	 * ����˵����
	 *   ִ������
	 * ������
	 *   commands�������б���һ��Ԫ��Ϊ���������Ԫ��Ϊ�����б�
	 *   count���б�����
	 */
	static void _exec_command(char *commands[],int count)
	{
		char *cmd = commands[0];
		if (strcmp(cmd,"stop") == 0)
		{
			moon_stop();
			exit(0);
			return;
		}
		else
		{
			printf("invalid command\n\r");
		}
	}

	/**
	 * ����˵����
	 *   ��������
	 * ������
	 *   str_cmd���û�����������ַ���
	 */
	static void _parse_command(char* str_cmd)
	{
		//ͨ���ո���зָ�
		int length = 0;
		int i = 0;
		char *commands[20] = {0};
		int cmd_count = 0;//��������
		char *p_s;//��ȡ���ʼλ��
		p_s = str_cmd;
		length = moon_get_string_length(str_cmd);
		for (i = 0; i < length; i++)
		{
			if (cmd_count > 20)
			{
				moon_write_error_log("command param too long");
				return;
			}
			if (str_cmd[i] == ' ')
			{
				if (i > 0 && str_cmd[i - 1] == ' ')
				{
					p_s = &(str_cmd[i]) + 1;
					continue;
				}
				commands[cmd_count] = (char*)moon_malloc(&(str_cmd[i]) - p_s + 1);
				memset(commands[cmd_count],0,&(str_cmd[i]) - p_s + 1);
				memcpy(commands[cmd_count],p_s,&(str_cmd[i]) - p_s);
				cmd_count++;
			}
			else if(i == length - 1)
			{
				commands[cmd_count] = (char*)moon_malloc(&(str_cmd[i]) - p_s + 1);
				memset(commands[cmd_count],0,&(str_cmd[i]) - p_s + 1);
				memcpy(commands[cmd_count],p_s,&(str_cmd[i]) - p_s);
				p_s = &(str_cmd[i]) + 1;
				cmd_count++;
			}
		}

		//����������ж�
		_exec_command(commands,cmd_count);

		//ʹ������ͷ��ڴ�
		for (i = 0;i < cmd_count;i++)
		{
			p_s = commands[i];
			moon_free(p_s);
		}
	}

	/**
	 * ����˵����
	 *   ������ȴ������û���������ֱ�������˳�����
	 *   
	 */
	void process_command()
	{
		char command_buf[COMMAND_BUF_SIZE] = {0};
		while(true)
		{
			printf("moon server> ");
			fflush(stdout);
			fgets(command_buf,COMMAND_BUF_SIZE,stdin);
			_parse_command(command_buf);
			memset(command_buf,0,COMMAND_BUF_SIZE);
		}
	}

#ifdef __cplusplus
}
#endif