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

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <st/st_board.h>
#include <st/st_reg.h>
#include <st/st_gpio.h>
#include <st/st_timer.h>

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx.h"
#include "misc.h"

#include "time.h"
#include "irq.h"
#include <utils.h>

#define ptr_to_timer_id(ptr)                  ((uint32_t)ptr)
#define ptr_to_timer_irq_num(ptr)             ((uint32_t)ptr)

struct timer_irq_history_t {
    uint64_t timestamp;
    uint32_t irq_num;
    uint32_t irq_status;
};

#define TIMER_IRQ_HISTORY_NUM                 (20)
static struct timer_irq_history_t timer_irq_history[TIMER_IRQ_HISTORY_NUM];
static uint8_t timer_irq_history_index = 0;

static struct timer_device support_timer_list[] = {
    {
        .id = TIMER1,
        .freq_hz = 0,
        .in_use = false,
        .irq_enable = false,
        .irq_num = TIM1_CC_IRQn,
        .base_addr = TIM1,
        .regs = TIM1_BASE_REG,
        .periph_clock = RCC_APB2Periph_TIM1,
        .periph_clock_group = RCC_APB2Periph,
    },
    {
        .id = TIMER2,
        .freq_hz = 0,
        .in_use = false,
        .irq_enable = false,
        .irq_num = TIM2_IRQn,
        .base_addr = TIM2,
        .regs = TIM2_BASE_REG,
        .periph_clock = RCC_APB1Periph_TIM2,
        .periph_clock_group = RCC_APB1Periph,
    },
    {
        .id = TIMER5,
        .freq_hz = 0,
        .in_use = false,
        .irq_enable = false,
        .irq_num = TIM5_IRQn,
        .base_addr = TIM5,
        .regs = TIM5_BASE_REG,
        .periph_clock = RCC_APB1Periph_TIM5,
        .periph_clock_group = RCC_APB1Periph,
    },
    {
        .id = TIMER8,
        .freq_hz = 0,
        .in_use = false,
        .irq_enable = false,
        .irq_num = TIM8_CC_IRQn,
        .base_addr = TIM8,
        .regs = TIM8_BASE_REG,
        .periph_clock = RCC_APB2Periph_TIM8,
        .periph_clock_group = RCC_APB2Periph,
    },
};

static uint16_t inline timer_get_irq_status(struct timer_device *dev)
{
    return dev->base_addr->SR;
}

static void inline timer_clear_pending_irq(struct timer_device *dev, uint16_t pending)
{
    dev->base_addr->SR = ~pending;
}

static int timer_irq_handler(void *private_data)
{
    int i = 0;
    uint16_t irq_status = 0;
    struct timer_device *dev = NULL;
    uint8_t irq_num = ptr_to_timer_irq_num(private_data);

    for (i = 0; i < ARRAY_SIZE(support_timer_list); i++) {
        dev = &support_timer_list[i];
        if (dev->irq_num == irq_num) {
            irq_status = timer_get_irq_status(dev);
            if (irq_status & dev->irq_request_type) {
                if (!dev->irq_handler)
                    return -ENXIO;

                timer_irq_history[timer_irq_history_index].irq_num = irq_num;
                timer_irq_history[timer_irq_history_index].irq_status = irq_status;
                timer_irq_history[timer_irq_history_index].timestamp = get_boot_time_ns() / 100;
                timer_irq_history_index++;
                timer_irq_history_index %= TIMER_IRQ_HISTORY_NUM;

                dev->irq_handler(private_data);
                timer_clear_pending_irq(dev, dev->irq_request_type);
                return 0;
            }
        }
    }

    printf("unhandle timer %u %u\n", irq_num, irq_status);
    return -ENXIO;
}

static int timer_init_irq(struct timer_device *dev)
{
    NVIC_InitTypeDef nvic;

    nvic.NVIC_IRQChannel = dev->irq_num;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    nvic.NVIC_IRQChannelPreemptionPriority = 15;
    nvic.NVIC_IRQChannelSubPriority = 15;
    NVIC_Init(&nvic);

    irq_register(dev->irq_num, timer_irq_handler, (void *)dev->irq_num);
    TIM_ITConfig(dev->base_addr, dev->irq_request_type, ENABLE);
    return 0;
}

int timer_start(struct timer_device *dev)
{
    TIM_Cmd(dev->base_addr, ENABLE);
    return 0;
}

int timer_stop(struct timer_device *dev)
{
    TIM_Cmd(dev->base_addr, DISABLE);
    return 0;
}

