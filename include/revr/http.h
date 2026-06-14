#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <limits.h>

typedef enum { UNKNOWN, GET, POST, HEAD } RevrHttpMethod;
typedef struct {
	RevrHttpMethod method;
	char path[PATH_MAX];
	char* version;
} RevrHttpRequest;

typedef struct {
	int status_code;
	const char* content_type;
	size_t content_length;
	void* body;

	bool owns_body;
} RevrHttpResponse;

int http_parse_request(char* req, RevrHttpRequest* request);
char* http_generate_response(const RevrHttpResponse* response);
