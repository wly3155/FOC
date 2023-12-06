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

#ifndef __ST_TIMER_H__
#define __ST_TIMER_H__

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "c" {
#endif

void timer_register(uint32_t period_ns,
        int (*timer_handler)(void *private_data), void *private_data);
int timer_enable(bool enable);
int timer_platform_init(void);

#ifdef __cplusplus
}
#endif
#endif
