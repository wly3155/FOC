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
#include "printf.h"
#include "utils.h"

#include "st/st_reg.h"
#include "st/mpu.h"

#define MPU_BASE_ADDR		(0xE000ED90UL)


struct mpu_register {
	uint32_t typer; /* MPU type register */
	uint32_t ctrl; /* MPU control register */
	uint32_t rnr; /* MPU region number register */
	uint32_t rbar; /* MPU region base address register */
	uint32_t rasr; /* MPU region attribute and size register */
};

static struct mpu_register *mpu_reg = (struct mpu_register *)MPU_BASE_ADDR;

void mpu_init(void)
{
        uint8_t data_region = (reg_readl(&mpu_reg->typer) & 0x0000ff00) >> 8;
        uint8_t separate = (reg_readl(&mpu_reg->typer) & 0x00000001) >> 0;
        pr_info("mpu support %u data region, mode: %s\n",
                data_region, separate ? "separate" : "unified");

        reg_writel(&mpu_reg->ctrl, 0);

        reg_writel(&mpu_reg->rnr, 0); /* sram memmory 128KB */
        reg_writel(&mpu_reg->rbar, 0x02000000);
        reg_writel(&mpu_reg->rasr, 0x03 << 24 | 0x23);

        reg_writel(&mpu_reg->rnr, 1); /* flash memmory 1MB */
        reg_writel(&mpu_reg->rbar, 0x80000000);
        reg_writel(&mpu_reg->rasr, 0x3 << 24 | 0x27);

        reg_writel(&mpu_reg->rnr, 2); /* platform periph memmory 512MB */
        reg_writel(&mpu_reg->rbar, 0x40000000);
        reg_writel(&mpu_reg->rasr, 0x1 << 28 | 0x3 << 24 | 0x39);

        reg_writel(&mpu_reg->rnr, 2); /* core periph memmory 512MB */
        reg_writel(&mpu_reg->rbar, 0xe0000000);
        reg_writel(&mpu_reg->rasr, 0x1 << 28 | 0x3 << 24 | 0x39);

	reg_bitwise_write(SHCSR_REG_ADDR, 7<<16, 7<<16);
        reg_writel(&mpu_reg->ctrl, 5);
}