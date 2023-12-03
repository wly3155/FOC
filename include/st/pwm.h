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

#ifndef __ST_PWM_H__
#define __ST_PWM_H__

#include <stdbool.h>
#include "stm32f4xx.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
	PWM_GROUP0,
	PWM_GROUP1,
	MAX_PWM_GROUP,
};

enum {
	PWM_CHAN0,
	PWM_CHAN1,
	PWM_CHAN2,
	MAX_PWM_CHAN,
};

int pwm_channel_enable(uint8_t group, uint8_t channel, bool enable);
int pwm_group_enable(uint8_t group, bool enable);
int pwm_set_duty(uint8_t group, float duty);
int pwm_platform_init(void);

#ifdef __cplusplus
}
#endif
#endif