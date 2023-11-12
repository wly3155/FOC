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

static void test_task_func(void *param)
{
	static uint8_t count = 0;

	while (1) {
		vTaskDelay(pdMS_TO_TICKS(1000));
		pr_info("%s runs @%u\n", __func__, count++);
	}
}

void test_task_init(void)
{
#define config_TEST_TASK_STACK_SIZE 1024
#define config_TEST_TASK_PRI (configMAX_PRIORITIES - 3)
	xTaskCreate(test_task_func, "test_task", config_TEST_TASK_STACK_SIZE,
		( void * ) NULL, config_TEST_TASK_PRI, NULL);
}