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

#ifndef __ST_IRQ_H__
#define __ST_IRQ_H__

#include <stdbool.h>
#include "stm32f4xx.h"

#ifdef __cplusplus
extern "C" {
#endif

bool is_in_isr(void);
void irq_enable(void);
void irq_disable(void);
void irq_register(uint8_t irq_num,
        int(*irq_handler)(void *private_data), void *private_data);
void irq_init(void);

#ifdef __cplusplus
}
#endif
#endif

