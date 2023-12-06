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

#ifndef __REG_OPS_H__
#define __REG_OPS_H__

#include <stdint.h>

#ifdef __cplusplus
extern "c" {
#endif

#define reg_read32(reg)			(uint32_t)(*((volatile uint32_t *)(reg)))
#define reg_write32(reg, val)		(*(volatile uint32_t *)(reg) = (uint32_t)(val))

static inline uint32_t reg_bitwise_write32(volatile uint32_t reg, uint32_t bitwise, uint32_t value)
{
	uint32_t tmp = reg_read32(reg);

	tmp &= ~bitwise;
	tmp |= value;
	reg_write32(reg, tmp);
	return tmp;
}

#ifdef __cplusplus
}
#endif
#endif
