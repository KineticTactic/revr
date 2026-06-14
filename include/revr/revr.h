#pragma once

typedef struct RevrApp RevrApp;
typedef struct RevrRequest RevrRequest;
typedef struct RevrResponse RevrResponse;

typedef void (*RevrHandler)(RevrRequest *req, RevrResponse *res);

RevrApp* revr_app_create();
void revr_app_free(RevrApp* app);

void revr_listen(RevrApp *app, const char *port);

void revr_get(RevrApp *app, const char *path, RevrHandler handler);
int revr_static(RevrApp *app, const char *path, const char *directory);

void revr_send_text(RevrResponse* res, int status, const char* text);

typedef enum {
    REVR_OK = 0,

    REVR_ERR_OOM,
    REVR_ERR_NOT_FOUND,
    REVR_ERR_NOT_DIRECTORY,
    REVR_ERR_PERMISSION,
    REVR_ERR_INVALID_ARGUMENT,
} RevrStatus;
