#pragma once

typedef struct RevrApp RevrApp;
typedef struct RevrRequest RevrRequest;
typedef struct RevrResponse RevrResponse;

typedef void (*RevrHandler)(RevrRequest *req, RevrResponse *res);

typedef enum {
	REVR_OK = 0,

	REVR_ERR_OOM,
	REVR_ERR_NOT_FOUND,
	REVR_ERR_NOT_DIRECTORY,
	REVR_ERR_PERMISSION,
	REVR_ERR_INVALID_ARGUMENT,
} RevrStatus;

RevrApp *revr_app_create();
void revr_app_free(RevrApp *app);

void revr_listen(RevrApp *app, const char *port);

void revr_get(RevrApp *app, const char *path, RevrHandler handler);
RevrStatus revr_static(RevrApp *app, const char *path, const char *directory);

RevrStatus revr_send_text(RevrResponse *res, int status, const char *text);
RevrStatus revr_send_html(RevrResponse *res, int status, const char *html);
RevrStatus revr_send_file(RevrResponse *res, int status, const char *path);

void revr_send_ok(RevrResponse *res);
void revr_send_created(RevrResponse *res);
void revr_send_no_content(RevrResponse *res);

void revr_send_bad_request(RevrResponse *res);
void revr_send_unauthorized(RevrResponse *res);
void revr_send_forbidden(RevrResponse *res);
void revr_send_not_found(RevrResponse *res);
void revr_send_method_not_allowed(RevrResponse *res);
void revr_send_conflict(RevrResponse *res);

void revr_send_internal_error(RevrResponse *res);
void revr_send_not_implemented(RevrResponse *res);
void revr_send_bad_gateway(RevrResponse *res);
void revr_send_service_unavailable(RevrResponse *res);

const char *revr_req_header(RevrRequest *req, const char *name);
void revr_res_header(RevrResponse *res, const char *name, const char *value);
