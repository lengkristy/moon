// Moon.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <locale.h>
#include "cfg/environment.h"
#include "app/moon_main.h"

#pragma comment (lib,"User32.lib")



#ifdef MS_WINDOWS
int _tmain(int argc, _TCHAR* argv[])
{
	HWND hWnd = GetConsoleWindow();
	SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(NULL, _T("IDI_ICON3")));//设置图标
	return moon_main(argc,argv);
}
#else
int main(int argc, char **argv)
{
	return 0;
}
#endif