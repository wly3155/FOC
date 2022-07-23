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
#include <stddef.h>
#include <string.h>

#include <st/st_reg.h>
#include <st/st_board.h>
#include <st/st_gpio.h>
#include <st/st_timer.h>
#include <st/st_pwm.h>

#include "FreeRTOS.h"
#include "task.h"
#include "stm32f4xx.h"
#include "stm32f4xx_tim.h"


enum pwm_chan_pin {
    CHAN_POSITIVE,
    CHAN_NEGATIVE,
    MAX_CHAN_PIN,
};


enum {
    LOW_LEVEL,
    HIGH_LEVEL,
};

struct pwm_t {
    struct timer_device *timer_dev;
    uint8_t bind_pin[MAX_PWM_CHANNEL][MAX_CHAN_PIN];
    bool enabled;
    uint16_t freq_count;
    uint16_t duty_count[MAX_PWM_CHANNEL];
    int (*irq_handler)(void *private_data);
    void *private_data;
};

#define ptr_to_pwm_group(ptr)             ((uint8_t)((uint32_t)ptr))
#define pwm_group_to_ptr(id)              ((void *)((uint32_t)group))
#define pwm_chn_to_ccxit_bit(chn)         (chn + 1)
#define MAX_PWM_DUTY                      (0.8)

static struct pwm_t support_pwm[MAX_PWM_GROUP];

static int pwm_update_duty_count(struct pwm_t *pwm,
    uint8_t chn, uint16_t duty_count)
{
    reg_bitwise_write(pwm->timer_dev->regs.ccr[chn],
        CCRX_MASK << CCRX_SHIFT_BITS, pwm->duty_count[chn] << CCRX_SHIFT_BITS);
    return 0;
}

static int pwm_irq_handler(void *private_data)
{
    uint8_t group = ptr_to_pwm_group(private_data);
    struct pwm_t *pwm = NULL;
    struct timer_device *timer_dev = NULL;
    uint16_t status = 0;
    uint8_t chn = 0;

    configASSERT(group < MAX_PWM_GROUP);
	pwm = &support_pwm[group];
    timer_dev = pwm->timer_dev;

    status = reg_readl(timer_dev->regs.sr);
    for (chn = 0; chn < MAX_PWM_CHANNEL; chn++) {
        if (status & pwm_chn_to_ccxit_bit(chn)) {
            pwm->duty_count[chn] = pwm->freq_count - pwm->duty_count[chn];
            pwm_update_duty_count(pwm, chn, pwm->duty_count[chn]);
        }
    }

    return 0;
}

static int pwm_set_idle_status(uint8_t group, uint8_t channel,
    bool pos_value, bool neg_value)
{
    struct pwm_t *pwm = &support_pwm[group];
    uint8_t shift_bits = 0;
    uint16_t mask = 0, value = 0;

    shift_bits = OIS_CH_SHIFT_BITS * channel;
    mask = (OISX_MASK << (shift_bits + OISX_SHIFT_BITS))
        | (OISNX_MASK << (shift_bits + OISNX_SHIFT_BITS));
    value = (pos_value << (shift_bits + OISX_SHIFT_BITS))
        | (neg_value << (shift_bits + OISNX_SHIFT_BITS));
    reg_bitwise_write(pwm->timer_dev->regs.cr2, mask, value);
    return 0;
}

static int pwm_enable(uint8_t group, bool en)
{
    struct pwm_t *pwm = &support_pwm[group];
    uint8_t arpe_value = en ? ARPE_BUFFERED : ARPE_NOT_BUFFERED;
    uint8_t cen_value = en ? CEN_ENABLE : CEN_DISABLE;

    reg_bitwise_write(pwm->timer_dev->regs.cr1,
        ARPE_MASK_VALUE << ARPE_SHIBFT_BITS, arpe_value << ARPE_SHIBFT_BITS);
    reg_bitwise_write(pwm->timer_dev->regs.cr1,
        CEN_MASK_VALUE << CEN_SHIBFT_BITS, cen_value << CEN_SHIBFT_BITS);
    return 0;
}

static int pwm_channel_enable(uint8_t group, uint8_t channel, bool en)
{
    struct pwm_t *pwm = &support_pwm[group];
    uint8_t shift_bits = 0;
    uint16_t mask = 0, value = 0;
    uint8_t ccxe_en = 0;
    uint8_t ccxne_en = 0;

    shift_bits = CCER_CH_SHIFT_BITS * channel;
    mask = (CCXE_MASK << (shift_bits + CCXE_SHIFT_BITS)) \
        | (CCXNE_MASK << (shift_bits + CCXNE_SHIFT_BITS));
    ccxe_en = en ? CCXE_ENABLE : CCXE_DISABLE;
    ccxne_en = en ? CCXNE_ENABLE : CCXNE_DISABLE;
    value = (ccxe_en << (shift_bits + CCXE_SHIFT_BITS)) \
        | (ccxne_en << (shift_bits + CCXNE_SHIFT_BITS));
    reg_bitwise_write(pwm->timer_dev->regs.ccer, mask, value);

    return 0;
}

int pwm_config_mode(uint8_t group, uint8_t channel, uint8_t mode)
{
    int ret = 0;

    if (mode != PWM_MODE) {
        switch (mode) {
        case PWM_BOTH_LOW_MODE:
            ret = pwm_set_idle_status(group, channel, LOW_LEVEL, LOW_LEVEL);
            break;
        case PWM_DC_LOW_MODE:
            ret = pwm_set_idle_status(group, channel, LOW_LEVEL, HIGH_LEVEL);
            break;
        case PWM_DC_HIGH_MODE:
            ret = pwm_set_idle_status(group, channel, HIGH_LEVEL, LOW_LEVEL);
            break;
        }

        if (ret < 0)
            configASSERT(0);

        return pwm_channel_enable(group, channel, false);
    }

    return pwm_channel_enable(group, channel, true);
}

