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

#ifndef __ST_PWM_H__
#define __ST_PWM_H__

#include <stdbool.h>
#include <stdint.h>
#include <st/st_board.h>
#include <stm32f4xx_tim.h>

#ifdef __cplusplus
extern "c" {
#endif

#define CLOCK_COUNT_FREQ            (21000000)

enum pwm_group {
    PWM_GROUP0,
    PWM_GROUP1,
    MAX_PWM_GROUP,
};

enum pwm_channel {
    PWM_CHANNEL1,
    PWM_CHANNEL2,
    PWM_CHANNEL3,
    PWM_CHANNEL4,
    MAX_PWM_CHANNEL,
};

enum {
    PWM_BOTH_LOW_MODE,
    PWM_DC_HIGH_MODE,
    PWM_DC_LOW_MODE,
    PWM_MODE,
};

int pwm_set_duty(uint8_t group, float duty_percent);
int pwm_config_mode(uint8_t group, uint8_t channel, uint8_t mode);
int pwm_bind_timer(uint8_t group, uint8_t timer_id, uint32_t freq_hz);
int pwm_bind_pin(uint8_t group, uint8_t channel,
    enum chip_pin postive, enum chip_pin negatie);
int pwm_init(uint8_t group, uint8_t channel);
int pwm_deinit(uint8_t group, uint8_t channel);
#ifdef __cplusplus
}
#endif
#endif
