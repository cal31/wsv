#include "wsv_retval.h"
#include "wsv_http_message.h"
#include "wsv_string.h"

static int
wsv_http_field_name(char* first, char* last, int* index)
{
	int							retval;

	*last = '\0';

	retval = wsv_http_header(first, index);

	*last = ':';

	return (retval);
}

int
wsv_http_header_field_load(struct wsv_http_message_s* m)
{
	int							index;
	int							len;
	int							flag;

	if (wsv_http_field_name(m->header_name_begin,
							m->header_name_end,
							&index) == WSV_OK)
	{
		flag = (1 << index);
		if (m->headers_flag & flag)
			return (WSV_ERROR_400); /* duplicate header */
		else
			m->headers_flag += flag;

		len = 0;
		while ((m->header_value_begin != m->header_value_end) &&
			   (len != HEADER_VALUE_SIZE))
		{
			m->headers[index][len] = *m->header_value_begin;
			++m->header_value_begin;
			++len;
		}
		if (len == HEADER_VALUE_SIZE)
			return (WSV_ERROR_400); /* header too large */
		else
			m->headers[index][len] = '\0';
	}

	return (WSV_OK);
}
