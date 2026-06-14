#pragma once

#include "revr/revr.h"
struct addrinfo;

struct addrinfo *server_get_addr_info(const char *PORT);
void server_free_addr_info(struct addrinfo* servinfo);
int server_create_and_bind_socket(struct addrinfo *servinfo);
void server_listen(int sockfd, int backlog);
void server_accept_connection(RevrApp* app, int sockfd);
