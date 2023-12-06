#include <inttypes.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "misc.h"

#include "st/irq.h"
#include "printf.h"
#include "utils.h"

#define IRQ_EXCEPTION_TOTAL_NUM				(16)

struct irq_device {
	int (*irq_handler)(void *private_data);
	void *private_data;
	uint64_t last_enter_time;
	uint64_t last_exit_time;
};

struct irq_device support_irq_list[MAX_IRQ_NUM];

bool is_in_isr(void)
{
	uint32_t irq_num = 0;

	__asm volatile ("mrs %0, ipsr" : "=r"(irq_num));
	return irq_num > 0 ? true : false;
}

void irq_dispatch(void)
{
	uint32_t irq_num = 0;
	struct irq_device *dev = NULL;

	__asm volatile ("mrs %0, ipsr" : "=r"(irq_num));
	irq_num -= IRQ_EXCEPTION_TOTAL_NUM;
	dev = &support_irq_list[irq_num];
	configASSERT(dev->irq_handler);
	dev->irq_handler(dev->private_data);
}

void irq_register(uint8_t irq_num, int (*irq_handler)(void *private_data), void *private_data)
{
	NVIC_InitTypeDef nvic;

	configASSERT(irq_num < MAX_IRQ_NUM);
	support_irq_list[irq_num].irq_handler = irq_handler;
	support_irq_list[irq_num].private_data = private_data;
	nvic.NVIC_IRQChannel = irq_num;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&nvic);
}

void irq_init(void)
{
	uint8_t i = 0;

	for (i = 0; i < ARRAY_SIZE(support_irq_list); i++)
		memset(&support_irq_list[i], sizeof(struct irq_device), 0x00);
}
