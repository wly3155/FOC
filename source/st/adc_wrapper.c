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

#include <stddef.h>

#include "stm32f4xx_adc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"

#include "irq.h"
#include "printf.h"

#include "st/adc_wrapper.h"

#define ADC_TRIG_RATE_HZ		(10)
#define TIM_PSC_CLOCK_HZ		(1000000)

struct adc_device {
	ADC_TypeDef *base;
	uint8_t chan;
	uint8_t wp;
	uint16_t data[MAX_CHAN_PRE_ADC];
};

static struct adc_device adc_devs[MAX_ADC_NUM] = {
	{
		.base = ADC1,
		.chan = 4,
	},
	{
		.base = ADC2,
		.chan = 0,
	},
	{
		.base = ADC3,
		.chan = 0,
	},
};

int adc_timer_irq_handler(void *private_data)
{
	pr_err("%s\n", __func__);
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
    		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		pr_err("%s\n", __func__);
	}

	return 0;
}

static int adc_wrapper_timer_init(uint16_t arr,uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	irq_register(TIM2_IRQn,  adc_timer_irq_handler, NULL);
	TIM_Cmd(TIM2, ENABLE);
	return 0;
}

static int adc_wrapper_irq_handler(void *private_data)
{
	uint8_t i = 0;
	struct adc_device *dev = NULL;

	for (i = 0; i < MAX_ADC_NUM; i++) {
		dev = &adc_devs[i];
		if (!ADC_GetITStatus(dev->base, ADC_IT_EOC))
			continue;

		dev->data[dev->wp] = ADC_GetConversionValue(dev->base);
		pr_info("%s adc%u value %u\n", __func__, i, dev->data[dev->wp]);
		dev->wp++;
		dev->wp &= dev->chan - 1;
		ADC_ClearITPendingBit(dev->base, ADC_IT_EOC);

	}

	return 0;
}

uint16_t adc_wrapper_get_value(uint8_t adc, uint8_t chan)
{
	return adc_devs[adc].data[chan];
}

uint16_t adc_wrapper_enable(uint8_t adc, uint8_t chan, bool enable)
{
	//return adc_devs[adc].data[chan];
	return 0;
}

int adc_wrapper_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;

	pr_info("********adc start*********\n");
        /* GPIOC Configuration: PC0~PC3 (ADC123_IN10~IN13) BLDC current */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
        GPIO_Init(GPIOC, &GPIO_InitStructure);

        /* GPIOA Configuration: PA6 (ADC12_IN6) BAT volatge */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* ADC Common Init **********************************************************/
	ADC_CommonInitStructure.ADC_Mode = ADC_TripleMode_RegSimult;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4; //21M
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

	/* ADC1 Init ****************************************************************/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_TRGO;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 4;
	ADC_Init(ADC1, &ADC_InitStructure);

	//ADC_ExternalTrigConvCmd(ADC1, ENABLE);
	//ADC_Init(ADC2, &ADC_InitStructure);
	//ADC_Init(ADC3, &ADC_InitStructure);

	/* Enable ADC1 DMA */
	//ADC_DMACmd(ADC1, ENABLE);
	pr_info("********adc start1*********\n");
	/* ADC1 regular channel18 (VBAT) configuration ******************************/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_3Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_3Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_3Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_3Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 5, ADC_SampleTime_3Cycles);
	/* Enable VBAT channel */
	//ADC_VBATCmd(ENABLE);

	/* Enable DMA request after last transfer (Single-ADC mode) */
	//ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

	pr_info("********adc start2*********\n");
	irq_register(ADC_IRQn,  adc_wrapper_irq_handler, NULL);
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

	/* Enable ADC1 **************************************************************/
	ADC_Cmd(ADC1, ENABLE);

	extern uint32_t SystemCoreClock;
	adc_wrapper_timer_init(ADC_TRIG_RATE_HZ, SystemCoreClock / TIM_PSC_CLOCK_HZ);

	pr_info("********adc start3*********\n");
	return 0;
}
