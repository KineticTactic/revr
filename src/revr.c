#include "revr/revr.h"
#include "route.h"
#include "server.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define BACKLOG 10

RevrApp *revr_app_create() {
	RevrApp *app = malloc(sizeof(RevrApp));
	route_init(app);
	return app;
}

void revr_app_free(RevrApp *app) {
	route_free(app);
	free(app);
}

void revr_listen(RevrApp *app, const char *port) {
	struct addrinfo *servinfo = server_get_addr_info(port);
	int sockfd = server_create_and_bind_socket(servinfo);
	server_listen(sockfd, BACKLOG);
	while (1) {
		server_accept_connection(app, sockfd);
	}
}

void revr_get(RevrApp *app, const char *path, RevrHandler handler) {
	route_add(app, GET, path, handler);
}

int revr_static(RevrApp *app, const char *path, const char *directory) {
	char resolved[PATH_MAX];

	if (realpath(directory, resolved) == NULL)
		return REVR_ERR_NOT_FOUND;

	struct stat st;

	if (stat(resolved, &st) == -1)
		return REVR_ERR_NOT_FOUND;

	if (!S_ISDIR(st.st_mode))
		return REVR_ERR_NOT_DIRECTORY;

	route_static_mount(app, path, resolved);
	return REVR_OK;
}

// TODO: Think about string ownership
void revr_send_text(RevrResponse *res, int status, const char *text) {
	res->status_code = status;
	res->body = text;
	res->owns_body = false;
	res->content_length = strlen(text);
	res->content_type = "text/plain";
}
