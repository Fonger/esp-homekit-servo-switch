#pragma once

#include <homekit/homekit.h>

void light_identify(homekit_value_t _value);
homekit_value_t light_on_get();
void light_on_set(homekit_value_t value);
