#include "ms_socket_context.h"
#include "../module/moon_malloc.h"

#ifdef MS_WINDOWS
#ifdef __cplusplus
extern "C" {
#endif

/*创建新的IO上下文*/
PMS_IO_CONTEXT create_new_io_context()
{
	PMS_IO_CONTEXT context = (MS_IO_CONTEXT*)moon_malloc(sizeof(MS_IO_CONTEXT));
	memset(&context->m_Overlapped,0, sizeof(context->m_Overlapped));
	memset(context->m_szBuffer,0,MAX_BUFFER_LEN );
	context->m_sockAccept = INVALID_SOCKET;
	context->m_wsaBuf.buf = context->m_szBuffer;
	context->m_wsaBuf.len = MAX_BUFFER_LEN;
	context->m_OpType     = NULL_POSTED;
	return context;
}

/*释放IO上下文*/
void free_io_context(PMS_IO_CONTEXT context)
{
	moon_free(context);
}

/*创建一个新的socket上下文*/
PMS_SOCKET_CONTEXT create_new_socket_context()
{
	PMS_SOCKET_CONTEXT context = (PMS_SOCKET_CONTEXT)moon_malloc(sizeof(MS_SOCKET_CONTEXT));
	InitializeCriticalSection(&context->SockCritSec);
	context->m_pListIOContext = array_list_init();
	return context;
}

/*释放socket上下文*/
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