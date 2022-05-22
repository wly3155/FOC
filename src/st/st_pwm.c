#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <st/st_board.h>
#include <st/st_gpio.h>
#include <st/st_timer.h>
#include <st/st_pwm.h>

#include "stm32f4xx.h"
#include "stm32f4xx_tim.h"

static TIM_TypeDef* chip_pin_to_pwm_timer(uint32_t pin)
{
	switch (pin) {
	case CHIP_PIN_23:
	case CHIP_PIN_26:
	case CHIP_PIN_27:
	case CHIP_PIN_34:
	case CHIP_PIN_35:
	case CHIP_PIN_36:
		return TIM1;
	case CHIP_PIN_37:
	case CHIP_PIN_38:
	case CHIP_PIN_39:
	case CHIP_PIN_41:
	case CHIP_PIN_42:
	case CHIP_PIN_43:
		return TIM8;
	}

	return NULL;
}

static int chip_pin_to_af_mode(uint32_t pin)
{
	switch (pin) {
	case CHIP_PIN_23:
	case CHIP_PIN_26:
	case CHIP_PIN_27:
	case CHIP_PIN_34:
	case CHIP_PIN_35:
	case CHIP_PIN_36:
		return GPIO_AF_TIM1;
	case CHIP_PIN_37:
	case CHIP_PIN_38:
	case CHIP_PIN_39:
	case CHIP_PIN_41:
	case CHIP_PIN_42:
	case CHIP_PIN_43:
		return GPIO_AF_TIM8;
	}

	return -EINVAL;
}

static int chip_pin_to_pwm_channel(uint32_t pin)
{
	switch (pin) {
	case CHIP_PIN_23:
	case CHIP_PIN_34:
	case CHIP_PIN_37:
	case CHIP_PIN_41:
		return TIM_Channel_1;
	case CHIP_PIN_26:
	case CHIP_PIN_35:
	case CHIP_PIN_38:
	case CHIP_PIN_42:
		return TIM_Channel_2;
	case CHIP_PIN_27:
	case CHIP_PIN_36:
	case CHIP_PIN_39:
	case CHIP_PIN_43:
		return TIM_Channel_3;
	}

	return -EINVAL;
}

int pwm_set_freq(enum chip_pin pin, uint32_t freq_hz)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TypeDef *timer = NULL;
	uint16_t real_count_clock_freq = 0;
	uint16_t prescalerValue = 0;

	timer = chip_pin_to_pwm_timer(pin);
	if (!timer)
		return -EINVAL;

	memset(&TIM_TimeBaseStructure, 0x00, sizeof(TIM_TimeBaseStructure));
	prescalerValue = (uint16_t) ((SystemCoreClock /2) / CLOCK_COUNT_FREQ) - 1;
	real_count_clock_freq = (prescalerValue + 1) * CLOCK_COUNT_FREQ;
	TIM_TimeBaseStructure.TIM_Period = real_count_clock_freq / freq_hz - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = prescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(timer, &TIM_TimeBaseStructure);
	return 0;
}

int pwm_set_duty(enum chip_pin pin, float duty_percent, bool dual_polarity)
{
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_TypeDef *timer = NULL;
	uint8_t channel = 0;

	timer = chip_pin_to_pwm_timer(pin);
	if (!timer)
		return -EINVAL;

	channel = chip_pin_to_pwm_channel(pin);
	if (channel < 0)
		return channel;

	memset(&TIM_OCInitStructure, 0x00, sizeof(TIM_OCInitStructure));
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = (uint32_t)(duty_percent * timer->ARR);
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	if (is_advance_timer(timer)) {
		if (dual_polarity) {
			TIM_OCInitStructure.TIM_OutputNState = TIM_OutputState_Enable;
			TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_Low;
		}
	}

	switch (channel) {
	case TIM_Channel_1:
		TIM_OC1Init(timer, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(timer, TIM_OCPreload_Enable);
		break;
	case TIM_Channel_2:
		TIM_OC2Init(timer, &TIM_OCInitStructure);
		TIM_OC2PreloadConfig(timer, TIM_OCPreload_Enable);
		break;
	case TIM_Channel_3:
		TIM_OC3Init(timer, &TIM_OCInitStructure);
		TIM_OC3PreloadConfig(timer, TIM_OCPreload_Enable);
		break;
	case TIM_Channel_4:
		TIM_OC4Init(timer, &TIM_OCInitStructure);
		TIM_OC4PreloadConfig(timer, TIM_OCPreload_Enable);
		break;
	default:
		break;
	}
	return 0;
}

int pwm_enabledisable(enum chip_pin pin, bool en)
{
	TIM_TypeDef *timer = NULL;

	timer = chip_pin_to_pwm_timer(pin);
	if (!timer)
		return -EINVAL;
	TIM_ARRPreloadConfig(timer, en);
	TIM_Cmd(timer, en);
	return 0;
}

int pwm_init(enum chip_pin pin)
{
	int ret = 0;
	int af_mode = 0;

	ret = gpio_set_mode(pin, GPIO_Mode_AF);
	if (ret < 0)
		return ret;

	af_mode = chip_pin_to_af_mode(pin);
	if (af_mode < 0)
		return af_mode;

	ret = gpio_set_af_mode(pin, af_mode);
	if (ret < 0)
		return ret;

	return timer_clock_enable(af_mode, true);
}

int pwm_deinit(enum chip_pin pin)
{
	int af_mode = 0;

	af_mode = chip_pin_to_af_mode(pin);
	if (af_mode < 0)
		return af_mode;

	return timer_clock_enable(af_mode, false);
}
