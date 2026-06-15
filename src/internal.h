#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <limits.h>

#define da_append(xs, x)\
	do{\
		if (xs.len >= xs.cap) {\
			if (xs.cap == 0) xs.cap = 256;\
			else xs.cap *= 2;\
			xs.items = realloc(xs.items, xs.cap * sizeof(*xs.items));\
		}\
		xs.items[xs.len++] = x;\
	} while (0)

typedef struct Route Route;
typedef struct StaticMount StaticMount;

typedef struct {
	Route* items;
	size_t len;
	size_t cap;
} Routes;

typedef struct {
	StaticMount* items;
	size_t len;
	size_t cap;
} StaticMounts;

typedef enum { UNKNOWN, GET, POST, HEAD } HttpMethod;

typedef struct Header {
	char* name;
	char* value;
} Header;

typedef struct {
	Header* items;
	size_t len;
	size_t cap;
} Headers;

typedef Header QueryParam;
typedef Headers QueryParams;

typedef struct RevrRequest {
	HttpMethod method;
	char path[PATH_MAX];
	char* version;
	Headers headers;
	QueryParams queries;
} RevrRequest;

typedef struct RevrResponse {
	int status_code;
	const char* content_type;
	size_t content_length;
	Headers headers;

	const void* body;
	bool owns_body;
} RevrResponse;

typedef struct RevrApp {
	Routes routes;
	StaticMounts static_mounts;
} RevrApp;

