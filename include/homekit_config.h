#pragma once

#include <homekit/characteristics.h>
#include <homekit/homekit.h>

#include "../config.h"

void on_homekit_event(homekit_event_t event);
void led_write(bool on);

extern homekit_characteristic_t status_active;
extern homekit_characteristic_t air_quality;
extern homekit_characteristic_t pm25_density;
extern homekit_characteristic_t pm10_density;
extern homekit_characteristic_t no2_density;
extern homekit_characteristic_t so2_density;
extern homekit_characteristic_t o3_density;
extern homekit_characteristic_t co_level;

extern homekit_characteristic_t light_on;
extern bool light_is_on;

extern homekit_server_config_t homekit_config;
extern bool homekit_initialized;
