#ifndef __BLDC_DEVICE_H__
#define __BLDC_DEVICE_H__

#ifdef __cplusplus
extern "c" {
#endif

enum phase {
	PHASE_A,
	PHASE_B,
	PHASE_C,
	PHASE_MAX,
};

enum phase_encoder{
	encoder_low,
	encoder_high,
	encoder_pwm,
};

int bldc_device_init(void);

#ifdef __cplusplus
}
#endif
#endif