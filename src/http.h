#pragma once

#include "internal.h"

typedef struct file_content file_content;

int http_parse_request(char* req, RevrRequest* request);
char* http_generate_response(const RevrResponse* response);

void http_free_req(RevrRequest* req);
void http_free_res(RevrResponse* res);

void http_send_file(RevrResponse *res, file_content *content);

void http_ok(RevrResponse *res);
void http_created(RevrResponse *res);
void http_no_content(RevrResponse *res);

void http_bad_request(RevrResponse *res);
void http_unauthorized(RevrResponse *res);
void http_forbidden(RevrResponse *res);
void http_not_found(RevrResponse *res);
void http_method_not_allowed(RevrResponse *res);
void http_conflict(RevrResponse *res);

void http_internal_error(RevrResponse *res);
void http_not_implemented(RevrResponse *res);
void http_bad_gateway(RevrResponse *res);
void http_service_unavailable(RevrResponse *res);
