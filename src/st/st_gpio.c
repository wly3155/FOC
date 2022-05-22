#include <errno.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <st/st_board.h>
#include <st/st_gpio.h>

#include "stm32f4xx_rcc.h"

static inline GPIO_TypeDef * chip_pin_to_gpio_group(uint32_t pin)
{
	switch (pin) {
	case CHIP_PIN_23:
	case CHIP_PIN_41:
	case CHIP_PIN_42:
	case CHIP_PIN_43:
		return GPIOA;
	case CHIP_PIN_26:
	case CHIP_PIN_27:
	case CHIP_PIN_33:
	case CHIP_PIN_34:
	case CHIP_PIN_35:
	case CHIP_PIN_36:
	case CHIP_PIN_58:
	case CHIP_PIN_59:
		return GPIOB;
	case CHIP_PIN_37:
	case CHIP_PIN_38:
	case CHIP_PIN_39:
	case CHIP_PIN_40:
		return GPIOC;
	}

	return NULL;
}

static inline int chip_pin_to_periph_group(uint32_t pin)
{
	switch (pin) {
	case CHIP_PIN_23:
	case CHIP_PIN_41:
	case CHIP_PIN_42:
	case CHIP_PIN_43:
		return RCC_AHB1Periph_GPIOA;
	case CHIP_PIN_26:
	case CHIP_PIN_27:
	case CHIP_PIN_33:
	case CHIP_PIN_34:
	case CHIP_PIN_35:
	case CHIP_PIN_36:
	case CHIP_PIN_58:
	case CHIP_PIN_59:
		return RCC_AHB1Periph_GPIOB;
	case CHIP_PIN_37:
	case CHIP_PIN_38:
	case CHIP_PIN_39:
	case CHIP_PIN_40:
		return RCC_AHB1Periph_GPIOC;
	}

	return -EINVAL;
}

static inline int chip_pin_to_gpio_pin(uint32_t pin)
{
	switch (pin) {
	case CHIP_PIN_26:
		return GPIO_Pin_0;
	case CHIP_PIN_27:
		return GPIO_Pin_1;
	case CHIP_PIN_37:
	case CHIP_PIN_58:
		return GPIO_Pin_6;
	case CHIP_PIN_23:
	case CHIP_PIN_38:
	case CHIP_PIN_59:
		return GPIO_Pin_7;
	case CHIP_PIN_39:
	case CHIP_PIN_41:
		return GPIO_Pin_8;
	case CHIP_PIN_40:
	case CHIP_PIN_42:
		return GPIO_Pin_9;
	case CHIP_PIN_43:
		return GPIO_Pin_10;
	case CHIP_PIN_33:
		return GPIO_Pin_12;
	case CHIP_PIN_34:
		return GPIO_Pin_13;
	case CHIP_PIN_35:
		return GPIO_Pin_14;
	case CHIP_PIN_36:
		return GPIO_Pin_15;

	}

	return -EINVAL;
}

static inline int gpio_pin_to_resoure(uint32_t pin)
{
	switch (pin) {
	case GPIO_Pin_0:
		return GPIO_PinSource0;
	case GPIO_Pin_1:
		return GPIO_PinSource1;
	case GPIO_Pin_2:
		return GPIO_PinSource2;
	case GPIO_Pin_3:
		return GPIO_PinSource3;
	case GPIO_Pin_4:
		return GPIO_PinSource4;
	case GPIO_Pin_5:
		return GPIO_PinSource5;
	case GPIO_Pin_6:
		return GPIO_PinSource6;
	case GPIO_Pin_7:
		return GPIO_PinSource7;
	case GPIO_Pin_8:
		return GPIO_PinSource8;
	case GPIO_Pin_9:
		return GPIO_PinSource9;
	case GPIO_Pin_10:
		return GPIO_PinSource10;
	case GPIO_Pin_11:
		return GPIO_PinSource11;
	case GPIO_Pin_12:
		return GPIO_PinSource12;
	case GPIO_Pin_13:
		return GPIO_PinSource13;
	case GPIO_Pin_14:
		return GPIO_PinSource14;
	case GPIO_Pin_15:
		return GPIO_PinSource15;
	}
	return -EINVAL;
}

static inline uint8_t gpio_pin_to_pos(uint32_t gpio_pin)
{
    uint8_t pos = -1;
	uint32_t tmp = gpio_pin;

	while (tmp) {
		tmp = tmp >> 2;
		pos++; 
	}

	return pos;
}

static bool is_output_or_af_mode(GPIO_TypeDef *gpio_group, uint32_t gpio_pin)
{
	uint8_t pos = gpio_pin_to_pos(gpio_pin);
	return ((gpio_group->MODER >> (pos * 2) & GPIO_MODER_MODER0) >> 2) ? true : false;
}

static uint32_t gpio_af_mode_to_periph_clock(int af_mode)
{
	switch (af_mode) {
	case GPIO_AF_TIM1:
		return RCC_APB2Periph_TIM1;
	case GPIO_AF_TIM8:
		return RCC_APB2Periph_TIM8;
	}

	return -EINVAL;
}