int timer_init(struct timer_device *dev)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    uint16_t prescaler = 0;
    uint16_t period = 0;

    if (dev->periph_clock_group == RCC_APB1Periph) {
        RCC_APB1PeriphClockCmd(dev->periph_clock, ENABLE);
        prescaler = (uint16_t)((SystemCoreClock / 2) / TIMER_COUNTER_BASE_FREQ_HZ) - 1;
    } else {
        RCC_APB2PeriphClockCmd(dev->periph_clock, ENABLE);
        prescaler = (uint16_t)((SystemCoreClock) / TIMER_COUNTER_BASE_FREQ_HZ) - 1;
    }

    dev->base_counter_freq_hz = TIMER_COUNTER_BASE_FREQ_HZ;
    TIM_TimeBaseStructure.TIM_Prescaler = prescaler;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    period = dev->freq_hz ? TIMER_COUNTER_BASE_FREQ_HZ / dev->freq_hz - 1 : UINT16_MAX;
    TIM_TimeBaseStructure.TIM_Period = period;
    TIM_TimeBaseInit(dev->base_addr, &TIM_TimeBaseStructure);

    if (dev->irq_enable)
        timer_init_irq(dev);

    return 0;
}

struct timer_device *request_timer_device(uint8_t id)
{
    int i = 0;

    for (i = 0; i < ARRAY_SIZE(support_timer_list); i++) {
        if (support_timer_list[i].id == id) {
            support_timer_list[i].in_use = true;
            return &support_timer_list[i];
        }
    }

    return NULL;
}

int timer_input_capture_enable(struct timer_device *dev, uint8_t channel)
{
    uint8_t shift_bits = 0;

    if (!(dev->regs->cr1 & TIMX_CEN_TEST_VALUE))
        reg_bitwise_write(dev->regs->cr1, CEN_MASK << CEN_SHIFT_BITS, CEN_ENABLE << CEN_SHIFT_BITS);

    shift_bits = channel * DIER_CH_SHIFT_BITS + CCXIE_SHIFT_BITS;
    reg_bitwise_write(dev->regs->dier, CCXIE_MASK << shift_bits, CCXIE_ENABLE << shift_bits);
    return 0;
}

int timer_input_capture_disable(struct timer_device *dev, uint8_t channel)
{
    uint8_t shift_bits = 0;

    shift_bits = channel * DIER_CH_SHIFT_BITS + CCXIE_SHIFT_BITS;
    reg_bitwise_write(dev->regs->dier, CCXIE_MASK << shift_bits, CCXIE_DISABLE << shift_bits);

    if (!(dev->regs->cr1 & TIMX_CEN_TEST_VALUE))
        reg_bitwise_write(dev->regs->cr1, CEN_MASK << CEN_SHIFT_BITS, CEN_ENABLE << CEN_SHIFT_BITS);

    return 0;
}

int timer_input_capture_init(struct timer_device *dev, uint8_t channel)
{
    uint8_t shift_bits = 0;
    uint8_t ccmr_sel = 0;
    uint16_t mask = 0, value = 0;

    if (dev->periph_clock_group == RCC_APB1Periph)
        RCC_APB1PeriphClockCmd(dev->periph_clock, ENABLE);
    else
        RCC_APB2PeriphClockCmd(dev->periph_clock, ENABLE);

    shift_bits = channel > CHANNEL_2 ? (channel - 2) * CCMR_CH_SHIFT_BITS : channel * CCMR_CH_SHIFT_BITS;
    ccmr_sel = channel > CHANNEL_2 ? 1 : 0;
    mask = ((ICXF_MASK << (shift_bits + ICXF_SHIFT_BITS)) | (ICXPSC_MASK << (shift_bits + ICXPSC_SHIFT_BITS)) | (CCXS_MASK << (shift_bits + CCXS_SHIFT_BITS)));
    value = ((ICXF_NO_FILTER << (shift_bits + ICXF_SHIFT_BITS)) | (ICXPSC_NO_PSC << (shift_bits + ICXPSC_SHIFT_BITS)) | (CCXS_DIRCT_INPUT << (shift_bits + CCXS_SHIFT_BITS)));
    reg_bitwise_write(dev->regs->ccmr[ccmr_sel], mask, value);

    shift_bits = channel * CCER_CH_SHIFT_BITS + CCXNP_SHIFT_BITS;
    reg_bitwise_write(dev->regs->ccer, CCXNP_MASK << shift_bits, CCXP_BOTH_EDGE << shift_bits);

    reg_bitwise_write(dev->regs->cr1, UDIS_MASK_MASK << UDIS_SHIFT_BITS, UDIS_DISABLE << UDIS_SHIFT_BITS);

    if (dev->irq_enable)
        timer_init_irq(dev);

    return 0;
}

uint8_t get_timer_gpio_alter_mode(uint8_t timer_id)
{
    switch (timer_id) {
    case TIMER1 ... TIMER2:
        return GPIO_ALTER_1;
    case TIMER3 ... TIMER5:
        return GPIO_ALTER_2;
    case TIMER8 ... TIMER11:
        return GPIO_ALTER_3;
    default:
        return GPIO_ALTER_INVALID;
    }

    return GPIO_ALTER_INVALID;
}

int timer_platform_init(void)
{
    return 0;
}
