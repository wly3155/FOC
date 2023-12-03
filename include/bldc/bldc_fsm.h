/*
 * Copyright (C) <2023>  <wuliyong3155@163.com>

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

#ifndef __BLDC_FSM_H__
#define __BLDC_FSM_H__

#include <stdinit.h>

#ifdef __cplusplus
extern "C" {
#endif

enum step_status {
	STATUS_A,
	STATUS_B,
	STATUS_C,
	STATUS_D,
	STATUS_E,
	STATUS_F,
};

struct bldc_step_fsm {
	uint8_t status;
	int status_func(void *private_data);
};

enum phase_encoder {
	encoder_idle,
	encoder_low,
	encoder_high,
	encoder_pwm,
};

int bldc_device_init(void);

#ifdef __cplusplus
}
#endif
#endif
