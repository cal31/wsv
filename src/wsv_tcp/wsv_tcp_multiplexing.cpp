#include "wsv_wrapper.h"
#include "wsv_retval.h"
#include "wsv_client.h"
#include "wsv_configuration.h"

struct			wsv_multiplexing_s
{
	fd_set		read_set;
	fd_set		write_set;
	fd_set		static_content_set;

	fd_set		maxfd_set;
	int			maxfd;
};

void
wsv_tcp_multiplexing_read(int fd, void* fd_sets)
{
	FD_SET(fd, &(((struct wsv_multiplexing_s*)fd_sets)->read_set));
	FD_CLR(fd, &(((struct wsv_multiplexing_s*)fd_sets)->write_set));

	if (fd > ((struct wsv_multiplexing_s*)fd_sets)->maxfd)
		((struct wsv_multiplexing_s*)fd_sets)->maxfd = fd;
	FD_SET(fd, &(((struct wsv_multiplexing_s*)fd_sets)->maxfd_set));
}

void
wsv_tcp_multiplexing_write(int fd, void* fd_sets)
{
	FD_CLR(fd, &(((struct wsv_multiplexing_s*)fd_sets)->read_set));
	FD_SET(fd, &(((struct wsv_multiplexing_s*)fd_sets)->write_set));

	if (fd > ((struct wsv_multiplexing_s*)fd_sets)->maxfd)
		((struct wsv_multiplexing_s*)fd_sets)->maxfd = fd;
	FD_SET(fd, &(((struct wsv_multiplexing_s*)fd_sets)->maxfd_set));
}

void
wsv_tcp_multiplexing_static_content_read(int fd, void* fd_sets)
{
	FD_SET(fd, &(((struct wsv_multiplexing_s*)fd_sets)->read_set));
	FD_SET(fd, &(((struct wsv_multiplexing_s*)fd_sets)->static_content_set));

	if (fd > ((struct wsv_multiplexing_s*)fd_sets)->maxfd)
		((struct wsv_multiplexing_s*)fd_sets)->maxfd = fd;
	FD_SET(fd, &(((struct wsv_multiplexing_s*)fd_sets)->maxfd_set));
}

void
wsv_tcp_multiplexing_static_content_write(int fd, void* fd_sets)
{
	FD_SET(fd, &(((struct wsv_multiplexing_s*)fd_sets)->write_set));
	FD_SET(fd, &(((struct wsv_multiplexing_s*)fd_sets)->static_content_set));

	if (fd > ((struct wsv_multiplexing_s*)fd_sets)->maxfd)
		((struct wsv_multiplexing_s*)fd_sets)->maxfd = fd;
	FD_SET(fd, &(((struct wsv_multiplexing_s*)fd_sets)->maxfd_set));
}

void
wsv_tcp_multiplexing_clear(int fd, void* fd_sets)
{
	struct wsv_multiplexing_s*	fdsets;

	fdsets = (struct wsv_multiplexing_s*)fd_sets;

	FD_CLR(fd, &(fdsets->read_set));
	FD_CLR(fd, &(fdsets->write_set));
	FD_CLR(fd, &(fdsets->static_content_set));

	FD_CLR(fd, &(fdsets->maxfd_set));

	if (fdsets->maxfd == fd)
	{
		while (fd--)
		{
			if (FD_ISSET(fd, &(fdsets->maxfd_set)))
			{
				fdsets->maxfd = fd;
				break;
			}
		}
	}
}

/*
** NOTES:
** +) dependency: new file descriptor is the lowest-numbered fd available,
**	meaning that every client c_sockfd is greater then s_listen_sockfd
** +) static/regular files are always considered ready, select is mostly
**	useful for sockets/pipes (on windows select is only for sockets)
** +) considering the case above sendfile (linux-specific) for example
**	could or couldn't provide an increase in performance
** +) if available I would rather prefer to use standard specific
** functions (iso c, berkerly/posix sockets...)
*/
int
wsv_tcp_multiplexing(void* clients, const void* configuration)
{
	struct wsv_multiplexing_s	fd_sets;
	fd_set						to_read_set;
	fd_set						to_write_set;
	int							maxfd;
	int							minfd;
	int							nready;
	int							fd;

	const struct wsv_server_s*	srv;
	int							n_server;
	int							n_client;


	FD_ZERO(&fd_sets.read_set);
	FD_ZERO(&fd_sets.write_set);
	FD_ZERO(&fd_sets.static_content_set);
	FD_ZERO(&fd_sets.maxfd_set);
	fd_sets.maxfd = 0;
	minfd = 4;
	maxfd = 4;

	srv = ((const struct wsv_configuration_s*)configuration)->servers;
	n_server = ((const struct wsv_configuration_s*)configuration)->n_server;
	while (n_server--)
	{
		wsv_tcp_multiplexing_read(srv->listen_sockfd, (void*)&fd_sets);
		minfd = srv->listen_sockfd + 1;
		++srv;
	}
	n_client = 0;

	for (;;)
	{
		to_read_set = fd_sets.read_set;
		to_write_set = fd_sets.write_set;
		maxfd = fd_sets.maxfd;

		nready = wsv_select(maxfd + 1, &to_read_set, &to_write_set, 0, 0);
		if (nready == -1)
			return (WSV_ERROR);

		srv = ((const struct wsv_configuration_s*)configuration)->servers;
		n_server = ((const struct wsv_configuration_s*)configuration)->n_server;
		while (n_server--)
		{
			if (srv->duplicate == 0 &&
				FD_ISSET(srv->listen_sockfd, &to_read_set))
			{
				n_client = wsv_client_insert(clients,
											 n_client,
											 configuration,
											 (const void*)srv,
											 (void*)&fd_sets);
				--nready;
			}
			++srv;
		}

		for (fd = minfd; fd <= maxfd && nready != 0; ++fd)
		{
			if (FD_ISSET(fd, &to_read_set))
			{
				if (FD_ISSET(fd, &(fd_sets.static_content_set)))
					n_client = wsv_client_static_content_read(fd, n_client);
				else
					n_client = wsv_client_read(fd, clients, n_client);
				--nready;
			}
		}

		for (fd = minfd; fd <= maxfd && nready != 0; ++fd)
		{
			if (FD_ISSET(fd, &to_write_set))
			{
				if (FD_ISSET(fd, &(fd_sets.static_content_set)))
					n_client = wsv_client_static_content_write(fd, n_client);
				else
					n_client = wsv_client_write(fd, clients, n_client);
				--nready;
			}
		}
	}
}
