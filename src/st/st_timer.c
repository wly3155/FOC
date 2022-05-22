#include <errno.h>
#include <string.h>
#include <st/st_timer.h>

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"

/*
static int timer_channel_to_clock(TIM_TypeDef* TIMx)
{
	switch (TIMx) {
	case TIM1:
		return RCC_APB2Periph_TIM1;
	case TIM2:
		return RCC_APB1Periph_TIM2;
	case TIM3:
		return RCC_APB1Periph_TIM3;
	case TIM8:
		return RCC_APB2Periph_TIM8;
	}

	return -EINVAL;
}
*/
int timer_clock_enable(int channel, bool en)
{
	int clock_source = 0;//timer_channel_to_clock(channel);

	RCC_APB1PeriphClockCmd(clock_source, en);
	return 0;
}

int timer_stop(TIM_TypeDef* TIMx)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	return 0;
}

int timer_start(TIM_TypeDef* TIMx, uint32_t freq)
{
	TIMx->ARR = freq;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	return 0;
}

int timer_init(TIM_TypeDef* TIMx, uint32_t freq)
{
	uint16_t PrescalerValue = 0;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 6000000) - 1;

	memset(&TIM_TimeBaseStructure, 0x00, sizeof(TIM_TimeBaseStructure));
	TIM_TimeBaseStructure.TIM_Period = 65535;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);

	/* Prescaler configuration */
	TIM_PrescalerConfig(TIMx, PrescalerValue, TIM_PSCReloadMode_Immediate);
	return 0;
}