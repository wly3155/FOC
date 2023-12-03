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

#ifndef __ADC_WRAPPER_H__
#define __ADC_WRAPPER_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_ADC_NUM			(3)
#define MAX_CHAN_PRE_ADC		(6)

uint16_t adc_wrapper_get_value(uint8_t adc, uint8_t chan);
int adc_wrapper_init(void);

#ifdef __cplusplus
}
#endif
#endif