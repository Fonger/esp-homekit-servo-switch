#pragma once

#include <homekit/homekit.h>
#include <http-parser/http_parser.h>

void light_identify(homekit_value_t _value);
homekit_value_t light_on_get();
void light_on_set(homekit_value_t value);

int on_http_message_complete(http_parser *parser);
void button_poll_task(void *pvParameters);