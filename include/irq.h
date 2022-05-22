#ifndef __IRQ_H__
#define __IRQ_H__

#ifdef __cplusplus
 extern "C" {
#endif 

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

