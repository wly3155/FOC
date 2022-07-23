#ifndef __ST_TIMER_H__
#define __ST_TIMER_H__

#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx_tim.h"

#ifdef __cplusplus
extern "c" {
#endif

#define CCMR_MAX                            (2)
#define CCR_CHANNEL_MAX                        (4)

enum TIMER_INDEX {
    TIMER_INVALID = 0,
    TIMER1,
    TIMER2,
    TIMER3,
    TIMER4,
    TIMER5,
    TIMER6,
    TIMER7,
    TIMER8,
    TIMER9,
    TIMER10,
    TIMER11,
    TIMER12,
    TIMER13,
    TIMER14,
    TIMER_MAX,
};

enum {
    CHANNEL_1 = 0,
    CHANNEL_2,
    CHANNEL_3,
    CHANNEL_4,
    MAX_CHANNEL,
};

struct timer_reg_t {
    uint32_t cr1;
    uint32_t cr2;
    uint32_t smcr;
    uint32_t dier;
    uint32_t sr;
    uint32_t egr;
    uint32_t ccmr[CCMR_MAX];
    uint32_t ccer;
    uint32_t cnt;
    uint32_t psc;
    uint32_t arr;
    uint32_t ccr[CCR_CHANNEL_MAX];
    uint32_t dcr;
    uint32_t dmar;
};

struct timer_device {
    uint8_t id;
    bool in_use;
    uint32_t freq_hz;
    uint32_t reg_base;
    struct timer_reg_t regs;
    TIM_TypeDef *base_addr;
    uint32_t periph_clock;
    uint32_t periph_clock_group;
    bool irq_enable;
    uint32_t irq_num;
    uint32_t irq_request_type;
    int (*irq_handler)(void *private_data);
    void *private_data;
};

struct timer_device *request_timer_device(uint8_t id);
int timer_init(struct timer_device *dev);
int timer_start(struct timer_device *dev);
int timer_stop(struct timer_device *dev);

int timer_input_capture_init(struct timer_device *dev, uint8_t channel);
int timer_input_capture_enable(struct timer_device *dev, uint8_t channel);
int timer_input_capture_disable(struct timer_device *dev, uint8_t channel);

int timer_platform_init(void);
#ifdef __cplusplus
}
#endif
#endif