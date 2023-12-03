#include <inttypes.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "misc.h"

#include "irq.h"
#include "printf.h"
#include "utils.h"

#define IRQ_EXCEPTION_TOTAL_NUM			(16)

#define IRQ_SET_REGS				(0xE000E100)
#define IRQ_CLR_REGS				(0xE000E180)
#define IRQ_PEND_REGS				(0xE000E200)
#define IRQ_PEND_CLR_REGS			(0xE000E280)
#define IRQ_ACTIVE_REGS				(0xE000E300)
#define IRQ_PRIORITY_REGS			(0xE000E400)

struct irq_device {
	int (*irq_handler)(void *private_data);
	void *private_data;
#ifdef IRQ_DEBUG_SUPPORT
	uint64_t last_enter_time;
	uint64_t last_exit_time;
#endif
};

static struct irq_device irq_devs[MAX_IRQ_NUM];

bool is_in_isr(void)
{
	uint32_t irq_num = 0;

	__asm volatile ("mrs %0, ipsr" : "=r"(irq_num));
	return irq_num > 0 ? true : false;
}

void irq_enable(void)
{

}

void irq_disable(void)
{

}

void irq_dispatch(void)
{
	uint32_t irq_num = 0;
	struct irq_device *dev = NULL;

	__asm volatile ("mrs %0, ipsr" : "=r"(irq_num));
	irq_num -= IRQ_EXCEPTION_TOTAL_NUM;
	dev = &irq_devs[irq_num];
	pr_err("%s irq %" PRIu32 ", handler:%p\n", __func__, irq_num, dev->irq_handler);
	configASSERT(dev->irq_handler);
	dev->irq_handler(dev->private_data);
}

void irq_register(uint8_t irq_num, int (*irq_handler)(void *private_data), void *private_data)
{
	NVIC_InitTypeDef nvic;
	configASSERT(irq_num < MAX_IRQ_NUM);
	irq_devs[irq_num].irq_handler = irq_handler;
	irq_devs[irq_num].private_data = private_data;
	nvic.NVIC_IRQChannel = irq_num;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 14;
	NVIC_Init(&nvic);
	pr_err("%s irq %u handler:%p\n", __func__, irq_num, irq_devs[irq_num].irq_handler);
}

void irq_init(void)
{
	uint8_t i = 0;
	for (i = 0; i < ARRAY_SIZE(irq_devs); i++)
		memset(&irq_devs[i], sizeof(struct irq_device), 0x00);
}
