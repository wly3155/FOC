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

#ifndef __ST_FLASH_H__
#define __ST_FLASH_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int flash_write_byte(void *addr, uint8_t value);
int flash_write_halfword(void *addr, uint16_t value);
int flash_write_word(void *addr, uint32_t value);

void *flash_malloc(uint32_t size);
int flash_platform_init(void);

#ifdef __cplusplus
}
#endif
#endif