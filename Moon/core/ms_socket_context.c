#include "ms_socket_context.h"
#include "../module/moon_memory_pool.h"

#ifdef MS_WINDOWS
#ifdef __cplusplus
extern "C" {
#endif

/*Create a new IO context*/
PMS_IO_CONTEXT create_new_io_context()
{
	PMS_IO_CONTEXT context = (MS_IO_CONTEXT*)moon_malloc(sizeof(MS_IO_CONTEXT));
	memset(&context->m_Overlapped,0, sizeof(context->m_Overlapped));
	memset(context->m_szBuffer,0,PKG_BYTE_MAX_LENGTH );
	context->m_sockAccept = INVALID_SOCKET;
	context->m_wsaBuf.buf = context->m_szBuffer;
	context->m_wsaBuf.len = PKG_BYTE_MAX_LENGTH;
	context->m_OpType     = NULL_POSTED;
	return context;
}

/*Release IO context*/
void free_io_context(PMS_IO_CONTEXT context)
{
	moon_free(context);
}

/*Create a new socket context.*/
PMS_SOCKET_CONTEXT create_new_socket_context()
{
	PMS_SOCKET_CONTEXT context = (PMS_SOCKET_CONTEXT)moon_malloc(sizeof(MS_SOCKET_CONTEXT));
	memset(context,0,sizeof(MS_SOCKET_CONTEXT));
	InitializeCriticalSection(&context->SockCritSec);
	context->m_pListIOContext = array_list_init();
	context->m_currentPkgSize = 0;
	context->m_pkgSize = 0;
	return context;
}

/*Release the socket context.*/
void free_socket_context(PMS_SOCKET_CONTEXT context)
{
	int i = 0;
	if (context->m_pListIOContext != NULL && context->m_pListIOContext->length > 0)
	{
		for (i = 0;i < context->m_pListIOContext->length;i++)
		{
			free_io_context((PMS_IO_CONTEXT)array_list_getAt(context->m_pListIOContext,i));
		}
		array_list_free(context->m_pListIOContext);
	}
	moon_free(context);
}

#ifdef __cplusplus
}
#endif
#endif