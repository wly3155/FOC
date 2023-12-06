#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "printf.h"
#include "st/timer_wrapper.h"

static void test_timer_task(void *param)
{
	//timer_wrapper_enable(true);

	while (1) {
		vTaskDelay(pdMS_TO_TICKS(1000));
		pr_info("%s\n", __func__);
	}
}

void test_timer_init(void)
{
#define config_TEST_TASK_STACK_SIZE 512
#define config_TEST_TASK_PRI (configMAX_PRIORITIES - 3)
	xTaskCreate(test_timer_task, "test_timer", config_TEST_TASK_STACK_SIZE,
		( void * ) NULL, config_TEST_TASK_PRI, NULL);
	//timer_wrapper_init(1000);
	//timer_wrapper_enable(true);
	pr_info("%s\n", __func__);
}