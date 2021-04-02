#include <errno.h>

#include "wsv_wrapper.h"
#include "wsv_retval.h"
#include "wsv_http_message.h"
#include "wsv_client.h"

int
wsv_http_response_write(int sockfd, struct wsv_http_message_s* m)
{
	enum states
	{
		status_line_headers = 0,
		body_send
	};
	ssize_t			bytes_sent;

	switch ((enum states)m->state_response)
	{
	case status_line_headers:
		bytes_sent = wsv_send(sockfd,
							  (void*)m->buf_begin,
							  (m->buf_end - m->buf_begin),
							  MSG_DONTWAIT);
		if (bytes_sent > 0)
		{

#ifdef WSV_LOG
			write(STDOUT_FILENO, "\n==== Response: ====\n", 21);
			write(STDOUT_FILENO, m->buf_begin, m->buf_end - m->buf_begin);
			write(STDOUT_FILENO, "===================\n", 20);
#endif

			if (m->ressource_fd == -1)
				return (WSV_OK);
			m->state_response = body_send;
			m->read_static_content_handler = &wsv_http_response_ressource_read;
			wsv_client_static_content_read_intialize(m->ressource_fd, (void*)m);
			return (WSV_RETRY);
		}
		else if (bytes_sent == -1 &&
				 (errno == EAGAIN || errno == EWOULDBLOCK))
			return (WSV_RETRY);
		else
		{
			if (m->ressource_fd >= 0)
				wsv_close(m->ressource_fd); /* insert error check */
			return (WSV_ERROR);
		}

	case body_send:
		bytes_sent = wsv_send(sockfd,
							  (void*)m->buf_begin,
							  (m->buf_end - m->buf_begin),
							  MSG_DONTWAIT);
		if (bytes_sent > 0)
		{
			wsv_client_static_content_read_intialize(m->ressource_fd, (void*)m);
			return (WSV_RETRY);
		}
		else if (bytes_sent == -1 &&
				 (errno == EAGAIN || errno == EWOULDBLOCK))
			return (WSV_RETRY);
		else
		{
			wsv_close(m->ressource_fd); /* insert error check */
			return (WSV_ERROR);
		}
	}

	return (WSV_ERROR);
}
