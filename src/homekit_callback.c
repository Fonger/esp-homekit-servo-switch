
#include <FreeRTOS.h>
#include <esp8266.h>
#include <esplibs/libmain.h>
#include <espressif/esp_system.h>
#include <event_groups.h>
#include <stdio.h>
#include <stdlib.h>
#include <task.h>

#include "homekit_callback.h"
#include "homekit_config.h"
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

homekit_value_t light_on_get() { return HOMEKIT_BOOL(light_on); };
void light_on_set(homekit_value_t value) {
  light_on = value.bool_value;
  servo_rotate_to_angle(light_on ? 20 : 40);
};
