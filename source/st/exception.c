/*
 * This file is licensed under the Apache License, Version 2.0.
 *
 * Copyright (c) 2023 wuliyong3155@163.com
 *
 * A copy of the license can be obtained at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "printf.h"
#include "reg_ops.h"
#include "utils.h"

#include "cm4/scb.h"
#include "st/exception.h"
#include "st/st_reg.h"

static char *exception_names[] = {
	"NONE",
	"RESET",
	"NMI",
	"HARD_FAULT",
	"MEM_FAULT",
	"BUS_FAULT",
	"USAGE_FAULT",
};

void pop_registers_from_fault_stack(uint32_t *hardfault_args)
{
	uint32_t stacked_r0;
	uint32_t stacked_r1;
	uint32_t stacked_r2;
	uint32_t stacked_r3;
	uint32_t stacked_r12;
	uint32_t stacked_lr;
	uint32_t stacked_pc;
	uint32_t stacked_psr;

	stacked_r0 = ((uint32_t) hardfault_args[0]);
	stacked_r1 = ((uint32_t) hardfault_args[1]);
	stacked_r2 = ((uint32_t) hardfault_args[2]);
	stacked_r3 = ((uint32_t) hardfault_args[3]);
	stacked_r12 = ((uint32_t) hardfault_args[4]);
	stacked_lr = ((uint32_t) hardfault_args[5]);
	stacked_pc = ((uint32_t) hardfault_args[6]);
	stacked_psr = ((uint32_t) hardfault_args[7]);
	pr_err("R0:0x%08lx, R1:0x%08lx\n", stacked_r0, stacked_r1);
	pr_err("R2:0x%08lx, R3:0x%08lx\n", stacked_r2, stacked_r3);
	pr_err("R12:0x%08lx, LR:0x%08lx\n", stacked_r12, stacked_lr);
	pr_err("PC:0x%08lx, PSR:0x%08lx\n", stacked_pc, stacked_psr);
}

void inline stack_dump(void)
{
	asm volatile
	(
		"tst    lr, #4                                  \n"
		"ite    eq                                      \n"
		"mrseq  r0, msp                                 \n"
		"mrsne  r0, psp                                 \n"
		"ldr    r1, [r0, #24]                           \n"
		"ldr    r2, handler2_address_const              \n"
		"bx     r2                                      \n"
		"handler2_address_const: .word pop_registers_from_fault_stack   \n"
	);
}

static void exception_dump(void)
{
	uint32_t exception_num = 0;

	__asm volatile (
		"mrs	%0, ipsr				\n"
		: "=r"(exception_num)
	);

	pr_err("exception type: %s\n",
		exception_num <= ARRAY_SIZE(exception_names)
		? exception_names[exception_num] : "UNKNOWN");
	pr_err("SHCSR:0x%08lx, CFSR:0x%08lx\n",
		reg_read32(SCB_SHCSR_ADDR), reg_readl(SCB_CFSR_ADDR));
	pr_err("HFSR:0x%08lx, MMAR:0x%08lx\n",
		reg_read32(SCB_HFSR_ADDR), reg_readl(SCB_MMAR_ADDR));
	pr_err("BFAR:0x%08lx, AFAR:0x%08lx\n",
		reg_read32(SCB_BFAR_ADDR), reg_readl(SCB_AFAR_ADDR));
}

void exception_handler(void)
{
	exception_dump();
	stack_dump();
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
