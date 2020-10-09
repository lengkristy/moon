#include "module_command.h"
#include "moon_string.h"
#include "../cfg/environment.h"


#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif

	extern void moon_stop();

	/**
	 * 函数说明：
	 *   执行命令
	 * 参数：
	 *   commands：命令列表，第一个元素为命令本身，后面元素为参数列表
	 *   count：列表数量
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
	 * 函数说明：
	 *   解析命令
	 * 参数：
	 *   str_cmd：用户输入的命令字符串
	 */
	static void _parse_command(char* str_cmd)
	{
		//通过空格进行分割
		int length = 0;
		int i = 0;
		char *commands[20] = {0};
		int cmd_count = 0;//命令数量
		char *p_s;//截取命令开始位置
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

		//进行命令的判断
		_exec_command(commands,cmd_count);

		//使用完成释放内存
		for (i = 0;i < cmd_count;i++)
		{
			p_s = commands[i];
			moon_free(p_s);
		}
	}

	/**
	 * 函数说明：
	 *   命令处理，等待接收用户输入的命令，直到输入退出命令
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