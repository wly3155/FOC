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

#ifndef __ST_TIMER_H__
#define __ST_TIMER_H__

#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx_tim.h"

#ifdef __cplusplus
extern "c" {
#endif

#define CCMR_MAX                               (2)
#define CCR_CHANNEL_MAX                        (4)

#define TIMER_COUNTER_BASE_FREQ_HZ             (12000000)

enum TIMER_INDEX {
    TIMER_INVALID = 0,
    TIMER1,
    TIMER2,
    TIMER3,
    TIMER4,
    TIMER5,
    TIMER6,
    TIMER7,
    TIMER8,
    TIMER9,
    TIMER10,
    TIMER11,
    TIMER12,
    TIMER13,
    TIMER14,
    TIMER_MAX,
};

enum {
    CHANNEL_1 = 0,
    CHANNEL_2,
    CHANNEL_3,
    CHANNEL_4,
    MAX_CHANNEL,
};

struct timer_reg_t {
    __IO uint32_t cr1;
    __IO uint32_t cr2;
    __IO uint32_t smcr;
    __IO uint32_t dier;
    __IO uint32_t sr;
    __IO uint32_t egr;
    __IO uint32_t ccmr[CCMR_MAX];
    __IO uint32_t ccer;
    __IO uint32_t cnt;
    __IO uint32_t psc;
    __IO uint32_t arr;
    __IO uint32_t ccr[CCR_CHANNEL_MAX];
    __IO uint32_t dcr;
    __IO uint32_t dmar;
};

struct timer_device {
    uint8_t id;
    bool in_use;
    uint32_t freq_hz;
    uint32_t base_counter_freq_hz;
    uint32_t reg_base;
    struct timer_reg_t *regs;
    TIM_TypeDef *base_addr;
    uint32_t periph_clock;
    uint32_t periph_clock_group;
    bool irq_enable;
    uint32_t irq_num;
    uint32_t irq_request_type;
    int (*irq_handler)(void *private_data);
    void *private_data;
};

#define is_advance_timer(timer) ((timer == TIMER1 || timer == TIMER8) ? true : false)
struct timer_device *request_timer_device(uint8_t id);
int timer_init(struct timer_device *dev);
int timer_start(struct timer_device *dev);
int timer_stop(struct timer_device *dev);

int timer_input_capture_init(struct timer_device *dev, uint8_t channel);
int timer_input_capture_enable(struct timer_device *dev, uint8_t channel);
int timer_input_capture_disable(struct timer_device *dev, uint8_t channel);
uint8_t get_timer_gpio_alter_mode(uint8_t timer_id);
int timer_platform_init(void);
#ifdef __cplusplus
}
#endif
#endif
