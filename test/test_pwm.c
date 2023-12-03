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

#include "st/pwm_wrapper.h"

static void test_pwm_task(void *param)
{
	pwm_wrapper_set_duty(PWM_GROUP0, 0.5);
	pwm_wrapper_group_enable(PWM_GROUP0, true);
	pwm_wrapper_channel_enable(PWM_GROUP0, PWM_CHAN0, true);
	pwm_wrapper_channel_enable(PWM_GROUP0, PWM_CHAN1, true);
	pwm_wrapper_channel_enable(PWM_GROUP0, PWM_CHAN2, true);
	while (1) {
		vTaskDelay(1000);
	}
}

void test_pwm_init(void)
{
#define config_TEST_TASK_STACK_SIZE 512
#define config_TEST_TASK_PRI (configMAX_PRIORITIES - 3)
	xTaskCreate(test_pwm_task, "test_pwm", config_TEST_TASK_STACK_SIZE, ( void * ) NULL, config_TEST_TASK_PRI, NULL);
}
