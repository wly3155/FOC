#ifndef __ST_PWM_H__
#define __ST_PWM_H__

#include <stdbool.h>
#include <stdint.h>
#include <st/st_board.h>

#ifdef __cplusplus
extern "c" {
#endif

#define CLOCK_COUNT_FREQ			(21000000)
#define is_advance_timer(timer) ((timer == TIM1 || timer == TIM8) ? true : false)

enum pwm_channel {
	channel_1,
	channel_2,
	channel_3,
	channel_4,
	channel_max,
};

int pwm_init(enum chip_pin pin);
int pwm_deinit(enum chip_pin pin);
int pwm_set_freq(enum chip_pin pin, uint32_t freq_hz);
int pwm_set_duty(enum chip_pin pin, float duty_percent, bool dual_polarity);
int pwm_enabledisable(enum chip_pin pin, bool en);

#ifdef __cplusplus
}
#endif
#endif