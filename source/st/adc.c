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

#include "FreeRTOS.h"
#include "task.h"

#include "printf.h"
#include "utils.h"

#include "st/adc.h"
#include "st/clock.h"
#include "st/irq.h"

#include "stm32f4xx_adc.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"

#define ADC_TRIG_PERIOD			ms_to_ns(500)
#define TIME_BASE_FREQ_HZ		(40 * 1e3) /* 40K Timer */
#define TIME_BASE_NS			(1 * 1e9 / TIME_BASE_FREQ_HZ)
#define TIME_MAX_PERIOD			ms_to_ns(1000)

/* Must define DMA mode if using multi channel */
#define ADC_USING_DMA_MODE

struct adc_device {
	ADC_TypeDef *base;
	uint8_t chan;
	uint8_t wp;
	uint16_t data[MAX_CHAN_PRE_ADC];
};

static struct adc_device adc_devs[MAX_ADC_NUM] = {
	{
		.base = ADC1,
		.chan = 5,
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

static int adc_timer_trig_irq_handler(void *private_data)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}

	return 0;
}

static int adc_timer_trig_init(uint64_t trig_period_ns)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	pr_info("trig preiod %llu\n", trig_period_ns);
	configASSERT(trig_period_ns <= TIME_MAX_PERIOD);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	TIM_TimeBaseStructure.TIM_Period = trig_period_ns / TIME_BASE_NS - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = configAPB1_TIM_CLOCK_HZ / TIME_BASE_FREQ_HZ - 1;;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	irq_register(TIM3_IRQn, adc_timer_trig_irq_handler, NULL);
	TIM_Cmd(TIM3, ENABLE);
	return 0;
}

uint16_t adc_get_value(uint8_t adc, uint8_t chan)
{
	return adc_devs[adc].data[chan];
}

uint16_t adc_enable(uint8_t adc, uint8_t chan, bool enable)
{
	//return adc_devs[adc].data[chan];
	return 0;
}

#ifdef ADC_USING_DMA_MODE
static int dma2_steam0_irq_handler(void *private_data)
{
	struct adc_device *adc_dev = NULL;

	adc_dev = &adc_devs[0];
	if (DMA_GetFlagStatus(DMA2_Stream0, DMA_FLAG_TCIF0)) {
		DMA_ClearITPendingBit(DMA2_Stream0, DMA_FLAG_TCIF0);
	}

	pr_info("%s adc value %u %u %u %u %u\n",
		__func__, adc_dev->data[0], adc_dev->data[1],
		adc_dev->data[2], adc_dev->data[3], adc_dev->data[4]);
	return 0;
}

static int adc_dma_init(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	struct adc_device *adc_dev = NULL;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

	/* DMA2_Stream0 channel0 configuration **************************************/
	DMA_DeInit(DMA2_Stream0);
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	adc_dev = &adc_devs[0];
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&adc_dev->base->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&adc_dev->data;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = adc_dev->chan;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);
	/* DMA2_Stream0 enable */
	irq_register(DMA2_Stream0_IRQn, dma2_steam0_irq_handler, NULL);
	DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);
	DMA_Cmd(DMA2_Stream0, ENABLE);
	return 0;
}
#else

static int adc_irq_handler(void *private_data)
{
	uint8_t i = 0;
	struct adc_device *dev = NULL;

	for (i = 0; i < MAX_ADC_NUM; i++) {
		dev = &adc_devs[i];
		if (!ADC_GetITStatus(dev->base, ADC_IT_EOC))
			continue;

		dev->data[dev->wp] = ADC_GetConversionValue(dev->base);
		pr_info("%s adc value %u %u %u %u %u\n", __func__,
			dev->data[0], dev->data[1], dev->data[2],
			dev->data[3], dev->data[4]);
		dev->wp++;
		dev->wp &= dev->chan - 1;
		ADC_ClearITPendingBit(dev->base, ADC_IT_EOC);
	}

	return 0;
}
#endif

int adc_platform_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	struct adc_device *adc_dev = NULL;

#ifdef ADC_USING_DMA_MODE
	adc_dma_init();
#endif

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

	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent; //TBD ADC_Mode_Independent
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	adc_dev = &adc_devs[0];
	ADC_InitStructure.ADC_NbrOfConversion = adc_dev->chan;
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_3Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_3Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_3Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_3Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 5, ADC_SampleTime_3Cycles);

#ifdef ADC_USING_DMA_MODE
	//ADC_MultiModeDMARequestAfterLastTransferCmd(ENABLE);
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);
#else
	irq_register(ADC_IRQn,  adc_irq_handler, NULL);
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
#endif

	adc_timer_trig_init(ADC_TRIG_PERIOD);
	ADC_Cmd(ADC1, ENABLE);
	return 0;
}
