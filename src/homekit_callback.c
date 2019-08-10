
#include <FreeRTOS.h>
#include <cJSON.h>
#include <esp8266.h>
#include <esplibs/libmain.h>
#include <espressif/esp_system.h>
#include <event_groups.h>
#include <http-parser/http_parser.h>
#include <stdio.h>
#include <stdlib.h>
#include <task.h>

#include "homekit_callback.h"
#include "homekit_config.h"
#include "http_request.h"
#include "servo.h"

void led_write(bool on) { gpio_write(LED_GPIO, on ? 0 : 1); }

void light_identify_task(void *_args) {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 2; j++) {
      led_write(true);
      vTaskDelay(100 / portTICK_PERIOD_MS);
      led_write(false);
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    vTaskDelay(250 / portTICK_PERIOD_MS);
  }

  led_write(false);
  vTaskDelete(NULL);
}

void light_identify(homekit_value_t _value) {
  printf("AC identify\n");
  xTaskCreate(light_identify_task, "Thermostat identify", 128, NULL, 2, NULL);
};

int on_http_message_complete(http_parser *parser) {
  printf("http response message complete\n");
  const cJSON *feed = NULL;
  const cJSON *feeds = NULL;
  const cJSON *EPA = NULL;
  cJSON *air_json = cJSON_Parse(body_data.data);
  if (air_json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      printf("Error before: %s\n", error_ptr);
    }
    goto end;
  }
  feeds = cJSON_GetObjectItemCaseSensitive(air_json, "feeds");
  feed = cJSON_GetArrayItem(feeds, 0);
  if (feed == NULL)
    goto end;
  EPA = cJSON_GetObjectItemCaseSensitive(feed, "EPA");
  if (EPA == NULL)
    goto end;

  cJSON *aqi = cJSON_GetObjectItemCaseSensitive(EPA, "AQI");
  cJSON *pm25 = cJSON_GetObjectItemCaseSensitive(EPA, "PM2_5");
  cJSON *pm10 = cJSON_GetObjectItemCaseSensitive(EPA, "PM10");
  cJSON *co = cJSON_GetObjectItemCaseSensitive(EPA, "CO");
  // cJSON *co2 = cJSON_GetObjectItemCaseSensitive(EPA, "CO2");
  cJSON *so2 = cJSON_GetObjectItemCaseSensitive(EPA, "SO2");
  cJSON *no2 = cJSON_GetObjectItemCaseSensitive(EPA, "NO2");
  cJSON *o3 = cJSON_GetObjectItemCaseSensitive(EPA, "O3");

  uint8_t qualityIndex = 0;
  if (cJSON_IsString(aqi)) {
    printf("AQI %s\n", aqi->valuestring);
    int aqi_number = atoi(aqi->valuestring);
    if (aqi_number < 0)
      qualityIndex = 0;
    else if (aqi_number <= 50)
      qualityIndex = 1;
    else if (aqi_number <= 100)
      qualityIndex = 2;
    else if (aqi_number <= 150)
      qualityIndex = 3;
    else if (aqi_number <= 200)
      qualityIndex = 4;
    else
      qualityIndex = 5;
  }

  air_quality.value = HOMEKIT_UINT8(qualityIndex);
  homekit_characteristic_notify(&air_quality, air_quality.value);
  status_active.value = HOMEKIT_BOOL(qualityIndex > 0);
  homekit_characteristic_notify(&status_active, status_active.value);

  if (cJSON_IsNumber(pm25)) {
    printf("PM2.5 %f ug/m3\n", pm25->valuedouble);
    pm25_density.value = HOMEKIT_FLOAT(pm25->valuedouble);
    homekit_characteristic_notify(&pm25_density, pm25_density.value);
  }
  if (cJSON_IsNumber(pm10)) {
    printf("PM10 %f ug/m3\n", pm10->valuedouble);
    pm10_density.value = HOMEKIT_FLOAT(pm10->valuedouble);
    homekit_characteristic_notify(&pm10_density, pm10_density.value);
  }
  if (cJSON_IsNumber(so2)) {
    printf("SO2 %f ppb\n", so2->valuedouble);
    so2_density.value =
      HOMEKIT_FLOAT(so2->valuedouble * 2.616); // ppb to ug/m3 25c 1atm
    homekit_characteristic_notify(&so2_density, so2_density.value);
  }
  if (cJSON_IsNumber(no2)) {
    printf("NO2 %f ppb\n", no2->valuedouble);
    no2_density.value =
      HOMEKIT_FLOAT(no2->valuedouble * 1.88); // ppb to ug/m3 25c 1atm
    homekit_characteristic_notify(&no2_density, no2_density.value);
  }
  if (cJSON_IsNumber(o3)) {
    printf("O3 %f ppb\n", o3->valuedouble);
    o3_density.value =
      HOMEKIT_FLOAT(o3->valuedouble * 1.962); // ppb to ug/m3 25c 1atm
    homekit_characteristic_notify(&o3_density, o3_density.value);
  }
  if (cJSON_IsNumber(co)) {
    printf("CO %f ppm\n", co->valuedouble);
    co_level.value = HOMEKIT_FLOAT(co->valuedouble);
    homekit_characteristic_notify(&co_level, co_level.value);
  }
end:
  cJSON_Delete(air_json);
  return -1;
}

homekit_value_t light_on_get() { return HOMEKIT_BOOL(light_is_on); };
void light_on_set(homekit_value_t value) {
  light_on.value = value;
  light_is_on = value.bool_value;
  servo_rotate_to_angle(light_is_on ? 60 : 95);
};

void button_poll_task(void *pvParameters) {
  while (true) {
    while (gpio_read(BTN_GPIO) == true) {
      taskYIELD();
    }
    printf("button_press!\n");
    light_on_set(HOMEKIT_BOOL(!light_is_on));
    homekit_characteristic_notify(&light_on, light_on.value);
    vTaskDelay(250 / portTICK_PERIOD_MS);
  }
}