int pwm_set_duty(uint8_t group, float duty_percent)
{
    struct pwm_t *pwm = &support_pwm[group];

    int chn = 0;

    duty_percent = duty_percent < MAX_PWM_DUTY ? duty_percent : MAX_PWM_DUTY;
    for (chn =0; chn < MAX_PWM_CHANNEL; chn++) {
        pwm->duty_count[chn] = (uint16_t)(pwm->freq_count * duty_percent);
        pwm_update_duty_count(pwm, chn, pwm->duty_count[chn]);
    }

    return 0;
}

int pwm_bind_timer(uint8_t group, uint8_t timer_id, uint32_t freq_hz)
{
    struct pwm_t *pwm = NULL;
	struct timer_device *timer_dev = NULL;

    printf("%s\n", __func__);

    configASSERT(group < MAX_PWM_GROUP);
    configASSERT(timer_id < TIMER_MAX);

	pwm = &support_pwm[group];
	pwm->timer_dev = request_timer_device(timer_id);
    configASSERT(pwm->timer_dev);

    timer_dev = pwm->timer_dev;

    printf("%s pwm %p\n", __func__, pwm);

    pwm->freq_count = TIMER_COUNTER_BASE_FREQ_HZ / freq_hz;
    timer_dev->irq_enable = true;
    timer_dev->irq_handler = pwm_irq_handler;
    timer_dev->private_data = pwm_group_to_ptr(group);
    timer_dev->irq_request_type = TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3;
    return timer_init(pwm->timer_dev);
}

int pwm_bind_pin(uint8_t group, uint8_t channel,
        enum chip_pin postive, enum chip_pin negatie)
{
    struct pwm_t *pwm = &support_pwm[group];
    struct timer_device *timer_dev = pwm->timer_dev;
    uint8_t af_mode = 0, mode = 0;

    af_mode = get_timer_gpio_alter_mode(timer_dev->id);
    mode = MODE_ATTR(af_mode, MODE_ALTER);
    printf("%s %u %u %d %d %u %u\n", __func__, group, channel, postive, negatie,
        mode, af_mode);
    if (postive)
        gpio_set_mode(postive, mode);
    if (negatie)
        gpio_set_mode(postive, mode);

    return 0;
}

static int pwm_setup(uint8_t group, uint8_t channel)
{
    struct pwm_t *pwm = &support_pwm[group];
    struct timer_device *timer_dev = pwm->timer_dev;
    uint8_t shift_bits = 0;
    uint8_t ccmr_sel = 0;
    uint16_t mask = 0, value = 0;

    ccmr_sel = channel >= PWM_CHANNEL3 ? 1 : 0;
    shift_bits = channel >= PWM_CHANNEL3 ? (channel - PWM_CHANNEL3)
        * CCMR_CH_SHIFT_BITS : channel * CCMR_CH_SHIFT_BITS;
    mask = ((OCXFE_MASK << (shift_bits + OCXFE_SHIFT_BITS))
        | (OCXPE_MASK << (shift_bits + OCXPE_SHIFT_BITS))
        | (OCXM_MASK << (shift_bits + OCXM_SHIFT_BITS)));
    value = ((OCXFE_FAST_ENABLE << (shift_bits + ICXF_SHIFT_BITS))
        | (OCXPE_DISABLE << (shift_bits + ICXPSC_SHIFT_BITS))
        | (OCXM_TOGGLE << (shift_bits + CCXS_SHIFT_BITS)));
    reg_bitwise_write(timer_dev->regs.ccmr[ccmr_sel], mask, value);

    shift_bits = CCER_CH_SHIFT_BITS * channel;
    mask = (CCXP_MASK << (shift_bits + CCXP_SHIFT_BITS))
        | (CCXNP_MASK << (shift_bits + CCXNP_SHIFT_BITS));
    value = (CCXP_ACTIVE_HIGH << (shift_bits + CCXP_SHIFT_BITS))
        | (CCXNP_ACTIVE_LOW << (shift_bits + CCXNP_SHIFT_BITS));
    reg_bitwise_write(timer_dev->regs.ccer, mask, value);

    mask = (CCXE_MASK << (shift_bits + CCXE_SHIFT_BITS))
        | (CCXNE_MASK << (shift_bits + CCXNE_SHIFT_BITS));
    value = (CCXE_DISABLE << (shift_bits + CCXE_SHIFT_BITS))
        | (CCXNE_DISABLE << (shift_bits + CCXNE_SHIFT_BITS));  
    reg_bitwise_write(timer_dev->regs.ccer, mask, value);

    return 0;
}

int pwm_init(uint8_t group, uint8_t channel)
{
    int ret = 0;
    struct pwm_t *pwm = &support_pwm[group];

    configASSERT(group < MAX_PWM_GROUP);
    configASSERT(channel < MAX_PWM_CHANNEL);

    pwm->enabled = false;

    ret = pwm_setup(group, channel);
    if (ret < 0)
        return ret;

    ret = pwm_set_idle_status(group, channel, LOW_LEVEL, LOW_LEVEL);
    if (ret < 0)
        return ret;

    return pwm_enable(group, false);
}

int pwm_deinit(uint8_t group, uint8_t channel)
{
    return 0;
}

