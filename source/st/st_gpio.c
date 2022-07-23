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
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include <st/st_board.h>
#include <st/st_reg.h>
#include <st/st_utils.h>
#include <st/st_gpio.h>

#include "stm32f4xx_rcc.h"

#define GET_BASIC_MODE(mode)                (mode & 0x0f)
#define GET_ALTER_MODE(mode)                ((mode & 0xf0) >> 4)

enum {
    GPIO_GROUP_A,
    GPIO_GROUP_B,
    GPIO_GROUP_C,
    GPIO_GROUP_MAX,
};

enum {
    GPIO_POS_0,
    GPIO_POS_1,
    GPIO_POS_2,
    GPIO_POS_3,
    GPIO_POS_4,
    GPIO_POS_5,
    GPIO_POS_6,
    GPIO_POS_7,
    GPIO_POS_8,
    GPIO_POS_9,
    GPIO_POS_10,
    GPIO_POS_11,
    GPIO_POS_12,
    GPIO_POS_13,
    GPIO_POS_14,
    GPIO_POS_15,
    GPIO_POS_MAX,
};

struct gpio_reg_t {
    uint32_t base;
    uint32_t mode;
    uint32_t output_type;
    uint32_t output_speed;
    uint32_t pull_updown;
    uint32_t input_value;
    uint32_t output_value;
    uint32_t alter[2];
};

struct chip_pin_t {
    uint32_t chip_pin;
    uint32_t gpio_pos;
    struct gpio_device_t *device;
    bool init_done;
};

struct gpio_device_t {
    struct gpio_reg_t reg;
    uint32_t periph_clock;
    bool clock_enable;
};

static struct gpio_device_t support_gpio_list[GPIO_GROUP_MAX];
static struct chip_pin_t support_chip_pin_list[] = {
    {
        .chip_pin = CHIP_PIN_14,
        .gpio_pos = GPIO_POS_0,
        .device = &support_gpio_list[GPIO_GROUP_A],
    },
    {
        .chip_pin = CHIP_PIN_15,
        .gpio_pos = GPIO_POS_1,
        .device = &support_gpio_list[GPIO_GROUP_A],
    },
    {
        .chip_pin = CHIP_PIN_16,
        .gpio_pos = GPIO_POS_2,
        .device = &support_gpio_list[GPIO_GROUP_A],
    },
    {
        .chip_pin = CHIP_PIN_17,
        .gpio_pos = GPIO_POS_3,
        .device = &support_gpio_list[GPIO_GROUP_A],
    },
    {
        .chip_pin = CHIP_PIN_23,
        .gpio_pos = GPIO_POS_7,
        .device = &support_gpio_list[GPIO_GROUP_A],
    },
    {
        .chip_pin = CHIP_PIN_26,
        .gpio_pos = GPIO_POS_0,
        .device = &support_gpio_list[GPIO_GROUP_B],
    },
    {
        .chip_pin = CHIP_PIN_27,
        .gpio_pos = GPIO_POS_1,
        .device = &support_gpio_list[GPIO_GROUP_B],
    },
    {
        .chip_pin = CHIP_PIN_33,
        .gpio_pos = GPIO_POS_12,
        .device = &support_gpio_list[GPIO_GROUP_B],
    },
    {
        .chip_pin = CHIP_PIN_34,
        .gpio_pos = GPIO_POS_13,
        .device = &support_gpio_list[GPIO_GROUP_B],
    },
    {
        .chip_pin = CHIP_PIN_35,
        .gpio_pos = GPIO_POS_14,
        .device = &support_gpio_list[GPIO_GROUP_B],
    },
    {
        .chip_pin = CHIP_PIN_36,
        .gpio_pos = GPIO_POS_15,
        .device = &support_gpio_list[GPIO_GROUP_B],
    },
    {
        .chip_pin = CHIP_PIN_37,
        .gpio_pos = GPIO_POS_6,
        .device = &support_gpio_list[GPIO_GROUP_C],
    },
    {
        .chip_pin = CHIP_PIN_38,
        .gpio_pos = GPIO_POS_7,
        .device = &support_gpio_list[GPIO_GROUP_C],
    },
    {
        .chip_pin = CHIP_PIN_39,
        .gpio_pos = GPIO_POS_8,
        .device = &support_gpio_list[GPIO_GROUP_C],
    },
    {
        .chip_pin = CHIP_PIN_40,
        .gpio_pos = GPIO_POS_9,
        .device = &support_gpio_list[GPIO_GROUP_C],
    },
    {
        .chip_pin = CHIP_PIN_41,
        .gpio_pos = GPIO_POS_8,
        .device = &support_gpio_list[GPIO_GROUP_A],
    },
    {
        .chip_pin = CHIP_PIN_42,
        .gpio_pos = GPIO_POS_9,
        .device = &support_gpio_list[GPIO_GROUP_A],
    },
    {
        .chip_pin = CHIP_PIN_43,
        .gpio_pos = GPIO_POS_10,
        .device = &support_gpio_list[GPIO_GROUP_A],
    },
    {
        .chip_pin = CHIP_PIN_58,
        .gpio_pos = GPIO_POS_6,
        .device = &support_gpio_list[GPIO_GROUP_B],
    },
    {
        .chip_pin = CHIP_PIN_59,
        .gpio_pos = GPIO_POS_7,
        .device = &support_gpio_list[GPIO_GROUP_B],
    },
};

