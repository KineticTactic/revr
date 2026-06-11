#pragma once

struct addrinfo;

typedef struct {
	int sockfd;
	const char* rootdir;
} server;

struct addrinfo *server_get_addr_info(const char *PORT);
void server_free_addr_info(struct addrinfo* servinfo);
void server_create_and_bind_socket(struct addrinfo *servinfo, server* server);
void server_listen(server* server, int backlog);
void server_accept_connection(server* server);
