#include "moon_main.h"
#include "../core/server.h"

#ifdef __cplusplus
extern "C" {
#endif

int moon_main(int argc, _TCHAR* argv[])
{
	moon_start();
	getchar();
	//提供操作命令窗体
	moon_stop();
	return 0;
}

#ifdef __cplusplus
}
#endif