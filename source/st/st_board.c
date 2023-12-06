#include "st/st_board.h"
#include "st/st_gpio.h"
#include "st/st_reg.h"
#include "st/time.h"
#include "st/st_timer.h"

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

void prvSetupHardware(void)
{
    /* Setup STM32 system (clock, PLL and Flash configuration) */
    SystemInit();

    /* Ensure all priority bits are assigned as preemption priority bits. */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    reg_bitwise_write(SCB_SHCSR_ADDR, 7<<16, 7<<16);
    gpio_platform_init();
    time_platform_init();
    //timer_platform_init();
}
