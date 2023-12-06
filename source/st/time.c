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

#include <stdbool.h>
#include <stdint.h>

#include "st/clock.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"

#define TIME_BASE_FREQ_HZ		(4 * 1e6)
#define TIME_BASE_NS			(1 * 1e9 / TIME_BASE_FREQ_HZ)

uint64_t get_boot_time_ns(void)
{
	/* TBD: max support 1073s and then overflow, need 64bits counter*/
	return (uint64_t)TIM_GetCounter(TIM2) * TIME_BASE_NS;
}

int time_platform_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_InternalClockConfig(TIM2);
	TIM_TimeBaseStructure.TIM_Period = UINT32_MAX -1;
	TIM_TimeBaseStructure.TIM_Prescaler = configAPB1_TIM_CLOCK_HZ / TIME_BASE_FREQ_HZ - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM_Cmd(TIM2, true);
	return 0;
}
