#pragma once

#include <stddef.h>

void http_get_task(void *pvParameters);

typedef struct {
  char *data;
  size_t length;
} http_response_body_data;

extern http_response_body_data body_data;