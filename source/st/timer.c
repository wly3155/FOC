/*
 * Copyright (C) <2023>  <wuliyong3155@163.com>

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

#include "printf.h"
#include "utils.h"

#include "st/clock.h"
#include "st/irq.h"
#include "st/timer.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"

#define TIME_BASE_FREQ_HZ		(40 * 1e3) /* 40K Timer */
#define TIME_BASE_NS			(1 * 1e9 / TIME_BASE_FREQ_HZ)
#define TIME_MAX_PERIOD			(TIME_BASE_NS * UINT16_MAX)

static struct timer_device *timer_dev = NULL;

static int timer_irq_handler(void *private_data)
{
	if (timer_dev->handler)
		timer_dev->handler(timer_dev->private_data);

	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
	return 0;
}

int timer_enable(struct timer_device *dev, bool enable)
{
	TIM_Cmd(TIM3, enable);
        return 0;
}

static int timer_init(uint32_t period_ns)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	configASSERT(period_ns <= TIME_MAX_PERIOD);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	TIM_InternalClockConfig(TIM3);
	TIM_TimeBaseStructure.TIM_Period = period_ns / TIME_BASE_NS - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = configAPB1_TIM_CLOCK_HZ / TIME_BASE_FREQ_HZ - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	//TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	irq_register(TIM3_IRQn, timer_irq_handler, NULL);
	return 0;
}

void timer_register(struct timer_device *dev, uint32_t period_ns)
{
	fatal(!timer_dev);
	timer_dev = dev;
	timer_init(period_ns);
}
