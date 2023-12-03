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

#include <inttypes.h>
#include <string.h>

#include "FreeRTOS.h"

#include "printf.h"
#include "utils.h"

#include <execption.h>

void BusFault_Handler(void)
{
	pr_err("%s\n", __func__);
	configASSERT(0);
}

void UsageFault_Handler(void)
{
	pr_err("%s\n", __func__);
	configASSERT(0);
}

void DebugMon_Handler(void)
{
	pr_err("%s\n", __func__);
	configASSERT(0);
}

void NMI_Handler(void)
{
	pr_err("%s\n", __func__);
	configASSERT(0);
}

 __attribute__((used)) static void pop_registers_from_fault_stack(uint32_t * hardfault_args)
{
	uint32_t stacked_r0 = (uint32_t)hardfault_args[0];
	uint32_t stacked_r1 = (uint32_t)hardfault_args[1];
	uint32_t stacked_r2 = (uint32_t)hardfault_args[2];
	uint32_t stacked_r3 = (uint32_t)hardfault_args[3];
	uint32_t stacked_r12 = (uint32_t)hardfault_args[4];
	uint32_t stacked_lr = (uint32_t)hardfault_args[5];
	uint32_t stacked_pc = (uint32_t)hardfault_args[6];
	uint32_t stacked_psr = (uint32_t)hardfault_args[7];

	/* Inspect stacked_pc to locate the offending instruction. */
	pr_info("dump regs: 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx\n",
		stacked_r0, stacked_r1, stacked_r2, stacked_r3,
		stacked_r12, stacked_lr, stacked_pc , stacked_psr);
}

void HardFault_Handler(void)
{
	asm volatile (
		" tst lr, #4 					\n"
		" ite eq					\n"
		" mrseq r0, msp					\n"
		" mrsne r0, psp 				\n"
		" ldr r1, [r0, #24]				\n"
		" ldr r2, handler2_address_const		\n"
		" bx r2						\n"
		" handler2_address_const: .word pop_registers_from_fault_stack	\n"
	);
	pr_err("%s\n", __func__);
	configASSERT(0);
}

void MemManage_Handler(void)
{
	pr_err("%s\n", __func__);
	configASSERT(0);
}
