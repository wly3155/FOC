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

#include <stdbool.h>
#include <stdint.h>

#include "reg_ops.h"

#include "st/clock.h"
#include "st/pwm.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_rcc.h"

#define TIME_BASE_FREQ_HZ		(42 * 1e6) /* 42M Timer Resoultion */
#define TIME_BASE_NS			(1 * 1e9 / TIME_BASE_FREQ_HZ)

#define PWM_PERIOD_HZ                   (20000)
#define PWM_CENTER_ALIGN                (true)
#define PWM_MAX_ALLOW_DUTY              (0.9f)

static TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
static TIM_OCInitTypeDef       TIM_OCInitStructure;
static TIM_BDTRInitTypeDef     TIM_BDTRInitStructure;
static TIM_TypeDef *tim[] = {TIM1, TIM8};
static uint32_t pwm_period_counter;

int pwm_channel_enable(uint8_t group, uint8_t channel, bool enable)
{
        TIM_CCxCmd(tim[group], channel * 4, enable);
        return 0;
}

int pwm_group_enable(uint8_t group, bool enable)
{
        /* TIM1 counter enable */
        TIM_Cmd(tim[group], enable);

        /* Main Output Enable */
        TIM_CtrlPWMOutputs(tim[group], enable);
        return 0;
}

int pwm_set_duty(uint8_t group, float duty)
{
        uint32_t pulse_counter = 0;

        duty = duty < PWM_MAX_ALLOW_DUTY ? duty : PWM_MAX_ALLOW_DUTY;
        pulse_counter = (uint32_t)(pwm_period_counter * duty);
        TIM_OCInitStructure.TIM_Pulse = pulse_counter;

        TIM_OC1Init(tim[group], &TIM_OCInitStructure);
        TIM_OC2Init(tim[group], &TIM_OCInitStructure);
        TIM_OC3Init(tim[group], &TIM_OCInitStructure);
        return 0;
}

int pwm_platform_init(void)
{

        GPIO_InitTypeDef GPIO_InitStructure;
	uint16_t pwm_period_ns = 1 * 1e9 / PWM_PERIOD_HZ;

	pwm_period_ns = PWM_CENTER_ALIGN ? pwm_period_ns >> 1 : pwm_period_ns;

        /* GPIOA and GPIOB clocks enable */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);

        /* TIM1 clock enable */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);

        /* GPIOA Configuration: Channel 1 and 3 as alternate function push-pull */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        /* GPIOB Configuration: Channel 1N, 2N and 3N as alternate function push-pull */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        /* GPIOB Configuration: Channel 1N, 2N and 3N as alternate function push-pull */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
        GPIO_Init(GPIOC, &GPIO_InitStructure);

        /* Connect TIM pins to AF1 */
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1);
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_TIM1);
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_TIM1);
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_TIM1);
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_TIM1);
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_TIM1);

        GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_TIM8);
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM8);
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_TIM8);
        GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM8);
        GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM8);
        GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM8);

        /* Time Base configuration */
        TIM_TimeBaseStructure.TIM_Prescaler = configAPB2_TIM_CLOCK_HZ / TIME_BASE_FREQ_HZ -1;
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned1;
        pwm_period_counter = pwm_period_ns / TIME_BASE_NS - 1;
	TIM_TimeBaseStructure.TIM_Period = pwm_period_counter;
        TIM_TimeBaseStructure.TIM_ClockDivision = 0;
        TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

        TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
        TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

        /* Channel 1, 2,3 and 4 Configuration in PWM mode */
        TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
        TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
        TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
        TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
        TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
        TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
        TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;
        TIM_OCInitStructure.TIM_Pulse = 0;
        TIM_OC1Init(TIM1, &TIM_OCInitStructure);
        TIM_OC2Init(TIM1, &TIM_OCInitStructure);
        TIM_OC3Init(TIM1, &TIM_OCInitStructure);
        TIM_OC1Init(TIM8, &TIM_OCInitStructure);
        TIM_OC2Init(TIM8, &TIM_OCInitStructure);
        TIM_OC3Init(TIM8, &TIM_OCInitStructure);

        /* Automatic Output enable, Break, dead time and lock configuration*/
        TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
        TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
        TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
        TIM_BDTRInitStructure.TIM_DeadTime = 1;
        TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
        TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
        TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;

        TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);
        TIM_CCPreloadControl(TIM1, ENABLE);
        TIM_ITConfig(TIM1, TIM_IT_COM, ENABLE);

        TIM_BDTRConfig(TIM8, &TIM_BDTRInitStructure);
        TIM_CCPreloadControl(TIM8, ENABLE);
        TIM_ITConfig(TIM8, TIM_IT_COM, ENABLE);
        return 0;
}
