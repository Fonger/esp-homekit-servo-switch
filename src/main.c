#include <FreeRTOS.h>
#include <esp/interrupts.h>
#include <esp/uart.h>
#include <esp8266.h>
#include <esplibs/libmain.h>
#include <espressif/esp_common.h>
#include <espressif/esp_sta.h>
#include <espressif/esp_wifi.h>
#include <homekit/characteristics.h>
#include <homekit/homekit.h>
#include <stdio.h>

#include <task.h>

#include "homekit_callback.h"
#include "homekit_config.h"
#include "http_request.h"
#include "servo.h"

void init();
void led_init();

static void wifi_init() {
  struct sdk_station_config wifi_config = {
    .ssid = WIFI_SSID,
    .password = WIFI_PASS,
  };

  sdk_wifi_set_opmode(STATION_MODE);
  sdk_wifi_station_set_config(&wifi_config);
  sdk_wifi_station_connect();
}

void on_homekit_event(homekit_event_t event) {
  if (event == HOMEKIT_EVENT_PAIRING_ADDED) {
    if (!homekit_initialized) {
      init();
    }
  } else if (event == HOMEKIT_EVENT_PAIRING_REMOVED) {
    if (!homekit_is_paired()) {
      printf("Restarting\n");
      sdk_system_restart();
    }
  }
}

void init() {
  led_init();
  servo_init();

  xTaskCreate(&http_get_task, "get_task", 2048, NULL, 2, NULL);

  gpio_enable(BTN_GPIO, GPIO_INPUT);
  gpio_set_pullup(BTN_GPIO, true, true);
  xTaskCreate(&button_poll_task, "btn_task", 256, NULL, 0, NULL);

  homekit_initialized = true;
}

void led_init() {
  gpio_enable(LED_GPIO, GPIO_OUTPUT);
  led_write(false);
}

void user_init(void) {
  uart_set_baud(0, 115200);

  wifi_init();

  homekit_server_init(&homekit_config);

  if (homekit_is_paired()) {
    init();
  }
}
