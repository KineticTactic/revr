#include "route.h"

#include "fs.h"
#include "http.h"
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void route_init(RevrApp *app) { app->routes = (Routes){0}; }
void route_free(RevrApp *app) {
	for (size_t i = 0; i < app->routes.len; i++) {
		free(app->routes.items[i].path);
	}
	for (size_t i = 0; i < app->static_mounts.len; i++) {
		free(app->static_mounts.items[i].directory);
		free(app->static_mounts.items[i].mount_path);
	}
	free(app->routes.items);
	free(app->static_mounts.items);
}

void route_add(RevrApp *app, HttpMethod method, const char *path,
               RevrHandler handler) {
	Route route = {
	    .method = method,
	    .path = strdup(path),
	    .handler = handler,
	};

	da_append(app->routes, route);
}

void route_static_mount(RevrApp *app, const char *mount_path,
                        const char *directory) {
	StaticMount mount = {
	    .mount_path = strdup(mount_path),
	    .directory = strdup(directory),
	};

	da_append(app->static_mounts, mount);
}

// TODO: Different method on implemented path gives Not implemented
void route_dispatch(RevrApp *app, RevrRequest *req, RevrResponse *res) {
	Route *route = NULL;
	for (int i = 0; i < app->routes.len; i++) {
		if (strcmp(req->path, app->routes.items[i].path) == 0 &&
		    req->method == app->routes.items[i].method) {
			route = &app->routes.items[i];
			break;
		}
	}

	if (route != NULL) {
		return route->handler(req, res);
	}

	if (req->method == UNKNOWN) {
		return http_not_implemented(res);
	}

	// TODO: Document ownership of file_content a bit.
	for (int i = 0; i < app->static_mounts.len; i++) {
		file_content content;
		char fullpath[PATH_MAX];
		snprintf(fullpath, PATH_MAX, "%s%s",
		         app->static_mounts.items[i].directory, req->path);

		if (fs_getpath(fullpath, &content) != 0)
			continue;
		if (req->method != GET || req->method != HEAD)
			return http_method_not_allowed(res);

		return http_send_file(res, &content);
	}

	return http_not_found(res);
}
