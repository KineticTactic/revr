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

	char *request_line = strtok_r(raw, "\r\n", &line_ptr);

	char *method = strtok_r(request_line, " ", &field_ptr);
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

	return 0;
}

char *http_generate_response(const RevrResponse *response) {
	const char *reason = response->status_code == 200 ? "OK" : response->body;

	size_t capacity = sizeof(char) * 512;
	char *response_str = malloc(capacity + 1);
	snprintf(response_str, capacity + 1,
	         "HTTP/1.0 %d %s\r\n"
	         "Content-Type: %s\r\n"
	         "Content-Length: %zu\r\n"
	         "\r\n",
	         response->status_code, reason, response->content_type,
	         response->content_length);

	return response_str;
}

void http_send_file(RevrResponse *res, file_content *content) {
	res->status_code = 200;
	res->body = content->content;
	res->content_type = content->content_type;
	res->content_length = content->content_length;
	res->owns_body = true;
}

void http_404(RevrResponse *res) {
	res->status_code = 404;
	res->body = "Not Found";
	res->owns_body = false;
	res->content_type = "text/plain";
	res->content_length = strlen(res->body);
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
