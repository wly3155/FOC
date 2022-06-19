#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <st/st_board.h>
#include <st/st_timer.h>
#include <st/st_utils.h>

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx.h"
#include "misc.h"

#include "irq.h"

#define TIMER_COUNTER_BASE_FREQ_HZ			(12000000)
#define ptr_to_timer_id(ptr)				((uint32_t)ptr)
#define ptr_to_timer_irq_num(ptr)			((uint32_t)ptr)

static struct timer_device support_timer_list[] = {
	{
		.id = TIMER1,
		.freq_hz = 0,
		.in_use = false,
		.irq_enable = false,
		.irq_num = TIM1_CC_IRQn,
		.base_addr = TIM1,
		.periph_clock = RCC_APB2Periph_TIM1,
		.periph_clock_group = RCC_APB2Periph,
	},
	{
		.id = TIMER2,
		.freq_hz = 0,
		.in_use = false,
		.irq_enable = false,
		.irq_num = TIM2_IRQn,
		.base_addr = TIM2,
		.periph_clock = RCC_APB1Periph_TIM2,
		.periph_clock_group = RCC_APB1Periph,
	},
	{
		.id = TIMER5,
		.freq_hz = 0,
		.in_use = false,
		.irq_enable = false,
		.irq_num = TIM5_IRQn,
		.base_addr = TIM5,
		.periph_clock = RCC_APB1Periph_TIM5,
		.periph_clock_group = RCC_APB1Periph,
	},
	{
		.id = TIMER8,
		.freq_hz = 0,
		.in_use = false,
		.irq_enable = false,
		.irq_num = TIM8_CC_IRQn,
		.base_addr = TIM8,
		.periph_clock = RCC_APB2Periph_TIM8,
		.periph_clock_group = RCC_APB2Periph,
	},
};

static uint16_t inline timer_get_irq_status(struct timer_device *dev)
{
	return dev->base_addr->SR;
}

static void inline timer_clear_pending_irq(struct timer_device *dev, uint16_t pending)
{
	dev->base_addr->SR = ~pending;
}

static int timer_irq_handler(void *private_data)
{
	int i = 0;
	uint16_t irq_status = 0;
	struct timer_device *dev = NULL;
	uint8_t irq_num = ptr_to_timer_irq_num(private_data);

	for (i = 0; i < ARRAY_SIZE(support_timer_list); i++) {
		dev = &support_timer_list[i];
		if (dev->irq_num == irq_num) {
			irq_status = timer_get_irq_status(dev);
			//printf("try to handle timer %u %u\n", irq_num, irq_status);
			if (irq_status & dev->irq_request_type) {
				timer_clear_pending_irq(dev, dev->irq_request_type);
				if (!dev->irq_handler)
					return -ENXIO;

				return dev->irq_handler(private_data);
			}
		}
	}

	printf("unhandle timer %u %u\n", irq_num, irq_status);
	return -ENXIO;
}

static int timer_init_irq(struct timer_device *dev)
{
	NVIC_InitTypeDef nvic;

	nvic.NVIC_IRQChannel = dev->irq_num;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	nvic.NVIC_IRQChannelPreemptionPriority = 15;
	nvic.NVIC_IRQChannelSubPriority = 15;
	NVIC_Init(&nvic);

	irq_register(dev->irq_num, timer_irq_handler, (void *)dev->irq_num);
	TIM_ITConfig(dev->base_addr, dev->irq_request_type, ENABLE);
	return 0;
}

int timer_start(struct timer_device *dev)
{
	TIM_Cmd(dev->base_addr, ENABLE);
	return 0;
}

int timer_stop(struct timer_device *dev)
{
	TIM_Cmd(dev->base_addr, DISABLE);
	return 0;
}

int timer_init(struct timer_device *dev)
{
#define MIN_FREQ_HZ			(5)

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	uint16_t PrescalerValue = 0;

	if (dev->periph_clock_group == RCC_APB1Periph) {
		RCC_APB1PeriphClockCmd(dev->periph_clock, ENABLE);
		PrescalerValue = (uint16_t)((SystemCoreClock / 2) / TIMER_COUNTER_BASE_FREQ_HZ) - 1;
	} else {
		RCC_APB2PeriphClockCmd(dev->periph_clock, ENABLE);
		PrescalerValue = (uint16_t)((SystemCoreClock) / TIMER_COUNTER_BASE_FREQ_HZ) - 1;
	}

	dev->freq_hz = dev->freq_hz > 1 ? dev->freq_hz : MIN_FREQ_HZ;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = TIMER_COUNTER_BASE_FREQ_HZ / dev->freq_hz - 1;
	TIM_TimeBaseInit(dev->base_addr, &TIM_TimeBaseStructure);

	if (dev->irq_enable)
		timer_init_irq(dev);

	return 0;
}

struct timer_device *request_timer_device(uint8_t id)
{
	int i = 0;

	for (i = 0; i < ARRAY_SIZE(support_timer_list); i++) {
		if (support_timer_list[i].id == id) {
			support_timer_list[i].in_use = true;
			return &support_timer_list[i];
		}
	}

	return NULL;
}