static struct chip_pin_t *get_chip_pin_info(enum chip_pin pin)
{
    uint32_t i = 0;
    struct chip_pin_t *pin_info = NULL;

    for (i = 0; i < ARRAY_SIZE(support_chip_pin_list); i++) {
        pin_info = &support_chip_pin_list[i];
        if (pin_info->chip_pin == pin)
            return pin_info;
    }

    return NULL;
}

int gpio_set_output_type(enum chip_pin pin, uint8_t out_type)
{
    struct chip_pin_t *pin_info = NULL;
    struct gpio_device_t *dev = NULL;
    uint8_t shift_bits = 0;

    pin_info = get_chip_pin_info(pin);
    configASSERT(pin_info);

    dev = pin_info->device;
    if (!dev->clock_enable)
        RCC_AHB1PeriphClockCmd(dev->periph_clock, true);

    shift_bits = pin_info->gpio_pos * OUT_TYPE_SHIFT_BITS;
    reg_bitwise_write(dev->reg.pull_updown, OUT_TYPE_MASK << shift_bits, out_type << shift_bits);
    return 0;
}

int gpio_set_output_speed(enum chip_pin pin, uint8_t speed)
{
    struct chip_pin_t *pin_info = NULL;
    struct gpio_device_t *dev = NULL;
    uint8_t shift_bits = 0;

    pin_info = get_chip_pin_info(pin);
    configASSERT(pin_info);

    dev = pin_info->device;
    if (!dev->clock_enable)
        RCC_AHB1PeriphClockCmd(dev->periph_clock, true);

    shift_bits = pin_info->gpio_pos * OUT_SPEED_SHIFT_BITS;
    reg_bitwise_write(dev->reg.pull_updown, OUT_SPEED_MASK << shift_bits, speed << shift_bits);
    return 0;
}

int gpio_set_output_pupd(enum chip_pin pin, uint8_t pull_updown)
{
    struct chip_pin_t *pin_info = NULL;
    struct gpio_device_t *dev = NULL;
    uint8_t shift_bits = 0;

    pin_info = get_chip_pin_info(pin);
    configASSERT(pin_info);

    dev = pin_info->device;
    if (!dev->clock_enable)
        RCC_AHB1PeriphClockCmd(dev->periph_clock, true);

    shift_bits = pin_info->gpio_pos * PULL_UPDOWN_SHIFT_BITS;
    reg_bitwise_write(dev->reg.pull_updown, PULL_UPDOWN_MASK << shift_bits, pull_updown << shift_bits);
    return 0;
}

