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


#include "misc.h"
#include "st/timer.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#ifdef CFG_SEGGER_RTT_SUPPORT
#include "RTT/SEGGER_RTT.h"
#endif

static void test_task_func(void *param)
{
	static uint8_t count = 0;
	uint8_t task_id = (uint8_t)(uint32_t)param;

	while (1) {
		    pr_info("[%u] %s runs @%u\n", task_id, __func__, count++);
            vTaskDelay(1000);
	}
}

void test_task_init(void)
{
#define config_TEST_TASK_STACK_SIZE 512
#define config_TEST_TASK_PRI (configMAX_PRIORITIES - 3)
	xTaskCreate(test_task_func, "test_task1", config_TEST_TASK_STACK_SIZE,
		( void * ) 1, config_TEST_TASK_PRI, NULL);
	xTaskCreate(test_task_func, "test_task2", config_TEST_TASK_STACK_SIZE,
		( void * ) 2, config_TEST_TASK_PRI, NULL);
	timer_init(1000000000);
	timer_enable(true);
}
