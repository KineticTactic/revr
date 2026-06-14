#include "uri.h"

#include <stdbool.h>
#include <string.h>

// Source - https://stackoverflow.com/a/4770992
// Posted by Fred Foo, modified by community. See post 'Timeline' for change
// history Retrieved 2026-06-11, License - CC BY-SA 3.0
bool prefix(const char *pre, const char *str) {
	return strncmp(pre, str, strlen(pre)) == 0;
}

int hex_to_int(char c) {
	if ('0' <= c && c <= '9')
		return c - '0';

	if ('A' <= c && c <= 'F')
		return c - 'A' + 10;

	if ('a' <= c && c <= 'f')
		return c - 'a' + 10;

	return -1;
}

char hex_to_char(char *hex) {
	int hi = hex_to_int(hex[0]);
	int lo = hex_to_int(hex[1]);

	if (hi < 0 || lo < 0)
		return 0;

	return (char)((hi << 4) | lo);
}

void uri_decode(char *uri) {
	char *percent;
	while ((percent = strchr(uri, '%')) != NULL) {
		char decoded = hex_to_char(percent + 1);
		*percent = decoded;
		memmove(percent + 1, percent + 3, strlen(percent + 3) + 1);
	}
}

void remove_last_segment(char *path) {
	char *last_seg = strrchr(path, '/');
	if (last_seg == NULL) {
		*path = '\0';
	} else {
		*last_seg = '\0';
	}
}

// Implements https://datatracker.ietf.org/doc/html/rfc3986#section-5.2.4
void remove_dot_segments(char *in, char *out) {
	*out = '\0';
	while (*in != '\0') {
		if (prefix("../", in)) {
			in += 3;
		} else if (prefix("./", in)) {
			in += 2;
		} else if (prefix("/./", in)) {
			in += 2;
		} else if (prefix("/.", in) && in[2] == '\0') {
			in += 1;
			*in = '/';
		} else if (prefix("/../", in)) {
			in += 3;
			remove_last_segment(out);
		} else if (prefix("/..", in) && in[3] == '\0') {
			in += 2;
			*in = '/';
			remove_last_segment(out);
		} else if (strcmp("..", in) == 0 || strcmp(".", in) == 0) {
			*in = '\0';
		} else {
			if (*in == '/') {
				strcat(out, "/");
				in++;
			}
			char *slash = strchr(in, '/');
			if (slash == NULL) {
				strcat(out, in);
				*in = '\0';
			} else {
				*slash = '\0';
				strcat(out, in);
				*slash = '/';
				in = slash;
			}
		}
	}
}
