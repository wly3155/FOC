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

#include <errno.h>

#include "stm32f4xx_flash.h"
#include "st/flash.h"

#define FLASH_MEMORY_ADDR		((uint32_t)0x080C0000)
#define FLASH_MEMORY_SIZE		(40000) //256Kbytes
#define FLASH_MEMORY_BLOCK_SIZE		(4096)
#define FLASH_MEMORY_BLOCK_NUM		(FLASH_MEMORY_SIZE / FLASH_MEMORY_BLOCK_SIZE)

struct flash_memory {
	void *addr;
	uint32_t size;
	uint8_t block_used;
};

static struct flash_memory flash_mem;

int flash_write_byte(void *addr, uint8_t value)
{
	if (addr < flash_mem.addr || addr >= flash_mem.addr + flash_mem.size)
		return -EINVAL;

	return FLASH_ProgramByte((uint32_t)addr, value);
}

int flash_write_halfword(void *addr, uint16_t value)
{
	if (addr < flash_mem.addr || addr >= flash_mem.addr + flash_mem.size)
		return -EINVAL;

	return FLASH_ProgramHalfWord((uint32_t)addr, value);
}

int flash_write_word(void *addr, uint32_t value)
{
	if (addr < flash_mem.addr || addr >= flash_mem.addr + flash_mem.size)
		return -EINVAL;

	return FLASH_ProgramWord((uint32_t)addr, value);
}

void *flash_malloc(uint32_t size)
{
	//TBD, thread unsafe
	if (flash_mem.block_used >= FLASH_MEMORY_BLOCK_NUM)
		return NULL;

	return flash_mem.addr + flash_mem.block_used++ * FLASH_MEMORY_BLOCK_SIZE;
}

int flash_platform_init(void)
{
	flash_mem.addr = (void *)FLASH_MEMORY_ADDR;
	flash_mem.size =FLASH_MEMORY_SIZE;
	flash_mem.block_used = 0;

	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR
		| FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

	return 0;
}