#define IS_RCC_AHB1_PERIPH(PERIPH)			((((PERIPH) & 0x010BE800) == 0x00) && ((PERIPH) != 0x00))
#define IS_RCC_AHB2_PERIPH(PERIPH)			((((PERIPH) & 0xFFFFFF0E) == 0x00) && ((PERIPH) != 0x00))

static int gpio_af_mode_clock_enabledisable(int af_mode, bool enable)
{
	uint32_t periph_clock = gpio_af_mode_to_periph_clock(af_mode);

	if (IS_RCC_AHB1_PERIPH(periph_clock)) {
		RCC_AHB1PeriphClockCmd(periph_clock, enable);
	} else if (IS_RCC_AHB2_PERIPH(periph_clock)) {
		RCC_AHB2PeriphClockCmd(periph_clock, enable);
	}
	return 0;
}

int gpio_af_mode_clock_enable(int af_mode, bool enable)
{
	return gpio_af_mode_clock_enabledisable(af_mode, true);
}

int gpio_af_mode_clock_disble(int af_mode, bool enable)
{
	return gpio_af_mode_clock_enabledisable(af_mode, false);
}

int gpio_set_output_value(enum chip_pin pin, BitAction val)
{
	GPIO_TypeDef *gpio_group = NULL;
	int gpio_pin = 0;

	gpio_group = chip_pin_to_gpio_group(pin);
	if (!gpio_group)
		return -EINVAL;

	gpio_pin = chip_pin_to_gpio_pin(pin);
	if (gpio_pin < 0)
		return gpio_pin;

	GPIO_WriteBit(gpio_group, gpio_pin, val);
	return 0;
}

int gpio_set_mode(enum chip_pin pin, GPIOMode_TypeDef mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_TypeDef *gpio_group = NULL;
	int periph_group = 0;
	int gpio_pin = 0;

	memset(&GPIO_InitStructure, 0x00, sizeof(GPIO_InitStructure));
	periph_group = chip_pin_to_periph_group(pin);
	if (periph_group < 0)
		return periph_group;

	gpio_group = chip_pin_to_gpio_group(pin);
	if (!gpio_group)
		return -EINVAL;

	gpio_pin = chip_pin_to_gpio_pin(pin);
	if (gpio_pin < 0) {
		return gpio_pin;
	}

	RCC_AHB1PeriphClockCmd(periph_group, ENABLE);
	GPIO_InitStructure.GPIO_Pin = gpio_pin;
	GPIO_InitStructure.GPIO_Mode = mode;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(gpio_group, &GPIO_InitStructure);
	return 0;
}

int gpio_set_af_mode(enum chip_pin pin, uint8_t mode)
{
	GPIO_TypeDef *gpio_group = NULL;
	int gpio_pin = 0;
	int gpio_resource = 0;

	gpio_group = chip_pin_to_gpio_group(pin);
	if (!gpio_group)
		return -EINVAL;

	gpio_pin = chip_pin_to_gpio_pin(pin);
	if (gpio_pin < 0)
		return gpio_pin;

	gpio_resource = gpio_pin_to_resoure(gpio_pin);
	if (gpio_resource < 0)
		return gpio_resource;

	GPIO_PinAFConfig(gpio_group, gpio_resource, mode);
	return 0;
}

int gpio_set_output_type(enum chip_pin pin, GPIOOType_TypeDef otype)
{
	GPIO_TypeDef *gpio_group = NULL;
	int gpio_pin = 0;
	uint8_t pos = 0;

	if (!is_output_or_af_mode(gpio_group, gpio_pin))
		return 0;

	gpio_group = chip_pin_to_gpio_group(pin);
	if (!gpio_group)
		return -EINVAL;

	gpio_pin = chip_pin_to_gpio_pin(pin);
	if (gpio_pin < 0)
		return gpio_pin;

	pos = gpio_pin_to_pos(gpio_pin);
	gpio_group->OTYPER &= ~((GPIO_OTYPER_OT_0) << ((uint16_t)pos));
	gpio_group->OTYPER |= (uint16_t)(((uint16_t)otype) << ((uint16_t)pos));
	return 0;
}

int gpio_set_output_pupd(enum chip_pin pin, GPIOPuPd_TypeDef pupd)
{
	GPIO_TypeDef *gpio_group = NULL;
	int gpio_pin = 0;
	uint8_t pos = 0;

	gpio_group = chip_pin_to_gpio_group(pin);
	if (!gpio_group)
		return -EINVAL;

	gpio_pin = chip_pin_to_gpio_pin(pin);
	if (gpio_pin < 0)
		return gpio_pin;

	pos = gpio_pin_to_pos(gpio_pin);
	gpio_group->PUPDR  &= ~((GPIO_PUPDR_PUPDR0) << ((uint16_t)pos * 2));
	gpio_group->PUPDR |= (uint16_t)(((uint16_t)pupd) << ((uint16_t)pos * 2));
	return 0;
}