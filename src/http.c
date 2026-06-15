#include "http.h"

#include "fs.h"
#include "internal.h"
#include "uri.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int http_parse_method(const char *method) {
	if (strcmp(method, "GET") == 0) {
		return GET;
	} else if (strcmp(method, "POST") == 0) {
		return POST;
	} else if (strcmp(method, "HEAD") == 0) {
		return HEAD;
	}
	return UNKNOWN;
}

int http_parse_request(char *raw, RevrRequest *request) {
	char *line_ptr;
	char *field_ptr;

	char *body = strstr(raw, "\r\n\r\n");
	*body = '\0';
	body += 4;

	char *line = strtok_r(raw, "\r\n", &line_ptr);

	char *method = strtok_r(line, " ", &field_ptr);
	char *path = strtok_r(NULL, " ", &field_ptr);
	char *version = strtok_r(NULL, " ", &field_ptr);

	if (method == NULL || path == NULL || version == NULL ||
	    (strcmp(version, "HTTP/1.0") != 0 &&
	     strcmp(version, "HTTP/1.1") != 0)) {
		return -1;
	}

	char *query = strchr(path, '?');
	if (query) {
		*query = '\0';
		query++;
	}

	request->method = http_parse_method(method);
	uri_decode(path);
	remove_dot_segments(path, request->path);
	request->version = version;

	printf("Method: %s\n", method);
	printf("Path: %s\n", request->path);
	printf("Query: %s\n", query);
	printf("Version: %s\n", version);

	// Parse headers
	printf("Header start\n");
	while ((line = strtok_r(NULL, "\r\n", &line_ptr)) != NULL) {
		char *colon = strchr(line, ':');

		if (!colon)
			return -1;

		*colon = '\0';

		char *name = line;
		char *value = colon + 1;

		while (*value == ' ' || *value == '\t') {
			value++;
		}

		Header header = {
		    .name = name,
		    .value = value,
		};

		printf("%s, %s\n", name, value);
		da_append(request->headers, header);
	}
	printf("Header end\n");

	return 0;
}

void http_free_req(RevrRequest *req) { free(req->headers.items); }

void http_free_res(RevrResponse *res) {
	for (size_t i = 0; i < res->headers.len; i++) {
		free(res->headers.items[i].name);
		free(res->headers.items[i].value);
	}

	free(res->headers.items);

	if (res->owns_body) {
		free((void *)res->body);
	}
}

char *http_generate_response(const RevrResponse *response) {
	const char *reason = response->status_code == 200 ? "OK" : response->body;

	size_t capacity = 256;

	for (size_t i = 0; i < response->headers.len; i++) {
		capacity += strlen(response->headers.items[i].name);
		capacity += strlen(response->headers.items[i].value);
		capacity += 4; // ": " + "\r\n"
	}
	char *response_str = malloc(capacity + 1);
	char *p = response_str;

	p += sprintf(p,
	             "HTTP/1.0 %d %s\r\n"
	             "Content-Type: %s\r\n"
	             "Content-Length: %zu\r\n",
	             response->status_code, reason, response->content_type,
	             response->content_length);

	for (size_t i = 0; i < response->headers.len; i++) {
		p += sprintf(p, "%s: %s\r\n", response->headers.items[i].name,
		             response->headers.items[i].value);
	}
	sprintf(p, "\r\n");

	return response_str;
}

void http_send_file(RevrResponse *res, file_content *content) {
	res->status_code = 200;
	res->body = content->content;
	res->content_type = content->content_type;
	res->content_length = content->content_length;
	res->owns_body = true;
}

void http_ok(RevrResponse *res) {
	res->status_code = 200;
	res->body = "OK";
	res->owns_body = false;
	res->content_type = "text/plain";
	res->content_length = 2;
}
void http_created(RevrResponse *res) {
	res->status_code = 201;
	res->body = "Created";
	res->owns_body = false;
	res->content_type = "text/plain";
	res->content_length = 7;
}
void http_no_content(RevrResponse *res) {
	res->status_code = 204;
	res->body = "";
	res->owns_body = false;
	res->content_type = "text/plain";
	res->content_length = 0;
}
void http_bad_request(RevrResponse *res) {
	res->status_code = 400;
	res->body = "Bad Request";
	res->owns_body = false;
	res->content_type = "text/plain";
	res->content_length = 11;
}
void http_unauthorized(RevrResponse *res) {
	res->status_code = 401;
	res->body = "Unauthorized";
	res->owns_body = false;
	res->content_type = "text/plain";
	res->content_length = 12;
}
void http_forbidden(RevrResponse *res) {
	res->status_code = 403;
	res->body = "Forbidden";
	res->owns_body = false;
	res->content_type = "text/plain";
	res->content_length = 9;
}
void http_not_found(RevrResponse *res) {
	res->status_code = 404;
	res->body = "Not Found";
	res->owns_body = false;
	res->content_type = "text/plain";
	res->content_length = 9;
}
void http_conflict(RevrResponse *res) {
	res->status_code = 409;
	res->body = "Conflict";
	res->owns_body = false;
	res->content_type = "text/plain";
	res->content_length = 8;
}
void http_internal_error(RevrResponse *res) {
	res->status_code = 500;
	res->body = "Internal Server Error";
	res->owns_body = false;
	res->content_type = "text/plain";
	res->content_length = 22;
}
void http_bad_gateway(RevrResponse *res) {
	res->status_code = 502;
	res->body = "Bad Gateway";
	res->owns_body = false;
	res->content_type = "text/plain";
	res->content_length = 11;
}
void http_service_unavailable(RevrResponse *res) {
	res->status_code = 503;
	res->body = "Service Unavailable";
	res->owns_body = false;
	res->content_type = "text/plain";
	res->content_length = 20;
}
void http_method_not_allowed(RevrResponse *res) {
	res->status_code = 405;
	res->body = "Method Not Allowed";
	res->owns_body = false;
	res->content_type = "text/plain";
	res->content_length = strlen(res->body);
}
void http_not_implemented(RevrResponse *res) {
	res->status_code = 501;
	res->body = "Not Implemented";
	res->owns_body = false;
	res->content_type = "text/plain";
	res->content_length = strlen(res->body);
}
