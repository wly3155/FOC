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

#ifndef __CM4_SCB_H__
#define __CM4_SCB_H__

#ifdef __cplusplus
extern "C" {
#endif

#define SCB_ACTLR_ADDR			(0xE000E008)

#define SCB_BASE_ADDR			(0xE000ED00)
#define SCB_CPUID_ADDR			(SCB_BASE_ADDR + 0x00)
#define SCB_ICSR_ADDR			(SCB_BASE_ADDR + 0x04)
#define SCB_VTOR_ADDR			(SCB_BASE_ADDR + 0x08)
#define SCB_AIRCR_ADDR			(SCB_BASE_ADDR + 0x0C)
#define SCB_SCR_ADDR			(SCB_BASE_ADDR + 0x10)
#define SCB_CCR_ADDR			(SCB_BASE_ADDR + 0x14)
#define SCB_SHPR1_ADDR			(SCB_BASE_ADDR + 0x18)
#define SCB_SHPR2_ADDR			(SCB_BASE_ADDR + 0x1C)
#define SCB_SHPR3_ADDR			(SCB_BASE_ADDR + 0x20)
#define SCB_SHCSR_ADDR			(SCB_BASE_ADDR + 0x24)
#define SCB_CFSR_ADDR			(SCB_BASE_ADDR + 0x28)
#define SCB_HFSR_ADDR			(SCB_BASE_ADDR + 0x2C)
#define SCB_MMAR_ADDR			(SCB_BASE_ADDR + 0x34)
#define SCB_BFAR_ADDR			(SCB_BASE_ADDR + 0x38)
#define SCB_AFAR_ADDR			(SCB_BASE_ADDR + 0x3C)

#ifdef __cplusplus
}
#endif
#endif