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

#define ADC_MAKE_DATE(adc, chan)		((void *)(uint32_t)(adc << 8 | chan))
#define ADC_GET_GROUP(data)			((uint8_t)((uint32_t)data >> 8))
#define ADC_GET_CHAN(data)			((uint8_t)((uint32_t)data & 0xff))

static int test_adc_handler(void *private_data, uint32_t adc_value)
{
	pr_info("test adc %u %u: %lu\n",
		ADC_GET_GROUP(private_data), ADC_GET_CHAN(private_data), adc_value);
	return 0;
}

static void test_task_func(void *param)
{
	static uint32_t count = 0;

	while (1) {
		vTaskDelay(1000);
		pr_info("test adc run@%" PRIu32 "tick %" PRIu32 "\n",
			count++, xTaskGetTickCount());
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

	adc_register_handler(ADC_GROUP0, ADC_CHAN0, test_adc_handler, ADC_MAKE_DATE(ADC_GROUP0, ADC_CHAN0));
	adc_register_handler(ADC_GROUP0, ADC_CHAN1, test_adc_handler, ADC_MAKE_DATE(ADC_GROUP0, ADC_CHAN1));
	adc_register_handler(ADC_GROUP0, ADC_CHAN2, test_adc_handler, ADC_MAKE_DATE(ADC_GROUP0, ADC_CHAN2));
	adc_register_handler(ADC_GROUP0, ADC_CHAN3, test_adc_handler, ADC_MAKE_DATE(ADC_GROUP0, ADC_CHAN3));
	adc_register_handler(ADC_GROUP0, ADC_CHAN4, test_adc_handler, ADC_MAKE_DATE(ADC_GROUP0, ADC_CHAN4));
}
