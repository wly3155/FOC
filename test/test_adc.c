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

#include <inttypes.h>

#include "FreeRTOS.h"
#include "task.h"

#include "printf.h"

#include "st/adc.h"

static void test_task_func(void *param)
{
//	uint8_t adc = 0, chan = 0;
	static uint32_t count = 0;

	while (1) {
//		for (adc = 0; adc < MAX_ADC_NUM; adc++) {
//			for (chan = 0; chan < MAX_CHAN_PRE_ADC; chan++)
//				pr_info("get adc %u chan %u: %u\n",
//					adc, chan, adc_get_value(adc, chan));
//		}
		vTaskDelay(1000);
		pr_info("test adc run@%" PRIu32 "tick %" PRIu32 "\n", count++, xTaskGetTickCount());
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
}

