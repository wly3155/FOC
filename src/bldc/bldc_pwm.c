#include "FreeRTOS.h"
#include "task.h"

#include <st/st_timer.h>
#include <st/st_pwm.h>

#include "log.h"
#include "time.h"
#include "bldc_resource.h"
#include "bldc_pwm.h"

#define ptr_to_bldc_id(ptr)				((uint8_t)((uint32_t)ptr))
#define bldc_id_to_ptr(id)				((void *)((uint32_t)id))

static struct bldc_pwm bldc_pwm_param[MAX_DEV_NUM] =
{
	{
		.base_timer = NULL,
		.base_timer_id = TIMER1,
		.high[PHASE_A] = AXIS0_INH_A_PIN,
		.high[PHASE_B] = AXIS0_INH_B_PIN,
		.high[PHASE_C] = AXIS0_INH_C_PIN,
		.low[PHASE_A] = AXIS0_INL_A_PIN,
		.low[PHASE_B] = AXIS0_INL_B_PIN,
		.low[PHASE_C] = AXIS0_INL_C_PIN,
	},
	{
		.base_timer = NULL,
		.base_timer_id = TIMER8,
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
	struct bldc_pwm *pwm_param = &bldc_pwm_param[bldc];
	int phase = 0;

	for (phase = 0; phase < PHASE_MAX; phase++) {
		pwm_enabledisable(pwm_param->high[phase], false);
		pwm_enabledisable(pwm_param->low[phase], false);
		pwm_set_freq(pwm_param->high[phase], freq_hz);
		pwm_set_freq(pwm_param->low[phase], freq_hz);
		pwm_enabledisable(pwm_param->high[phase], true);
		pwm_enabledisable(pwm_param->low[phase], true);
	}

	return 0;
}

int bldc_set_pwm_duty(uint8_t bldc, float duty_percent)
{
	struct bldc_pwm *pwm_param = &bldc_pwm_param[bldc];
	int phase = 0;

	for (phase = 0; phase < PHASE_MAX; phase++) {
		pwm_enabledisable(pwm_param->high[phase], false);
		pwm_enabledisable(pwm_param->low[phase], false);
		pwm_set_duty(pwm_param->high[phase], duty_percent, true);
		pwm_set_duty(pwm_param->low[phase], duty_percent, true);
		pwm_enabledisable(pwm_param->high[phase], true);
		pwm_enabledisable(pwm_param->low[phase], true);
	}

	return 0;
}

int bldc_pwm_enabledisable(uint8_t bldc, uint8_t enable)
{
	struct bldc_pwm *pwm_param = &bldc_pwm_param[bldc];
	int phase = 0;

	for (phase = 0; phase < PHASE_MAX; phase++) {
		pwm_enabledisable(pwm_param->high[phase], enable);
		pwm_enabledisable(pwm_param->low[phase], enable);
	}
	return 0;
}

static uint64_t now = 0, last = 0;
static int bldc_pwm_irq_handler(void *private_data)
{
	uint8_t id = ptr_to_bldc_id(private_data);

	last = now;
	now = get_boot_time_ns();
	//logi("%s trigger %u, delta %lu\n", __func__, id, (uint32_t)(now - last));

	switch (id) {
	case BLDC_ID_0:
		break;
	case BLDC_ID_1:
		break;
	default:
		break;
	}

	return 0;
}

int bldc_pwm_init(uint8_t id, uint32_t freq_hz)
{
	struct bldc_pwm *pwm_param = &bldc_pwm_param[id];
	struct timer_device *timer_dev = NULL;
	int phase = 0;

	pwm_param->base_timer = request_timer_device(pwm_param->base_timer_id);
	configASSERT(pwm_param->base_timer);

	timer_dev = pwm_param->base_timer;
	timer_dev->freq_hz = freq_hz;
	timer_dev->irq_enable = true;
	timer_dev->irq_handler = bldc_pwm_irq_handler;
	timer_dev->private_data = bldc_id_to_ptr(id);
	timer_dev->irq_request_type = TIM_IT_Update;
	timer_init(timer_dev);

	for (phase = 0; phase < PHASE_MAX; phase++) {
		pwm_init(pwm_param->high[phase]);
		pwm_init(pwm_param->low[phase]);

		pwm_enabledisable(pwm_param->high[phase], false);
		pwm_enabledisable(pwm_param->low[phase], false);
	}
	return 0;
}