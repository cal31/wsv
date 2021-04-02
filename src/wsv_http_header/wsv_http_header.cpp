#include "wsv_http_header.h"
#include "wsv_retval.h"
#include "wsv_hash.h"

static const int				headers_index[HEADERS_MAX] =
{
	TRANSFER_ENCODING,
	CONTENT_LENGTH,
	HOST
};

static struct wsv_pair_s		g_wsv_headers[HEADERS_MAX] =
{
	{ (const void*)"transfer-encoding",	(const void*)(headers_index + 0) },
	{ (const void*)"content-length",	(const void*)(headers_index + 1) },
	{ (const void*)"host",				(const void*)(headers_index + 2) }
};

static struct wsv_pair_s		g_wsv_headers_buckets[HEADERS_BUCKETS_SIZE];

int
wsv_http_headers_initialize(void)
{
	return (wsv_hash_str_initialize(g_wsv_headers_buckets,
									HEADERS_BUCKETS_SIZE,
									g_wsv_headers,
									HEADERS_MAX));
}

int
wsv_http_header(const char* header, int* index)
{
	const int*		value;

	if (wsv_hash_str_find(g_wsv_headers_buckets,
						  HEADERS_BUCKETS_SIZE,
						  (const void*)header,
						  (const void**)&value) == WSV_OK)
	{
		*index = *value;
		return (WSV_OK);
	}
	else
		return (WSV_ERROR);
}
