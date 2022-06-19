#ifndef __ST_TIMER_H__
#define __ST_TIMER_H__

#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx_tim.h"

#ifdef __cplusplus
extern "c" {
#endif

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

struct timer_device {
	uint8_t id;
	bool in_use;
	uint32_t freq_hz;
	TIM_TypeDef *base_addr;
	uint32_t periph_clock;
	uint32_t periph_clock_group;

	bool irq_enable;
	uint32_t irq_num;
	uint32_t irq_request_type;
	int (*irq_handler)(void *private_data);
	void *private_data;
};

int timer_start(struct timer_device *dev);
int timer_stop(struct timer_device *dev);
int timer_init(struct timer_device *dev);
struct timer_device *request_timer_device(uint8_t id);

#ifdef __cplusplus
}
#endif
#endif