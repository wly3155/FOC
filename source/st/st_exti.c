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

#include <FreeRTOS.h>
#include <stm32f4xx.h>
#include <stm32f4xx_syscfg.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_exti.h>
#include <misc.h>

#include "st/st_gpio.h"

enum {
    exit4,
    exit5,
};

static uint8_t get_exti_irq_num_from_pin_source(    uint8_t pin_source)
{
    switch (pin_source) {
    case exit4:
        return EXTI4_IRQn;
    case exit5:
        return EXTI9_5_IRQn;
    default:
        configASSERT(0);
    }

    return 0;
}

int exti_init(enum chip_pin pin)
{
    int ret =0;
    struct exit_info_t info;
    uint8_t exit_irq_num = 0;
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef  EXTI_InitStructure;

    ret = get_exti_info_from_chip_pin(pin, &info);
    if (ret < 0)
        return ret;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* Connect EXTI Line0 to PA0 pin */
    SYSCFG_EXTILineConfig(info.port_source, info.pin_source);

    /* Configure EXTI Line0 */
    EXTI_InitStructure.EXTI_Line = info.line;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable and set EXTI Line0 Interrupt to the lowest priority */
    exit_irq_num = get_exti_irq_num_from_pin_source(info.pin_source);
    NVIC_InitStructure.NVIC_IRQChannel = exit_irq_num;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    return exit_irq_num;
}

