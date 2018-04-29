#include "moon_main.h"
#include "../core/server.h"
#include "../cfg/environment.h"

#ifdef __cplusplus
extern "C" {
#endif

/**window main entry point*/
#ifdef MS_WINDOWS
int windows_main(int argc, _TCHAR* argv[])
{
	moon_start();
	getchar();
	//提供操作命令窗体
	moon_stop();
	return 0;
}
#endif

/**linux main entry point*/
#ifdef LINUX
int linux_main(int argc, char **argv)
{
	printf("linux server");
	return 0;
}
#endif

#ifdef __cplusplus
}
#endif
