#include <inttypes.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "log.h"
#include "irq.h"
#include "utils.h"

#include <st/st_board.h>

#define IRQ_EXCEPTION_TOTAL_NUM                    (16)

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
    configASSERT(irq_num < MAX_IRQ_NUM);

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
    configASSERT(0);
}

void UsageFault_Handler(void)
{
    loge("%s\n", __func__);
    configASSERT(0);
}

void DebugMon_Handler(void)
{
    loge("%s\n", __func__);
    configASSERT(0);
}
void NMI_Handler(void)
{
    loge("%s\n", __func__);
    configASSERT(0);
}

void pop_registers_from_fault_stack(unsigned int * hardfault_args)
{
    unsigned int stacked_r0;
    unsigned int stacked_r1;
    unsigned int stacked_r2;
    unsigned int stacked_r3;
    unsigned int stacked_r12;
    unsigned int stacked_lr;
    unsigned int stacked_pc;
    unsigned int stacked_psr;

    stacked_r0 = ((unsigned long) hardfault_args[0]);
    stacked_r1 = ((unsigned long) hardfault_args[1]);
    stacked_r2 = ((unsigned long) hardfault_args[2]);
    stacked_r3 = ((unsigned long) hardfault_args[3]);
    stacked_r12 = ((unsigned long) hardfault_args[4]);
    stacked_lr = ((unsigned long) hardfault_args[5]);
    stacked_pc = ((unsigned long) hardfault_args[6]);
    stacked_psr = ((unsigned long) hardfault_args[7]);
    /* Inspect stacked_pc to locate the offending instruction. */
    logi("dump regs: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n",
        stacked_r0, stacked_r1, stacked_r2, stacked_r3,
        stacked_r12, stacked_lr, stacked_pc , stacked_psr);
    for( ;; );
}

void HardFault_Handler(void)
{
    asm volatile
    (
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, handler2_address_const                            \n"
        " bx r2                                                     \n"
        " handler2_address_const: .word pop_registers_from_fault_stack    \n"
    );
    configASSERT(0);
}

void MemManage_Handler(void)
{
    loge("%s\n", __func__);
    configASSERT(0);
}

void TIM2_IRQHandler(void)
{
}

void TIM7_IRQHandler(void)
{
}

void irq_init(void)
{
    uint8_t i = 0;

    for (i = 0; i < ARRAY_SIZE(support_irq_list); i++)
        memset(&support_irq_list[i], sizeof(struct irq_device), 0x00);
}
