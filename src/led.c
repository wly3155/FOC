#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "led.h"

static void led_task(void *param)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	while (1) {
		GPIO_ResetBits(GPIOB, GPIO_Pin_12);
		vTaskDelay(1000);
		GPIO_SetBits(GPIOB, GPIO_Pin_12);
		vTaskDelay(1000);
	}
}

static void led_task2(void *param)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	while (1) {
		GPIO_SetBits(GPIOC, GPIO_Pin_9);
		vTaskDelay(1000);
		GPIO_ResetBits(GPIOC, GPIO_Pin_9);
		vTaskDelay(1000);
	}
}

void led_init(void)
{
#define config_LED_TASK_STACK_SIZE 1024
#define config_LED_TASK_PRI (configMAX_PRIORITIES - 1)
    xTaskCreate( led_task, "LED", config_LED_TASK_STACK_SIZE, ( void * ) NULL, config_LED_TASK_PRI, NULL);
    xTaskCreate( led_task2, "LED", config_LED_TASK_STACK_SIZE, ( void * ) NULL, config_LED_TASK_PRI, NULL);
}
/*-----------------------------------------------------------*/