int gpio_set_mode(enum chip_pin pin, uint8_t mode)
{
    struct chip_pin_t *pin_info = NULL;
    struct gpio_device_t *dev = NULL;
    uint8_t basic_mode = 0;
    uint8_t alter_mode = 0;
    uint8_t shift_bits = 0;

    pin_info = get_chip_pin_info(pin);
    configASSERT(pin_info);

    dev = pin_info->device;
    if (!dev->clock_enable) {
        RCC_AHB1PeriphClockCmd(dev->periph_clock, true);
        dev->clock_enable = true;
    }

    basic_mode = GET_BASIC_MODE(mode);
    shift_bits = pin_info->gpio_pos * MODE_SHIFT_BITS;
    reg_bitwise_write(dev->reg.mode, MODE_MASK << shift_bits, basic_mode << shift_bits);
    if (pin_info->init_done)
        return 0;

    if (basic_mode == MODE_OUTPUT || basic_mode == MODE_ALTER) {
        shift_bits = pin_info->gpio_pos * OUT_TYPE_SHIFT_BITS;
        reg_bitwise_write(dev->reg.output_type, OUT_TYPE_MASK << shift_bits, OUT_TYPE_DEFAULT << shift_bits);

        shift_bits = pin_info->gpio_pos * OUT_SPEED_SHIFT_BITS;
        reg_bitwise_write(dev->reg.output_speed, OUT_SPEED_MASK << shift_bits, OUT_SPEED_DEFAULT << shift_bits);

        shift_bits = pin_info->gpio_pos * PULL_UPDOWN_SHIFT_BITS;
        reg_bitwise_write(dev->reg.pull_updown, PULL_UPDOWN_MASK << shift_bits, PULL_UPDOWN_DEFAULT << shift_bits);
    }

    if (basic_mode == MODE_ALTER) {
        alter_mode = GET_ALTER_MODE(mode);
        shift_bits = (pin_info->gpio_pos % ALTER_MODE_PORT_BITS)* ALTER_MODE_SHIFT_BITS;
        reg_bitwise_write(dev->reg.alter[pin_info->gpio_pos / ALTER_MODE_PORT_BITS],
            ALTER_MODE_MASK << shift_bits, alter_mode << shift_bits);
    }

    pin_info->init_done = true;
    return 0;
}

int gpio_set_output_value(enum chip_pin pin, uint8_t val)
{
    struct chip_pin_t *pin_info = NULL;
    struct gpio_device_t *dev = NULL;
    uint8_t shift_bits = 0;

    pin_info = get_chip_pin_info(pin);
    configASSERT(pin_info);

    dev = pin_info->device;
    if (!dev->clock_enable)
        RCC_AHB1PeriphClockCmd(dev->periph_clock, true);

    shift_bits = pin_info->gpio_pos * OUT_VALUE_SHIFT_BITS;
    reg_bitwise_write(dev->reg.output_value, OUT_VALUE_MASK << shift_bits, val << shift_bits);
    return 0;
}

int gpio_platform_init(void)
{
    int i = 0;
    struct gpio_device_t *gpio_dev = NULL;
    struct gpio_reg_t *gpio_reg = NULL;

    for (i = 0; i < GPIO_GROUP_MAX; i++) {
        gpio_dev = &support_gpio_list[i];
        gpio_reg = &gpio_dev->reg;
        gpio_reg->base = GPIO_BASE_REG + i * GPIOx_GROUP_BASE_OFFSET;
        gpio_reg->mode = gpio_reg->base + GPIOx_MODE_OFFSET;
        gpio_reg->output_type = gpio_reg->base + GPIOx_OUT_TYPE_OFFSET;
        gpio_reg->output_speed = gpio_reg->base + GPIOx_OUT_SPEED_OFFSET;
        gpio_reg->pull_updown = gpio_reg->base + GPIOx_PULL_UPDOWN_OFFSET;
        gpio_reg->input_value = gpio_reg->base + GPIOx_IN_DATA_OFFSET;
        gpio_reg->output_value = gpio_reg->base + GPIOx_OUT_DATA_OFFSET;
        gpio_reg->alter[0] = gpio_reg->base + GPIOx_ALTER_LOW_OFFSET;
        gpio_reg->alter[1] = gpio_reg->base + GPIOx_ALTER_HIGH_OFFSET;

        gpio_dev->clock_enable = false;
        gpio_dev->periph_clock = i >= 1 ? PERIPH_CLOCK_BIT_SHIFT << (i -1): 1;
    }

    return 0;
}
