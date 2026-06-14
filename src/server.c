#include "server.h"
#include "fs.h"
#include "http.h"
#include "internal.h"
#include "route.h"

#include <arpa/inet.h>
#include <limits.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

struct addrinfo *server_get_addr_info(const char *PORT) {
	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo;

	// Fill up the hints struct to send to getaddrinfo
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;     // either ipv4 or v6
	hints.ai_socktype = SOCK_STREAM; // we are using stream sockets
	hints.ai_flags = AI_PASSIVE;

	// The call to getaddrinfo fills up the servinfo struct.
	// the NULL signifies local machine
	if ((status = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "gai error: %s\n", gai_strerror(status));
		exit(1);
	}

	return servinfo;
}

void server_free_addr_info(struct addrinfo *servinfo) {
	freeaddrinfo(servinfo);
}

int server_create_and_bind_socket(struct addrinfo *servinfo) {
	// loop through all the results and bind to the first one which we can
	int sockfd;
	struct addrinfo *p;
	for (p = servinfo; p != NULL; p = p->ai_next) {
		// Create a socket of the required family (ipv4/6), type (STREAM vs
		// DGRAM) and protocol (most commonly 0 for default)
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) ==
		    -1) {
			perror("server: socket");
			continue;
		}

		// Immediately after server stops, the port is still in use according to
		// a TCP state called TIME_WAIT. This line tells it to reuse the port.
		int yes = 1;
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) ==
		    -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server:bind");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "server: failed to bind!\n");
		exit(1);
	}

	return sockfd;
}

void server_listen(int sockfd, int backlog) {
	// Start listening. Accept at most BACKLOG connections on this socket.
	if (listen(sockfd, backlog) == -1) {
		perror("listen");
		exit(1);
	}
	printf("Listening...\n");
}

void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET)
		return &(((struct sockaddr_in *)sa)->sin_addr);

	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

// int server_handle_request(const RevrHttpRequest *req, RevrHttpResponse *res) {
// 	if (req->method != GET) {
// 		res->status_code = 501;
// 		res->body = "Not Implemented";
// 		res->content_type = "text/plain";
// 		res->content_length = strlen(res->body);
// 		res->owns_body = false;
// 		return 0;
// 	}
//
// 	file_content content;
// 	char fullpath[PATH_MAX];
// 	snprintf(fullpath, PATH_MAX, "%s%s", server->rootdir, req->path);
//
// 	if (fs_getpath(fullpath, &content) != 0) {
// 		res->status_code = 404;
// 		res->body = "Not Found";
// 		res->content_type = "text/plain";
// 		res->content_length = strlen(res->body);
// 		res->owns_body = false;
// 		return 0;
// 	}
//
// 	res->status_code = 200;
// 	res->body = content.content;
// 	res->content_type = content.content_type;
// 	res->content_length = content.content_length;
// 	res->owns_body = true;
//
// 	return 0;
// }

void server_accept_connection(RevrApp* app, int sockfd) {
	struct sockaddr_storage their_addr;
	socklen_t addr_size = sizeof their_addr;
	int new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);

	if (new_fd == -1) {
		perror("accept");
		return;
	}

	char s[INET6_ADDRSTRLEN];
	inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr),
	          s, sizeof s);
	printf("server: got connection from %s\n", s);

	if (!fork()) {
		close(sockfd);

		// Receive message
		char request_str[1024];
		recv(new_fd, request_str, 1024, 0);
		printf("Received: \n%s\n", request_str);

		RevrRequest req;
		int status = http_parse_request(request_str, &req);

		RevrResponse res;
		if (status == -1) {
			res.status_code = 400;
			res.content_type = "text/plain";
			res.body = "Bad Request";
			res.content_length = strlen(res.body);
			res.owns_body = false;
		} else {
			route_dispatch(app, &req, &res);
		}

		char *response_str = http_generate_response(&res);

		printf("sending response: \n%s\n", response_str);

		int len = strlen(response_str);
		int bytes_sent = send(new_fd, response_str, len, 0);

		size_t total = 0;
		while (total < res.content_length) {
			ssize_t n = send(new_fd, (char *)res.body + total,
			                 res.content_length - total, 0);

			if (n <= 0)
				break;

			total += n;
		}

		free(response_str);

		// TODO: THink about this.
		if (res.owns_body) {
			free((void*) res.body);
		}

		if (bytes_sent == -1)
			perror("send");
		close(new_fd);
		exit(0);
	}

	close(new_fd);
}
