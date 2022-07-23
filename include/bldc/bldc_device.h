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

#ifndef __BLDC_DEVICE_H__
#define __BLDC_DEVICE_H__

#ifdef __cplusplus
extern "c" {
#endif

enum phase {
    PHASE_A,
    PHASE_B,
    PHASE_C,
    PHASE_MAX,
};

enum phase_encoder{
    encoder_low,
    encoder_high,
    encoder_pwm,
};

int bldc_device_init(void);

#ifdef __cplusplus
}
#endif
#endif
