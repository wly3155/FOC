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

#ifndef __ST_CLOCK_H__
#define __ST_CLOCK_H__

#ifdef __cplusplus
extern "c" {
#endif

extern uint32_t SystemCoreClock;
#define configAPB1_CLOCK_HZ		(SystemCoreClock >> 2)
#define configAPB2_CLOCK_HZ		(SystemCoreClock >> 1)

#define configAPB1_TIM_CLOCK_HZ		(configAPB1_CLOCK_HZ << 1)
#define configAPB2_TIM_CLOCK_HZ		(configAPB2_CLOCK_HZ << 1)

#ifdef __cplusplus
}
#endif
#endif
