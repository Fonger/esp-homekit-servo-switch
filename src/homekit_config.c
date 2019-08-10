#include <FreeRTOS.h>
#include <esp8266.h>
#include <stdio.h>

#include "homekit_callback.h"
#include "homekit_config.h"

bool homekit_initialized = false;
bool light_is_on = false;

/*
 AIR_QUALITY
 - NAME
 - STATUS_ACTIVE
 - STATUS_FAULT
 - STATUS_TAMPERED
 - STATUS_LOW_BATTERY
 - OZONE_DENSITY
 - NITROGEN_DIOXIDE_DENSITY
 - SULPHUR_DIOXIDE_DENSITY
 - PM25_DENSITY
 - PM10_DENSITY
 - VOC_DENSITY
 - CARBON_MONOXIDE_LEVEL
 - CARBON_DIOXIDE_LEVEL
 */

homekit_characteristic_t status_active =
  HOMEKIT_CHARACTERISTIC_(STATUS_ACTIVE, false);
homekit_characteristic_t air_quality = HOMEKIT_CHARACTERISTIC_(AIR_QUALITY, 0);
homekit_characteristic_t pm25_density =
  HOMEKIT_CHARACTERISTIC_(PM25_DENSITY, 0);
homekit_characteristic_t pm10_density =
  HOMEKIT_CHARACTERISTIC_(PM10_DENSITY, 0);
homekit_characteristic_t o3_density = HOMEKIT_CHARACTERISTIC_(OZONE_DENSITY, 0);
homekit_characteristic_t no2_density =
  HOMEKIT_CHARACTERISTIC_(NITROGEN_DIOXIDE_DENSITY, 0);
homekit_characteristic_t so2_density =
  HOMEKIT_CHARACTERISTIC_(SULPHUR_DIOXIDE_DENSITY, 0);
homekit_characteristic_t co_level =
  HOMEKIT_CHARACTERISTIC_(CARBON_MONOXIDE_LEVEL, 0);

homekit_characteristic_t light_on = HOMEKIT_CHARACTERISTIC_(
  ON, false, .getter = light_on_get, .setter = light_on_set);

homekit_accessory_t *homekit_accessories[] = {
  HOMEKIT_ACCESSORY(
      .id = 1, .category = homekit_accessory_category_sensor,
      .services =
        (homekit_service_t *[]){
          HOMEKIT_SERVICE(
            ACCESSORY_INFORMATION,
            .characteristics =
              (homekit_characteristic_t *[]){
                HOMEKIT_CHARACTERISTIC(NAME, "環保署淡水測站"),
                HOMEKIT_CHARACTERISTIC(MANUFACTURER, "行政院環境保護署"),
                HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "EPA-Tamsui"),
                HOMEKIT_CHARACTERISTIC(MODEL, "EPA"),
                HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "0.0.1"),
                HOMEKIT_CHARACTERISTIC(IDENTIFY, light_identify), NULL}),
          HOMEKIT_SERVICE(AIR_QUALITY_SENSOR, .primary = true,
                          .characteristics =
                            (homekit_characteristic_t *[]){
                              HOMEKIT_CHARACTERISTIC(NAME, "EPA 空氣"),
                              &status_active, &air_quality, &pm10_density,
                              &pm25_density, &o3_density, &so2_density,
                              &no2_density, &co_level, NULL}),
          NULL}),
  HOMEKIT_ACCESSORY(
      .id = 2, .category = homekit_accessory_category_lightbulb,
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
                HOMEKIT_CHARACTERISTIC(NAME, "Servo Light"), &light_on, NULL}),
          NULL}),
  NULL};

homekit_server_config_t homekit_config = {.accessories = homekit_accessories,
                                          .password = HOMEKIT_PASSWORD,
                                          .on_event = on_homekit_event};