#include <FreeRTOS.h>
#include <esp/uart.h>
#include <esp8266.h>
#include <stdio.h>
#include <stdlib.h>
#include <task.h>

#include "pwm.h"

#include "homekit_config.h"
#include "servo.h"

void servo_task(void *pvParameters) {
  printf("Hello from servo_task!\r\n");

  uint8_t pins[1] = {PWM_GPIO};
  printf("pwm_init(1, [%d])\n", PWM_GPIO);

  pwm_init(1, pins, false);

  printf("pwm_set_freq(50)     # 50 Hz\n");
  pwm_set_freq(50);

  printf("pwm_set_duty(UINT16_MAX*0.05)\n");
  pwm_set_duty(UINT16_MAX * 0.05);

  printf("pwm_start()\n");
  pwm_start();

  vTaskDelay(1500 / portTICK_PERIOD_MS);

  printf("pwm_set_duty(UINT16_MAX*0.10)\n");
  pwm_set_duty(UINT16_MAX * 0.10);

  vTaskDelay(1500 / portTICK_PERIOD_MS);
  pwm_stop();

  vTaskDelay(2000 / portTICK_PERIOD_MS);

  vTaskDelete(NULL);
}

void servo_rotate_to_angle(int angle) {
  printf("rotate servo to angle %d\n", angle);

  xTaskCreate(servo_task, "Servo PWM Task", 1024, NULL, 2, NULL);
}