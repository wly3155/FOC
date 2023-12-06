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
#include "st/timer_wrapper.h"

#include "misc.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#ifdef CFG_SEGGER_RTT_SUPPORT
#include "RTT/SEGGER_RTT.h"
#endif

void TIM_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  uint16_t PrescalerValue = 0;

  /* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  /* Enable the TIM2 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 6000000) - 1;
  /* Prescaler configuration */
  TIM_PrescalerConfig(TIM2, PrescalerValue, TIM_PSCReloadMode_Immediate);

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 65530;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

  /* TIM2 enable counter */
  TIM_Cmd(TIM2, ENABLE);
}

static void test_task_func(void *param)
{
	static uint8_t count = 0;
	uint8_t task_id = (uint8_t)(uint32_t)param;
    extern uint32_t test_timer_irq;
	//timer_wrapper_enable(true);
	while (1) {
        if (test_timer_irq) {
            test_timer_irq = 0;
		    pr_info("[%u] %s runs @%u\n", task_id, __func__, count++);
        }
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
	timer_wrapper_init(1000000);
	timer_wrapper_enable(true);

    //TIM_Config();
}


void TIM2_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    SEGGER_RTT_WriteString(0, "tim irq enter\n");
  }
}
