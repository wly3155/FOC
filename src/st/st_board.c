#include "st/st_board.h"

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

void prvSetupHardware(void)
{
	/* Setup STM32 system (clock, PLL and Flash configuration) */
	SystemInit();

	/* Ensure all priority bits are assigned as preemption priority bits. */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}