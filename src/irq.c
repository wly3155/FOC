#include "log.h"
#include "irq.h"

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



