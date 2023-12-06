/*
 * Copyright (C) <2022>  <wuliyong3155@163.com>

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "FreeRTOS.h"
#include "task.h"

#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "irq.h"
#include "printf.h"
#include "st/timer_wrapper.h"
#ifdef CFG_SEGGER_RTT_SUPPORT
#include "RTT/SEGGER_RTT.h"
#endif

#define TIM_RES_NS			(1000)

uint32_t test_timer_irq = 0;
static int timer_wrapper_irq_handler(void *private_data)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
		test_timer_irq = 1;
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
	pr_info("%s\n", __func__);
	return 0;
}

#include "misc.h"
int timer_wrapper_enable(bool enable)
{
	NVIC_InitTypeDef nvic;

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	irq_register(TIM2_IRQn, timer_wrapper_irq_handler, NULL);
	nvic.NVIC_IRQChannel = TIM2_IRQn;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&nvic);

	TIM_Cmd(TIM2, enable);
        return 0;
}

int timer_wrapper_init(uint32_t period_ns)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_InternalClockConfig(TIM2);
	TIM_TimeBaseStructure.TIM_Period = period_ns / TIM_RES_NS - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = configCPU_CLOCK_HZ / TIM_RES_NS;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	//TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	return 0;
}
