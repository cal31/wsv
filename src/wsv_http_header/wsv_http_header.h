#ifndef WSV_HTTP_HEADER_H
# define WSV_HTTP_HEADER_H

# define HEADERS_MAX				3
# define HEADER_VALUE_SIZE			512
# define HEADERS_BUCKETS_SIZE		11 /* prime number */

# define TRANSFER_ENCODING			0
# define CONTENT_LENGTH				1
# define HOST						2

int		wsv_http_headers_initialize(void);
int		wsv_http_header(const char* header, int* index);

#endif
