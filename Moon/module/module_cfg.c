#include <stdio.h>
#include <string.h>
#include "module_cfg.h"
#include "module_log.h"
#include "moon_file.h"
#include "moon_string.h"

#ifdef __cplusplus
extern "C" {
#endif

static const char *keys[] = {"server_ip", "server_port","http_port", "server_node_name","client_count","router_server_ip","log_level_debug","log_level_info","log_level_warnning","log_level_error"};/*define key*/

bool getValue(const char* keyAndValue,const char * key, moon_server_config* pConf)/*get value by key*/
{
	char val[1024] = {0};
	char tmpVal[1024] = {0};
	unsigned int keyLength = moon_get_string_length(key);
	moon_trim(&(keyAndValue[keyLength]),tmpVal);//
	moon_trim(&(tmpVal[1]),val);//
	if (strcmp("server_ip",key) == 0)
	{
		//
		memset(tmpVal,0,1024);
		memcpy(tmpVal,val,1024);
		memset(val,0,1024);
		moon_trim_line(tmpVal,val);
		//
		if(stringIsEmpty(val))
		{
			moon_write_error_log("server ip is null");
			return false;
		}
		strcpy(pConf->server_ip,val);
	}
	else if(strcmp("server_port",key) == 0)
	{
		//
		memset(tmpVal,0,1024);
		memcpy(tmpVal,val,1024);
		memset(val,0,1024);
		moon_trim_line(tmpVal,val);
		if(stringIsEmpty(val))
		{
			moon_write_error_log("server port is null");
			return false;
		}
		if (!moon_string_parse_to_int(val,&(pConf->server_port)))
		{
			return false;
		}
	}
	else if(strcmp("http_port",key) == 0)
	{
		//
		memset(tmpVal,0,1024);
		memcpy(tmpVal,val,1024);
		memset(val,0,1024);
		moon_trim_line(tmpVal,val);
		if(stringIsEmpty(val))
		{
			moon_write_error_log("http_port is null");
			return false;
		}
		if (!moon_string_parse_to_int(val,&(pConf->http_port)))
		{
			return false;
		}
	}
	else if(strcmp("server_node_name",key) == 0)
	{
		//
		memset(tmpVal,0,1024);
		memcpy(tmpVal,val,1024);
		memset(val,0,1024);
		moon_trim_line(tmpVal,val);
		if(stringIsEmpty(val))
		{
			moon_write_error_log("server node name is null");
			return false;
		}
		strcpy(pConf->server_node_name,val);
	}
	else if(strcmp("client_count",key) == 0)
	{
		//
		memset(tmpVal,0,1024);
		memcpy(tmpVal,val,1024);
		memset(val,0,1024);
		moon_trim_line(tmpVal,val);
		if(stringIsEmpty(val))
		{
			moon_write_error_log("client count is null");
			return false;
		}
		if (!moon_string_parse_to_int(val,&(pConf->client_count)))
		{
			return false;
		}
	}
	else if(strcmp("log_level_debug",key) == 0)
	{
		//
		memset(tmpVal,0,1024);
		memcpy(tmpVal,val,1024);
		memset(val,0,1024);
		moon_trim_line(tmpVal,val);
		if(stringIsEmpty(val))
		{
			return true;
		}
		if (moon_get_string_length(val) > 1)
		{
			moon_write_error_log("log_level_debug must be Y OR N");
			return false;
		}
		strcpy(&pConf->log_level_debug,val);
	}
	else if(strcmp("log_level_info",key) == 0)
	{
		//
		memset(tmpVal,0,1024);
		memcpy(tmpVal,val,1024);
		memset(val,0,1024);
		moon_trim_line(tmpVal,val);
		if(stringIsEmpty(val))
		{
			return true;
		}
		if (moon_get_string_length(val) > 1)
		{
			moon_write_error_log("log_level_info must be Y OR N");
			return false;
		}
		strcpy(&pConf->log_level_info,val);
	}
	else if(strcmp("log_level_warnning",key) == 0)
	{
		//
		memset(tmpVal,0,1024);
		memcpy(tmpVal,val,1024);
		memset(val,0,1024);
		moon_trim_line(tmpVal,val);
		if(stringIsEmpty(val))
		{
			return true;
		}
		if (moon_get_string_length(val) > 1)
		{
			moon_write_error_log("log_level_warnning must be Y OR N");
			return false;
		}
		strcpy(&pConf->log_level_warnning,val);
	}
	else if(strcmp("log_level_error",key) == 0)
	{
		//
		memset(tmpVal,0,1024);
		memcpy(tmpVal,val,1024);
		memset(val,0,1024);
		moon_trim_line(tmpVal,val);
		if(stringIsEmpty(val))
		{
			return true;
		}
		if (moon_get_string_length(val) > 1)
		{
			moon_write_error_log("log_level_error must be Y OR N");
			return false;
		}
		strcpy(&pConf->log_level_error,val);
	}
	else if(strcmp("router_server_ip",key) == 0)
	{
		//
		memset(tmpVal,0,1024);
		memcpy(tmpVal,val,1024);
		memset(val,0,1024);
		moon_trim_line(tmpVal,val);
		if(stringIsEmpty(val))
		{
			moon_write_error_log("router_server_ip");
			return false;
		}
		strcpy(pConf->router_server_ip,val);
	}
}

bool load_config(moon_server_config* pConf)//load configuration file
{
	bool bFlag = true;
	FILE* cfgFile = NULL;
	int iKey = 0;
	char keyValue[1024] = {0};
	char* confFileStream = NULL;
	char* lineConfig = NULL;
	char path[1024] = {0};
	moon_write_info_log("load config");
#ifdef LINUX
	//get moon current work path
	getcwd(path , 1024);
	strcat(path,"/");
	strcat(path,MOON_SERVER_CONF_FILE);
#endif
#ifdef MS_WINDOWS
	strcpy(path,MOON_SERVER_CONF_FILE);
#endif
	if( NULL == (cfgFile = fopen(path, "r")))
	{
		moon_write_error_log("can not open config file");
		return false;
	}
	confFileStream = (char*)moon_malloc(MOON_CONF_FILE_SIZE);
	if(confFileStream == NULL)
	{
		moon_write_error_log("create memory faild");
		return false;
	}
	moon_memory_zero(confFileStream,MOON_CONF_FILE_SIZE);
	memset(confFileStream,0,MOON_CONF_FILE_SIZE);
	lineConfig = (char*)moon_malloc(MOON_CONF_FILE_SIZE);
	if(lineConfig == NULL)
	{
		moon_write_error_log("create memory faild");
		return false;
	}
	moon_memory_zero(lineConfig,MOON_CONF_FILE_SIZE);
	while(fgets(confFileStream, MOON_CONF_FILE_SIZE, cfgFile) != NULL)/*read line*/
	{
		//
		moon_trim(confFileStream,lineConfig);
		if('#' == lineConfig[0] || '\n' == lineConfig[0])
		{
			continue;
		}
		iKey = 0;
		while(NULL != keys[iKey])
		{
			//getValue(lineConfig,keys[iKey],keyValue);
			//iKey++;
			if(strstr(lineConfig,keys[iKey]) != NULL && index_of(lineConfig,keys[iKey]) == 0)
			{
				if (!getValue(lineConfig,keys[iKey],pConf))
				{
					memset(keyValue,0,1024);
					sprintf(keyValue,"load config error,key and value:%s",lineConfig);
					moon_write_error_log(keyValue);
					moon_free(confFileStream);
					moon_free(lineConfig);
					return false;
				}
				break;
			}
			iKey++;
		}
		//
		memset(confFileStream,0,MOON_CONF_FILE_SIZE);
		memset(lineConfig,0,MOON_CONF_FILE_SIZE);
	}
	moon_free(confFileStream);
	moon_free(lineConfig);
	fclose(cfgFile);
	//////////////////////////////////////////////////////////////////////////log
	moon_write_info_log("The read main configuration is as follows:");
	//
	if(stringIsEmpty(pConf->server_ip))
	{
		moon_write_error_log("server ip is null");
		return false;
	}
	memset(keyValue,0,1024);
	sprintf(keyValue,"server_ip:%s",pConf->server_ip);
	moon_write_info_log(keyValue);
	memset(keyValue,0,1024);
	sprintf(keyValue,"server_port:%d",pConf->server_port);
	moon_write_info_log(keyValue);
	memset(keyValue,0,1024);
	sprintf(keyValue,"http_server_port:%d",pConf->http_port);
	moon_write_info_log(keyValue);
	if(stringIsEmpty(pConf->server_node_name))
	{
		moon_write_error_log("server_node_name is null");
		return false;
	}
	memset(keyValue,0,1024);
	sprintf(keyValue,"server_node_name:%s",pConf->server_node_name);
	moon_write_info_log(keyValue);
	memset(keyValue,0,1024);
	sprintf(keyValue,"client_count:%d",pConf->client_count);
	moon_write_info_log(keyValue);
	memset(keyValue,0,1024);
	sprintf(keyValue,"router_server_ip:%s",pConf->router_server_ip);
	moon_write_info_log(keyValue);
	memset(keyValue,0,1024);
	return bFlag;
}

#ifdef __cplusplus
}
#endif