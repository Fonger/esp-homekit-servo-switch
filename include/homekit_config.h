#pragma once

#include <homekit/characteristics.h>
#include <homekit/homekit.h>

#include "../config.h"

void led_write(bool on);

extern bool light_on;
extern homekit_server_config_t homekit_config;
extern bool homekit_initialized;
