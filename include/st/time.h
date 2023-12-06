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

#ifndef __ST_TIME_H__
#define __ST_TIME_H__

#include "utils.h"
#ifdef __cplusplus
extern "c" {
#endif

void udelay(uint32_t delay_us);
#define mdelay(x) udelay(ms_to_us(x))
uint64_t get_boot_time_ns(void);
int time_platform_init(void);

#ifdef __cplusplus
}
#endif
#endif
