#include "revr/revr.h"
#include "fs.h"
#include "http.h"
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

RevrStatus revr_static(RevrApp *app, const char *path, const char *directory) {
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
RevrStatus revr_send_text(RevrResponse *res, int status, const char *text) {
	res->status_code = status;
	res->body = strdup(text);
	res->owns_body = true;
	res->content_length = strlen(text);
	res->content_type = "text/plain";
	return REVR_OK;
}

RevrStatus revr_send_html(RevrResponse *res, int status, const char *html) {
	res->status_code = status;
	res->body = strdup(html);
	res->owns_body = true;
	res->content_length = strlen(html);
	res->content_type = "text/html";
	return REVR_OK;
}

RevrStatus revr_send_file(RevrResponse *res, int status, const char *path) {
	char fullpath[PATH_MAX];

	if (realpath(path, fullpath) == NULL) {
		return REVR_ERR_NOT_FOUND;
	}

	file_content content;

	if (fs_getpath(fullpath, &content) != 0) {
		return REVR_ERR_NOT_FOUND;
	}

	res->status_code = status;
	res->content_type = content.content_type;
	res->content_length = content.content_length;
	res->body = content.content;
	res->owns_body = true;

	return REVR_OK;
}

void revr_send_ok(RevrResponse *res) { http_ok(res); }
void revr_send_created(RevrResponse *res) { http_created(res); }
void revr_send_no_content(RevrResponse *res) { http_no_content(res); }
void revr_send_bad_request(RevrResponse *res) { http_bad_request(res); }
void revr_send_unauthorized(RevrResponse *res) { http_unauthorized(res); }
void revr_send_forbidden(RevrResponse *res) { http_forbidden(res); }
void revr_send_not_found(RevrResponse *res) { http_not_found(res); }
void revr_send_method_not_allowed(RevrResponse *res) {
	http_method_not_allowed(res);
}
void revr_send_conflict(RevrResponse *res) { http_conflict(res); }
void revr_send_internal_error(RevrResponse *res) { http_internal_error(res); }
void revr_send_not_implemented(RevrResponse *res) { http_not_implemented(res); }
void revr_send_bad_gateway(RevrResponse *res) { http_bad_gateway(res); }
void revr_send_service_unavailable(RevrResponse *res) {
	http_service_unavailable(res);
}

const char *revr_req_header(RevrRequest *req, const char *name) {
	for (int i = 0; i < req->headers.len; i++) {
		if (strcmp(name, req->headers.items[i].name) == 0)
			return req->headers.items[i].value;
	}
	return NULL;
}

void revr_res_header(RevrResponse *res, const char *name, const char *value) {
	for (int i = 0; i < res->headers.len; i++) {
		if (strcmp(name, res->headers.items[i].name) == 0) {
			free(res->headers.items[i].value);
			res->headers.items[i].value = strdup(value);
			return;
		}
	}

	Header header = {
	    .name = strdup(name),
	    .value = strdup(value),
	};

	da_append(res->headers, header);
}
