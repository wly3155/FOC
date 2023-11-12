#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "foc_board.h"
#include "printf.h"
#include <st/st_gpio.h>

static int led_set_off(enum LED_PIN pin)
{
    gpio_set_output_value(pin, OUTPUT_HIGH);
    return 0;
}

static int led_resource_init()
{
    gpio_set_mode(LED_GREEN_PIN, MODE_OUTPUT);
    gpio_set_mode(LED_RED_PIN, MODE_OUTPUT);
    return 0;
}


void led_init(void)
{
#define config_LED_TASK_STACK_SIZE 1024
#define config_LED_TASK_PRI (configMAX_PRIORITIES - 3)
    led_resource_init();
    led_set_off(LED_RED_PIN);
    led_set_off(LED_GREEN_PIN);

}
