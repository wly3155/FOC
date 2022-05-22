#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "led.h"

static int led_set_on(enum LED_PIN pin)
{
	gpio_set_output_value(pin, Bit_RESET);
	return 0;
}

static int led_set_off(enum LED_PIN pin)
{
	gpio_set_output_value(pin, Bit_SET);
	return 0;
}

static int led_resource_init()
{
	gpio_set_mode(LED_GREEN_PIN, GPIO_Mode_OUT);
	gpio_set_mode(LED_RED_PIN, GPIO_Mode_OUT);
	return 0;
}

static void led_task(void *param)
{
	while (1) {
		led_set_on(LED_GREEN_PIN);
		vTaskDelay(pdMS_TO_TICKS(1000));
		led_set_off(LED_GREEN_PIN);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

static void led_task2(void *param)
{
	while (1) {
		led_set_on(LED_RED_PIN);
		vTaskDelay(pdMS_TO_TICKS(1000));
		led_set_off(LED_RED_PIN);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void led_init(void)
{
#define config_LED_TASK_STACK_SIZE 1024
#define config_LED_TASK_PRI (configMAX_PRIORITIES - 3)
	led_resource_init();
	led_set_off(LED_RED_PIN);
	led_set_off(LED_GREEN_PIN);
	xTaskCreate( led_task, "LED", config_LED_TASK_STACK_SIZE, ( void * ) NULL, config_LED_TASK_PRI, NULL);
	xTaskCreate( led_task2, "LED", config_LED_TASK_STACK_SIZE, ( void * ) NULL, config_LED_TASK_PRI, NULL);
}
