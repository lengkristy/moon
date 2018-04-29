#pragma once
#ifndef _MOONMAIN_H
#define _MOONMAIN_H

#ifdef MS_WINDOWS
#include <tchar.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**window main entry point*/
#ifdef MS_WINDOWS
int windows_main(int argc, _TCHAR* argv[]);//windows Moon入口
#endif




#ifdef __cplusplus
}
#endif

#endif
