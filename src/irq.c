#include <inttypes.h>
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "log.h"
#include "irq.h"

#include <st/st_board.h>

#define IRQ_EXCEPTION_TOTAL_NUM					(16)

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
	if (irq_num < IRQ_EXCEPTION_TOTAL_NUM)
		configASSERT(0);

	irq_num -= IRQ_EXCEPTION_TOTAL_NUM;
	dev = &support_irq_list[irq_num];
	if (!dev->irq_handler) {
		loge("Cannot handle irq num %" PRIu32 "\n", irq_num);
		configASSERT(0);
	}

	dev->irq_handler(dev->private_data);
}

uint8_t last_register_num = 0xff;
void irq_register(uint8_t irq_num, int (*irq_handler)(void *private_data), void *private_data)
{
	last_register_num = irq_num;
	support_irq_list[irq_num].irq_handler = irq_handler;
	support_irq_list[irq_num].private_data = private_data;
}

__weak void SVC_Handler(void)
{
	/* override at freertos if support */
	logi("%s\n", __func__);
}
__weak void PendSV_Handler(void)
{
	/* override at freertos if support */
	logi("%s\n", __func__);
}

__weak void SysTick_Handler(void)
{
	/* override at freertos if support */
	logi("%s\n", __func__);
}

__weak void EXTI9_5_IRQHandler(void)
{
	logi("%s\n", __func__);
}

__weak void EXTI15_10_IRQHandler(void)
{
	logi("%s\n", __func__);
}

void BusFault_Handler(void)
{
	loge("%s\n", __func__);
	while (1) {
	}
}

void UsageFault_Handler(void)
{
	loge("%s\n", __func__);
	while (1) {
	}
}

void DebugMon_Handler(void)
{
	loge("%s\n", __func__);
}
void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
	loge("%s\n", __func__);
	while (1) {
	}
}

void MemManage_Handler(void)
{
	loge("%s\n", __func__);
	while (1) {
	}
}

void TIM2_IRQHandler(void)
{
}

void TIM7_IRQHandler(void)
{
}