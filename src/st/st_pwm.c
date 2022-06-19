#include <errno.h>
#include <stddef.h>
#include <string.h>

#include <st/st_reg.h>
#include <st/st_board.h>
#include <st/st_gpio.h>
#include <st/st_timer.h>
#include <st/st_pwm.h>

#include "stm32f4xx.h"
#include "stm32f4xx_tim.h"

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
	case channel_1:
		TIM_OC1Init(timer, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(timer, TIM_OCPreload_Enable);
		break;
	case channel_2:
		TIM_OC2Init(timer, &TIM_OCInitStructure);
		TIM_OC2PreloadConfig(timer, TIM_OCPreload_Enable);
		break;
	case channel_3:
		TIM_OC3Init(timer, &TIM_OCInitStructure);
		TIM_OC3PreloadConfig(timer, TIM_OCPreload_Enable);
		break;
	case channel_4:
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

int pwm_init(enum chip_pin pin)
{
	int af_mode = chip_pin_to_af_mode(pin);

	return gpio_set_mode(pin, MODE_ATTR(af_mode, MODE_ALTER));
}

int pwm_deinit(enum chip_pin pin)
{
	return 0;
}
