#include <FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include <task.h>
#include "irq.h"
#include "utils.h"

/* systick registers */
#define REG_SYST_CSR                        (0xE000E010) //SysTick Control and Status Register
#define REG_SYST_RVR                        (0xE000E014) //SysTick Reload Value Register
#define REG_SYST_CVR                        (0xE000E018) //SysTick Current Value Register
#define REG_SYST_CALIB                      (0xE000E01C) //SysTick Calibration Value Register

#define configSYSTICK_CLOCK_NS              (1000000000ULL / configCPU_CLOCK_HZ)

static uint32_t get_curr_systick_count(void)
{
    return reg_read32(REG_SYST_CVR);
}

uint64_t get_boot_time_ns(void)
{
    TickType_t tick;
    uint32_t tick_count;

    if (is_in_isr())
        tick = xTaskGetTickCountFromISR();
    else
        tick = xTaskGetTickCount();

    tick_count = get_curr_systick_count();
    return ms_to_ns(tick_to_ms(tick)) + tick_count * configSYSTICK_CLOCK_NS;
}
