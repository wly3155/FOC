#include <stdio.h>
#include <st/st_pwm.h>
#include "bldc_resource.h"
#include "bldc_pwm.h"

static struct bldc_pwm_pin pwm_pin[BLDC_AXES] = 
{
	{
		.high[PHASE_A] = AXIS0_INH_A_PIN,
		.high[PHASE_B] = AXIS0_INH_B_PIN,
		.high[PHASE_C] = AXIS0_INH_C_PIN,
		.low[PHASE_A] = AXIS0_INL_A_PIN,
		.low[PHASE_B] = AXIS0_INL_B_PIN,
		.low[PHASE_C] = AXIS0_INL_C_PIN,
	},
	{
		.high[PHASE_A] = AXIS1_INH_A_PIN,
		.high[PHASE_B] = AXIS1_INH_B_PIN,
		.high[PHASE_C] = AXIS1_INH_C_PIN,
		.low[PHASE_A] = AXIS1_INL_A_PIN,
		.low[PHASE_B] = AXIS1_INL_B_PIN,
		.low[PHASE_C] = AXIS1_INL_C_PIN,
	},
};

int bldc_set_pwm_freq(uint8_t bldc, uint32_t freq_hz)
{
	struct bldc_pwm_pin *pin = NULL;
	int phase = 0;

	pin = &pwm_pin[bldc];
	for (phase = 0; phase < PHASE_MAX; phase++) {
		pwm_enabledisable(pin->high[phase], false);
		pwm_enabledisable(pin->low[phase], false);
		pwm_set_freq(pin->high[phase], freq_hz);
		pwm_set_freq(pin->low[phase], freq_hz);
		pwm_enabledisable(pin->high[phase], true);
		pwm_enabledisable(pin->low[phase], true);
	}
	return 0;
}

int bldc_set_pwm_duty(uint8_t bldc, float duty_percent)
{
	struct bldc_pwm_pin *pin = NULL;
	int phase = 0;

	pin = &pwm_pin[bldc];
	for (phase = 0; phase < PHASE_MAX; phase++) {
		pwm_enabledisable(pin->high[phase], false);
		pwm_enabledisable(pin->low[phase], false);
		pwm_set_duty(pin->high[phase], duty_percent, true);
		pwm_set_duty(pin->low[phase], duty_percent, true);
		pwm_enabledisable(pin->high[phase], true);
		pwm_enabledisable(pin->low[phase], true);
	}
	return 0;
}

int bldc_pwm_enabledisable(uint8_t bldc, uint8_t enable)
{
	struct bldc_pwm_pin *pin = NULL;
	int phase = 0;

	pin = &pwm_pin[bldc];
	for (phase = 0; phase < PHASE_MAX; phase++) {
		pwm_enabledisable(pin->high[phase], enable);
		pwm_enabledisable(pin->low[phase], enable);
	}
	return 0;
}

int bldc_pwm_init(uint8_t bldc, uint32_t freq_hz)
{
	struct bldc_pwm_pin *pin = NULL;
	int phase = 0;

	pin = &pwm_pin[bldc];
	for (phase = 0; phase < PHASE_MAX; phase++) {
		pwm_init(pin->high[phase]);
		pwm_init(pin->low[phase]);
		pwm_set_freq(pin->high[phase], freq_hz);
		pwm_set_freq(pin->low[phase], freq_hz);
		pwm_enabledisable(pin->high[phase], false);
		pwm_enabledisable(pin->low[phase], false);
	}
	return 0;
}