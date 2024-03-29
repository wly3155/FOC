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

#include "cm4/scb.h"
#include "st/st_board.h"
#include "st/st_gpio.h"
#include "st/st_reg.h"
#include "st/time.h"
#include "st/st_timer.h"

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

void prvSetupHardware(void)
{
	/* Setup STM32 system (clock, PLL and Flash configuration) */
	SystemInit();
	/* Ensure all priority bits are assigned as preemption priority bits. */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	gpio_platform_init();
	time_platform_init();
	//timer_platform_init();
}
