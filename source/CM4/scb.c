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

#include "reg_ops.h"
#include "cm4/scb.h"

int scb_platform_init(void)
{
        reg_bitwise_write32(SCB_SHCSR_ADDR, 7<<16, 7<<16);
        reg_bitwise_write32(SCB_CCR_ADDR, 1<<4, 1<<4);
        return 0;
}
