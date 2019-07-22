#include <FreeRTOS.h>
#include <esp/uart.h>
#include <esp8266.h>
#include <stdio.h>
#include <stdlib.h>
#include <task.h>

#include "pwm.h"

#include "homekit_config.h"
#include "servo.h"

// My SG90 works on 500µs ~ 2650µs (spec: 500µ ~ 2400µ)
uint16_t calc_duty_from_angle(int angle) {
  return (0.025 + (0.1325 - 0.025) * (double)angle / 180) * UINT16_MAX;
}

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

  for (int i = 0; i < 180; i += 36) {
    pwm_set_duty(calc_duty_from_angle(i));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
  for (int i = 180; i >= 0; i -= 36) {
    pwm_set_duty(calc_duty_from_angle(i));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }

  pwm_stop();
  printf("pwm_stop()\n");

  vTaskDelay(1000 / portTICK_PERIOD_MS);

  vTaskDelete(NULL);
}

void servo_rotate_to_angle(int angle) {
  printf("rotate servo to angle %d\n", angle);

  xTaskCreate(servo_task, "Servo PWM Task", 1024, NULL, 2, NULL);
}