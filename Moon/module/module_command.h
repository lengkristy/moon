#ifndef _COMMAND_H
#define _COMMAND_H

#ifdef __cplusplus
extern "C" {
#endif

#define COMMAND_BUF_SIZE 128 //定义输入命令的缓冲区大小


	/**
	 * 函数说明：
	 *   命令处理，等待接收用户输入的命令，直到输入退出命令
	 *   相关的命令主要是有如下：
	 *   1、show moon #显示当前moon通信服务器节点的数量
	 *   2、stop #停止服务
	 *   
	 */
	void process_command();

#ifdef __cplusplus
}
#endif

#endif