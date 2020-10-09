#include "moon_main.h"
#include "../core/server.h"
#include "../cfg/environment.h"
#include <stdio.h>
#ifdef MS_WINDOWS
#include <tchar.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**window main entry point*/
#ifdef MS_WINDOWS
int windows_main(int argc, _TCHAR* argv[])
{
	//��������
	moon_start();
	return 0;
}
#endif

/**linux main entry point*/
#ifdef LINUX
int linux_main(int argc, char **argv)
{
	moon_start();
	getchar();
	//cammand option
	moon_stop();
	return 0;
}
#endif

#ifdef __cplusplus
}
#endif
