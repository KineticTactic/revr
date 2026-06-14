#include "fs.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int fs_getpath(const char *path, file_content *ret) {
	struct stat st;
	if (stat(path, &st) == -1) {
		perror("stat");
		return -1;
	}
	if (S_ISDIR(st.st_mode)) {
		char index_path[PATH_MAX];

		snprintf(index_path, sizeof(index_path), "%s/index.html", path);
		printf("INDEX PATH: %s\n", index_path);

		if (stat(index_path, &st) != 0 || !S_ISREG(st.st_mode))
			return -1;

		if (fs_loadfile(index_path, ret) != 0)
			return -1;
		return 0;
	}

	if (fs_loadfile(path, ret) != 0)
		return -1;
	return 0;
}

int fs_loadfile(const char *path, file_content *ret) {
	struct stat st;

	if (stat(path, &st) == -1)
		return -1;

	FILE *fp = fopen(path, "rb");
	if (!fp)
		return -1;

	void *buffer = malloc(st.st_size);
	if (!buffer) {
		fclose(fp);
		return -1;
	}

	size_t bytes_read = fread(buffer, 1, st.st_size, fp);

	fclose(fp);

	if (bytes_read != (size_t)st.st_size) {
		free(buffer);
		return -1;
	}

	ret->content = buffer;
	ret->content_length = bytes_read;
	ret->content_type = fs_content_type(path);

	return 0;
}

const char *fs_content_type(const char *path) {
	const char *ext = strrchr(path, '.');

	if (!ext)
		return "application/octet-stream";

	if (strcmp(ext, ".html") == 0)
		return "text/html";

	if (strcmp(ext, ".css") == 0)
		return "text/css";

	if (strcmp(ext, ".js") == 0)
		return "application/javascript";

	if (strcmp(ext, ".json") == 0)
		return "application/json";

	if (strcmp(ext, ".png") == 0)
		return "image/png";

	if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0)
		return "image/jpeg";

	if (strcmp(ext, ".gif") == 0)
		return "image/gif";

	if (strcmp(ext, ".svg") == 0)
		return "image/svg+xml";

	if (strcmp(ext, ".txt") == 0)
		return "text/plain";

	return "application/octet-stream";
}
