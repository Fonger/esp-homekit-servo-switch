#include <FreeRTOS.h>
#include <esp8266.h>
#include <stdio.h>

#include "homekit_callback.h"
#include "homekit_config.h"

bool homekit_initialized = false;
bool light_on = false;
homekit_accessory_t *homekit_accessories[] = {
  HOMEKIT_ACCESSORY(
      .id = 1, .category = homekit_accessory_category_lightbulb,
      .services =
        (homekit_service_t *[]){
          HOMEKIT_SERVICE(
            ACCESSORY_INFORMATION,
            .characteristics =
              (homekit_characteristic_t *[]){
                HOMEKIT_CHARACTERISTIC(NAME, "Servo Light Switch"),
                HOMEKIT_CHARACTERISTIC(MANUFACTURER, "Fonger"),
                HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "12345678"),
                HOMEKIT_CHARACTERISTIC(MODEL, "Wall-Servo-Switch"),
                HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "0.0.1"),
                HOMEKIT_CHARACTERISTIC(IDENTIFY, light_identify), NULL}),
          HOMEKIT_SERVICE(
            LIGHTBULB, .primary = true,
            .characteristics =
              (homekit_characteristic_t *[]){
                HOMEKIT_CHARACTERISTIC(NAME, "Servo Light"),
                HOMEKIT_CHARACTERISTIC(ON, false, .getter = light_on_get,
                                       .setter = light_on_set),
                NULL}),
          NULL}),
  NULL};

homekit_server_config_t homekit_config = {.accessories = homekit_accessories,
                                          .password = HOMEKIT_PASSWORD};