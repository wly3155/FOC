/*
 * This file is licensed under the Apache License, Version 2.0.
 *
 * Copyright (c) 2024 wuliyong3155@163.com
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

#include "printf.h"
#include "sec_defs.h"

#include "stm32f4xx.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_dma.h"

RO_DEBUG volatile ADC_TypeDef *const dbg_adc1 = ADC1;
RO_DEBUG volatile DMA_TypeDef *const dbg_dma2 = DMA2;
RO_DEBUG volatile DMA_Stream_TypeDef *const dbg_dma2_stream0 = DMA2_Stream0;

int debug_init(void)
{
        pr_info("dbg_adc1 %p\n", dbg_adc1);
        pr_info("dbg_dma2 %p\n", dbg_dma2);
        pr_info("dbg_dma2_stream0 %p\n", dbg_dma2_stream0);
        return 0;
}