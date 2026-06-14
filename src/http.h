#pragma once

#include "internal.h"

typedef struct file_content file_content;

int http_parse_request(char* req, RevrRequest* request);
char* http_generate_response(const RevrResponse* response);

void http_send_file(RevrResponse *res, file_content *content);

void http_404(RevrResponse* res);
void http_method_not_allowed(RevrResponse* res);
void http_not_implemented(RevrResponse* res);
