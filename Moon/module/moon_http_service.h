/**************************************************************************
 * this file is moon http service                                      
 * coding by haoran dai
 * time 2018-04-30 11:24
 * purpose:
 * 	This module is primarily an interface service that provides moonmanage.
 **************************************************************************/
#pragma once
#ifndef _MOON_HTTP_SERVICE_H
#define _MOON_HTTP_SERVICE_H

#ifdef MS_WINDOWS
#include <winsock2.h>
#include <MSWSock.h>
#pragma comment(lib,"ws2_32.lib")
#endif

#ifdef __cplusplus
extern "C" {
#endif

	/************************************************************************/
	/* http context that parameters passed to the thread.                   */
	/************************************************************************/
	typedef struct _moon_http_context{
#ifdef MS_WINDOWS
		SOCKET m_socket;
#endif
	}moon_http_context;


	/************************************************************************/
	/* start http service                                                   */
	/************************************************************************/
	bool lauch_http_service();

	/************************************************************************/
	/* stop http service                                                    */
	/************************************************************************/
	bool end_http_service();

#ifdef __cplusplus
}
#endif

#endif