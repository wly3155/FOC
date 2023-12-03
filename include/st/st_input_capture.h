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

#ifndef __ST_INPUT_CAPTURE_H__
#define __ST_INPUT_CAPTURE_H__

#include <stdint.h>
#include <st/st_board.h>

#ifdef __cplusplus
extern "c" {
#endif

enum {
    INPUT_CAPTURE_GROUP0,
    MAX_INPUT_CAPTURE_GROUP,
};

int input_capture_enable(uint8_t group, uint8_t channel);
int input_capture_disable(uint8_t group, uint8_t channel);
int input_capture_init(uint8_t group, uint8_t channel);
int input_capture_irq_register(uint8_t group, int (*irq_handler)(void *private_data), void *private_data);
int input_capture_bind_timer(uint8_t group, uint8_t timer_id);
int input_capture_bind_chip_pin(uint8_t group, uint8_t channel, enum chip_pin pin);

#ifdef __cplusplus
}
#endif
#endif