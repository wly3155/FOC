#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "log.h"
#include "foc_board.h"
#include "bldc/bldc_manager.h"

#include "st/st_timer.h"
#include "st/st_input_capture.h"

static int test_input_capture_irq_handler(void *private_data)
{
    return 0;
}

static void test_task(void *param)
{
    input_capture_bind_timer(INPUT_CAPTURE_GROUP0, TIMER5);
    input_capture_bind_chip_pin(INPUT_CAPTURE_GROUP0, CHANNEL_1, INPUT_CAPTURE_CH1);
    input_capture_bind_chip_pin(INPUT_CAPTURE_GROUP0, CHANNEL_2, INPUT_CAPTURE_CH2);
    input_capture_bind_chip_pin(INPUT_CAPTURE_GROUP0, CHANNEL_3, INPUT_CAPTURE_CH3);
    input_capture_bind_chip_pin(INPUT_CAPTURE_GROUP0, CHANNEL_4, INPUT_CAPTURE_CH4);

    input_capture_init(INPUT_CAPTURE_GROUP0, CHANNEL_1);
    input_capture_init(INPUT_CAPTURE_GROUP0, CHANNEL_2);
    input_capture_init(INPUT_CAPTURE_GROUP0, CHANNEL_3);
    input_capture_init(INPUT_CAPTURE_GROUP0, CHANNEL_4);
    input_capture_irq_register(INPUT_CAPTURE_GROUP0, test_input_capture_irq_handler, NULL);

    input_capture_enable(INPUT_CAPTURE_GROUP0, CHANNEL_1);
    input_capture_enable(INPUT_CAPTURE_GROUP0, CHANNEL_2);
    input_capture_enable(INPUT_CAPTURE_GROUP0, CHANNEL_3);
    input_capture_enable(INPUT_CAPTURE_GROUP0, CHANNEL_4);

    //bldc_enable(BLDC_ID_0, 0.3);
    bldc_enable(BLDC_ID_1, 0.3);
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void test_init(void)
{
#define config_TEST_TASK_STACK_SIZE 512
#define config_TEST_TASK_PRI (configMAX_PRIORITIES - 3)
    xTaskCreate(test_task, "test", config_TEST_TASK_STACK_SIZE, ( void * ) NULL, config_TEST_TASK_PRI, NULL);
}