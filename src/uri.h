#pragma once

#include "internal.h"

void uri_decode(char* uri);
void remove_dot_segments(char *in, char *out);
void uri_parse_query(char *in, QueryParams *queries);
