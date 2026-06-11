#include "revr/server.h"
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

#define PORT "3490"
#define BACKLOG 10

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Usage: http-c <root-directory>\n");
		return 1;
	}

	DIR *rootdir;
	if ((rootdir = opendir(argv[1])) == NULL) {
		fprintf(stderr, "Failed to open root directory at %s", argv[1]);
		return 1;
	}
	struct dirent *dp;
	while ((dp = readdir(rootdir)) != 0) {
		if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
			continue;
		printf("Name: %s/%s\n", argv[1], dp->d_name);
	}

	struct addrinfo *servinfo = server_get_addr_info(PORT);

	char resolved_path[PATH_MAX];
	realpath(argv[1], resolved_path);
	printf("ROOT DIR: %s\n", resolved_path);

	server server = {
		.rootdir = resolved_path
	};
	server_create_and_bind_socket(servinfo, &server);
	server_free_addr_info(servinfo);
	server_listen(&server, BACKLOG);

	while (1) {
		server_accept_connection(&server);
	}

	return 0;
}
