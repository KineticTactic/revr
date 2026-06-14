#pragma once

#include "internal.h"
#include "revr/revr.h"

typedef struct Route {
	char *path;
	HttpMethod method;
	RevrHandler handler;
} Route;

typedef struct StaticMount {
	char *mount_path;
	char *directory;
} StaticMount;

void route_init(RevrApp *app);
void route_free(RevrApp *app);

void route_add(RevrApp *app, HttpMethod method, const char *path,
               RevrHandler handler);
void route_static_mount(RevrApp *app, const char *mount_path,
                        const char *directory);

void route_dispatch(RevrApp *app, RevrRequest *req, RevrResponse *res);
