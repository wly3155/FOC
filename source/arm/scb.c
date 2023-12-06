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

#include "scb.h"

struct scb_regs {
        uint32_t actlr;
        uint32_t cupid;
        uint32_t icsr;
        uint32_t vtor;
        uint32_t aircr;
        uint32_t scr;
        uint32_t ccr;
        uint32_t shpr1;
        uint32_t shpr2;
        uint32_t shpr3;
        uint32_t shcsr;
        uint32_t cfsr;
        uint32_t hfsr;
        uint32_t reserved;
        uint32_t mmar;
        uint32_t bfar;
        uint32_t afsr;
};
