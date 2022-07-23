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
#include <stddef.h>
#include <string.h>

#include <st/st_reg.h>
#include <st/st_board.h>
#include <st/st_gpio.h>
#include <st/st_timer.h>
#include <st/st_pwm.h>

#include "FreeRTOS.h"
#include "stm32f4xx.h"
#include "stm32f4xx_tim.h"

struct pwm_t {
	bool timer_inited;
	uint8_t timer_id;
	struct timer_device *timer_dev;
	uint32_t bind_pin[MAX_CHANNEL];
	int (*irq_handler)(void *private_data);
	void *private_data;
};

struct pwm_t support_pwm[MAX_PWM_GROUP];

static int chip_pin_to_af_mode(uint32_t pin)
{
	switch (pin) {
	case CHIP_PIN_23:
	case CHIP_PIN_26:
	case CHIP_PIN_27:
	case CHIP_PIN_34:
	case CHIP_PIN_35:
	case CHIP_PIN_36:
		return GPIO_ALTER_1;
	case CHIP_PIN_37:
	case CHIP_PIN_38:
	case CHIP_PIN_39:
	case CHIP_PIN_41:
	case CHIP_PIN_42:
	case CHIP_PIN_43:
		return GPIO_ALTER_3;
	}

	return -EINVAL;
}

int pwm_set_duty(TIM_TypeDef *timer, enum pwm_channel channel, float duty_percent, bool dual_polarity)
{
	TIM_OCInitTypeDef TIM_OCInitStructure;

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_Pulse = (uint32_t)(duty_percent * timer->ARR);
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;

	if (is_advance_timer(timer)) {
		if (dual_polarity) {
			TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
			TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
			TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
		}
	}

	switch (channel) {
	case PWM_CHANNEL1:
		TIM_OC1Init(timer, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(timer, TIM_OCPreload_Enable);
		break;
	case PWM_CHANNEL2:
		TIM_OC2Init(timer, &TIM_OCInitStructure);
		TIM_OC2PreloadConfig(timer, TIM_OCPreload_Enable);
		break;
	case PWM_CHANNEL3:
		TIM_OC3Init(timer, &TIM_OCInitStructure);
		TIM_OC3PreloadConfig(timer, TIM_OCPreload_Enable);
		break;
	case PWM_CHANNEL4:
		TIM_OC4Init(timer, &TIM_OCInitStructure);
		TIM_OC4PreloadConfig(timer, TIM_OCPreload_Enable);
		break;
	default:
		break;
	}

	return 0;
}

int pwm_set_complementary_output(TIM_TypeDef *timer, enum pwm_channel channel,
	bool pos_value, bool neg_value)
{
	uint8_t ois_shift_bit = 0;
	uint8_t ois_value = 0;

	if (!is_advance_timer(timer))
		return -EINVAL;

	assert_param(pos_value != neg_value);

	ois_shift_bit = OIS1_SHIFT_BIT + (channel - 1) * 2;
	ois_value = neg_value << 1 | pos_value;
	reg_bitwise_write(timer->CR2, 0x3 << ois_shift_bit, ois_value << ois_shift_bit);
	return 0;
}

int pwm_enabledisable(TIM_TypeDef *timer, bool en)
{
	TIM_ARRPreloadConfig(timer, en);
	TIM_Cmd(timer, en);
	return 0;
}

int pwm_bind_timer(uint8_t group, uint8_t timer_id)
{
	struct pwm_t *pwm = NULL;

	configASSERT(group < MAX_PWM_GROUP);
	configASSERT(timer_id < TIMER_MAX);
	pwm = &support_pwm[group];
	pwm->timer_id = timer_id;
	return 0;
}

int pwm_bind_pin(uint8_t group, uint8_t channel, enum chip_pin pin)
{
	struct pwm_t *pwm = NULL;

	configASSERT(group < MAX_PWM_GROUP);
	configASSERT(channel < MAX_PWM_CHANNEL);
	pwm = &support_pwm[group];
	pwm->bind_pin[channel] = pin;
	return 0;
}

int pwm_init(uint8_t group, uint8_t channel)
{
	struct pwm_t *pwm = NULL;
	int af_mode = 0;

	configASSERT(group < MAX_PWM_GROUP);
	configASSERT(channel < MAX_PWM_CHANNEL);
	pwm = &support_pwm[group];
	af_mode = chip_pin_to_af_mode(pwm->bind_pin[channel]);
	return gpio_set_mode(pwm->bind_pin[channel], MODE_ATTR(af_mode, MODE_ALTER));
}

int pwm_deinit(uint8_t group, uint8_t channel)
{
	return 0;
}
