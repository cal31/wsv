#ifndef WSV_CLIENT_H
# define WSV_CLIENT_H

void	wsv_clients_terminate(void* clients);

int		wsv_client_insert(void* clients,
						  int n_client,
						  const void* configuration,
						  const void* server,
						  void* fd_sets);

/* ---- I/O [initialisation] ---- */
void	wsv_client_static_content_read_intialize(int fd, void* client);
void	wsv_client_static_content_write_intialize(int fd, void* client);

void	wsv_client_read_initialize(int fd, void* client);
void	wsv_client_write_initialize(int fd, void* client);
/* ---------------------- */

/* ---- I/O [initialisation] ---- */
int		wsv_client_read(int c_sockfd, void* clients, int n_client);
int		wsv_client_write(int c_sockfd, void* clients, int n_client);

int		wsv_client_static_content_read(int fd, int n_client);
int		wsv_client_static_content_write(int fd, int n_client);
/* -------------------------- */

#endif
