#pragma once

#include <stddef.h>

typedef struct file_content {
	void* content;
	const char* content_type;
	size_t content_length;
} file_content;

int fs_getpath(const char* path, file_content* ret);
int fs_loadfile(const char* path, file_content* ret);
const char* fs_content_type(const char* path);
