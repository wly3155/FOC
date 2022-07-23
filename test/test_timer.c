#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "log.h"
#include "st/st_timer.h"

static struct timer_device *timer_dev;
static struct timer_device *input_capture_dev;

static int test_timer_handler(void *private_data)
{
    static uint8_t count = 0;
    logi("%s %u\n", __func__, count++);
    return 0;
}

static void test_task(void *param)
{
    timer_dev = request_timer_device(TIMER2);
    configASSERT(timer_dev);

    timer_dev->freq_hz = 10;
    timer_dev->irq_enable = true;
    timer_dev->irq_request_type = TIM_IT_Update;
    timer_dev->irq_handler = test_timer_handler;
    timer_init(timer_dev);
    timer_start(timer_dev);

    input_capture_dev = request_timer_device(TIMER5);
    configASSERT(input_capture_dev);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void test_init(void)
{
#define config_TEST_TASK_STACK_SIZE 1024
#define config_TEST_TASK_PRI (configMAX_PRIORITIES - 3)
    xTaskCreate(test_task, "test", config_TEST_TASK_STACK_SIZE, ( void * ) NULL, config_TEST_TASK_PRI, NULL);
}