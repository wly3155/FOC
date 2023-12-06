/*
 * This file is licensed under the Apache License, Version 2.0.
 *
 * Copyright (c) 2023 wuliyong3155@163.com
 *
 * A copy of the license can be obtained at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "FreeRTOS.h"
#include "task.h"

#include "printf.h"
#include "utils.h"
#include "st/timer.h"

static struct timer_device test_timer_dev;

static void test_task_func(void *param)
{
	static uint8_t count = 0;
	uint8_t task_id = (uint8_t)(uint32_t)param;

	while (1) {
		pr_info("[%u] %s runs @%u\n", task_id, __func__, count++);
            	vTaskDelay(1000);
	}
}

static int test_timer_handler(void *private_data)
{
	pr_info("%s\n", __func__);
	return 0;
}

void test_task_init(void)
{
#define config_TEST_TASK_STACK_SIZE 512
#define config_TEST_TASK_PRI (configMAX_PRIORITIES - 3)
	xTaskCreate(test_task_func, "test_task1", config_TEST_TASK_STACK_SIZE,
		( void * ) 1, config_TEST_TASK_PRI, NULL);
	xTaskCreate(test_task_func, "test_task2", config_TEST_TASK_STACK_SIZE,
		( void * ) 2, config_TEST_TASK_PRI, NULL);

	test_timer_dev.handler = test_timer_handler;
	test_timer_dev.private_data = NULL;	
	timer_register(&test_timer_dev, ms_to_ns(1000));
	timer_enable(&test_timer_dev, true);
}
