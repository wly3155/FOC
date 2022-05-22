#ifndef __BLDC_PWM_H__
#define __BLDC_PWM_H__

#ifdef __cplusplus
extern "c" {
#endif

enum {
	PHASE_A,
	PHASE_B,
	PHASE_C,
	PHASE_MAX,
};

struct bldc_pwm_pin {
	uint16_t high[PHASE_MAX];
	uint16_t low[PHASE_MAX];
};

int bldc_set_pwm_freq(uint8_t bldc, uint32_t freq_hz);
int bldc_set_pwm_duty(uint8_t bldc, float duty_percent);
int bldc_pwm_enabledisable(uint8_t bldc, uint8_t enable);
int bldc_pwm_init(uint8_t bldc, uint32_t freq_hz);

#ifdef __cplusplus
}
#endif
#endif