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

#ifndef __ST_ADC_H__
#define __ST_ADC_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
        ADC_GROUP0,
        ADC_GROUP1,
        ADC_GROUP2,
        MAX_ADC_NUM,
};

enum {
        ADC_CHAN0,
        ADC_CHAN1,
        ADC_CHAN2,
        ADC_CHAN3,
        ADC_CHAN4,
        ADC_CHAN5,
        MAX_ADC_CHAN,
};

uint16_t adc_get_value(uint8_t adc, uint8_t chan);
int adc_platform_init(void);
int adc_register_handler(uint8_t adc, uint8_t chan,
	int (*adc_handler)(void *private_data,  uint32_t adc_value), void *private_data);
void adc_unregister_handler(uint8_t adc, uint8_t chan);

#ifdef __cplusplus
}
#endif
#endif
