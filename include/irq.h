#ifndef __IRQ_H__
#define __IRQ_H__

#include <stdbool.h>

#ifdef __cplusplus
 extern "C" {
#endif 

bool is_in_isr(void);
void irq_register(uint8_t irq_num, int (*irq_handler)(void *private_data), void *private_data);

#ifndef __weak
#define __weak __attribute__((weak))
#endif

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void DebugMon_Handler(void);

#ifdef __cplusplus
}
#endif
#endif

