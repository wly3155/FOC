
#ifndef __ST_GPIO_H__
#define __ST_GPIO_H__

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

#include <stdint.h>
#include <st/st_board.h>

#ifdef __cplusplus
extern "c" {
#endif

#define MODE_ATTR(alter, basic)        ((alter << 4) | basic)

enum {
    GPIO_ALTER_0, //system
    GPIO_ALTER_1, //tim1 ~ tim2
    GPIO_ALTER_2, //tim3 ~ tim5
    GPIO_ALTER_3, //tim8 ~ tim11
    GPIO_ALTER_4, //i2c1 ~ i2c3
    GPIO_ALTER_5, //spi1 ~ spi2
    GPIO_ALTER_6, //spi3
    GPIO_ALTER_7, //usart1 ~ usart3
    GPIO_ALTER_8, //usart4 ~ usart6
    GPIO_ALTER_9, //can1 ~ can2, tim12 ~ tim14
    GPIO_ALTER_10, //otg_fs, otg_hs
    GPIO_ALTER_11, //eth
    GPIO_ALTER_12, //fsmc, sdio, otg_hs
    GPIO_ALTER_13, //dcmi
    GPIO_ALTER_14, //none
    GPIO_ALTER_15, //event_out
    GPIO_ALTER_INVALID,
};

enum {
    MODE_INPUT,
    MODE_OUTPUT,
    MODE_ALTER,
    MODE_ANALOG,
};

enum {
    OUTPUT_LOW,
    OUTPUT_HIGH,
};

int gpio_set_output_type(enum chip_pin pin, uint8_t out_type);
int gpio_set_output_speed(enum chip_pin pin, uint8_t speed);
int gpio_set_output_pupd(enum chip_pin pin, uint8_t pull_updown);
int gpio_set_mode(enum chip_pin pin, uint8_t mode);
int gpio_set_output_value(enum chip_pin pin, uint8_t val);
int gpio_platform_init(void);

#ifdef __cplusplus
}
#endif
#endif