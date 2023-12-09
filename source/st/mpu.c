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

#include "st/mpu.h"

#define MPU_BASE_ADDR           (0xE000ED90)

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
        uint8_t data_region = REG32_READ(mpu_reg->typer) & 0x0000fe00 >> 9;
        uint8_t separate = REG32_READ(mpu_reg->typer) & 0x00000001 >> 0;
        pr_info("mpu support %u data region, mode %s\n",
                data_region, separate ? "separate" : "unified");
}