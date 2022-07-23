#include "FreeRTOS.h"
#include "task.h"

#include <st/st_timer.h>
#include <st/st_pwm.h>

#include "log.h"
#include "time.h"
#include "foc_board.h"
#include "bldc_resource.h"
#include "bldc_manager.h"
#include "bldc_device.h"

#define BLDC_PWM_FREQ_HZ				(20000)
#define BLDC_PWM_MAX_DUTY				(100)

#define ptr_to_bldc_id(ptr)				((uint8_t)((uint32_t)ptr))
#define bldc_id_to_ptr(id)				((void *)((uint32_t)id))
#define phase_to_channel(phase)			(phase + 1)

/*
a,bc;100 4
ac,b;101 5
c,ab;001 1
cb,a;011 3
b,ca;010 2
ab,c;110 6
*/

enum BLDC_STEP_ENCODER {
	BLDC_100, /* step1 */
	BLDC_101, /* step2 */
	BLDC_001, /* step3 */
	BLDC_011, /* step4 */
	BLDC_010, /* step5 */
	BLDC_110, /* step6 */
	BLDC_STEPS_PRE_CYCLE,
};

struct bldc_device_pwm {
	struct timer_device *base_timer;
	uint8_t base_timer_id;
	enum chip_pin high[PHASE_MAX];
	enum chip_pin low[PHASE_MAX];
};

struct bldc_device {
	uint8_t status;
	int8_t error_num;
	uint8_t direction;
	uint8_t to_step;
	float duty;
	uint32_t freq;
	struct bldc_device_pwm *pwm_param;
};

static struct bldc_device_pwm bldc_pwm_param[MAX_DEV_NUM] =
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

static struct bldc_device bldc_dev[MAX_DEV_NUM];

static int bldc_set_phase_encoder(uint8_t bldc, enum phase p, enum phase_encoder e, float duty_percent)
{
	struct bldc_device_pwm *pwm_param = &bldc_pwm_param[bldc];
	struct timer_device *timer_dev = pwm_param->base_timer;

	switch (e) {
	case encoder_high:
	case encoder_low:
		pwm_set_complementary_output(timer_dev->base_addr, phase_to_channel(p), e, ~e);
		break;
	case encoder_pwm:
		pwm_set_duty(timer_dev->base_addr, phase_to_channel(p), duty_percent, true);
		break;
	}

	return 0;
}

static int bldc_pwm_enabledisable(uint8_t bldc, bool en)
{
	struct bldc_device_pwm *pwm_param = &bldc_pwm_param[bldc];
	struct timer_device *timer_dev = pwm_param->base_timer;

	logi("%s id %u en %u\n", __func__, bldc, en);
	pwm_enabledisable(timer_dev->base_addr, en);
	return 0;
}

static int bldc_pwm_irq_handler(void *private_data)
{
	uint8_t id = ptr_to_bldc_id(private_data);

	switch (id) {
	case BLDC_ID_0:
	case BLDC_ID_1:
		bldc_update(id);
		break;
	default:
		break;
	}

	return 0;
}

static int bldc_device_update_step(struct bldc_event *event)
{
	struct bldc_header *header = &event->header;
	struct bldc_device *dev = &bldc_dev[header->id];

	if (dev->direction == DIR_CW)
		dev->to_step = (dev->to_step + 1) % BLDC_STEPS_PRE_CYCLE;
	else
		dev->to_step = (dev->to_step - 1) % BLDC_STEPS_PRE_CYCLE;

	switch (dev->to_step) {
	case BLDC_100:
		//bldc_set_phase_encoder(header->id, PHASE_A, encoder_pwm, 0);
		bldc_set_phase_encoder(header->id, PHASE_B, encoder_low, 0);
		//bldc_set_phase_encoder(header->id, PHASE_C, encoder_low, 0);
		break;
	case BLDC_101:
		//bldc_set_phase_encoder(header->id, PHASE_A, encoder_pwm, 0);
		//bldc_set_phase_encoder(header->id, PHASE_B, encoder_low, 0);
		bldc_set_phase_encoder(header->id, PHASE_C, encoder_pwm, 0);
		break;
	case BLDC_001:
		bldc_set_phase_encoder(header->id, PHASE_A, encoder_low, 0);
		//bldc_set_phase_encoder(header->id, PHASE_B, encoder_low, 0);
		//bldc_set_phase_encoder(header->id, PHASE_C, encoder_pwm, 0);
		break;
	case BLDC_011:
		//bldc_set_phase_encoder(header->id, PHASE_A, encoder_low, 0);
		bldc_set_phase_encoder(header->id, PHASE_B, encoder_pwm, 0);
		//bldc_set_phase_encoder(header->id, PHASE_C, encoder_pwm, 0);
		break;
	case BLDC_010:
		//bldc_set_phase_encoder(header->id, PHASE_A, encoder_low, 0);
		//bldc_set_phase_encoder(header->id, PHASE_B, encoder_pwm, 0);
		bldc_set_phase_encoder(header->id, PHASE_C, encoder_low, 0);
		break;
	case BLDC_110:
		bldc_set_phase_encoder(header->id, PHASE_A, encoder_pwm, 0);
		//bldc_set_phase_encoder(header->id, PHASE_B, encoder_pwm, 0);
		//bldc_set_phase_encoder(header->id, PHASE_C, encoder_low, 0);
		break;
	}

	return 0;
}

static int bldc_device_startup(uint8_t id)
{
	/* TBD */
	bldc_pwm_enabledisable(id, true);
	return 0;
}

static int bldc_device_stop(uint8_t id)
{
	bldc_pwm_enabledisable(id, false);
	return 0;
}

static int bldc_device_disable(struct bldc_event *event)
{
	struct bldc_header *header = &event->header;
	struct bldc_device *dev = &bldc_dev[header->id];
	struct bldc_control *ctrl = &event->ctrl;

	dev->status = STATUS_IDLE;
	dev->duty = ctrl->duty;
	bldc_device_stop(header->id);
	return 0;
}

static int bldc_device_enable(struct bldc_event *event)
{
	struct bldc_header *header = &event->header;
	struct bldc_device *dev = &bldc_dev[header->id];
	struct bldc_control *ctrl = &event->ctrl;

	dev->duty = ctrl->duty;
	if (dev->status == STATUS_IDLE)
		bldc_device_startup(header->id);

	return 0;
}

static int bldc_device_config(struct bldc_event *event)
{
	struct bldc_header *header = &event->header;
	struct bldc_device *dev = &bldc_dev[header->id];
	struct bldc_config *cfg = &event->cfg;

	switch (cfg->cmd) {
	case DIRECTION:
		dev->direction = cfg->data[0];
		break;
	default:
		break;
	}

	return 0;
}

static struct bldc_device_interface device_interface = {
	.update_step = bldc_device_update_step,
	.enable = bldc_device_enable,
	.disable = bldc_device_disable,
	.config = bldc_device_config,
};

static int bldc_device_pwm_init(uint8_t id, uint32_t freq_hz)
{
	struct bldc_device_pwm *pwm_param = &bldc_pwm_param[id];
	struct timer_device *timer_dev = NULL;
	int phase = 0;

	pwm_param->base_timer = request_timer_device(pwm_param->base_timer_id);
	configASSERT(pwm_param->base_timer);

	timer_dev = pwm_param->base_timer;
	timer_dev->freq_hz = freq_hz;
	timer_dev->irq_enable = true;
	timer_dev->irq_handler = bldc_pwm_irq_handler;
	timer_dev->private_data = bldc_id_to_ptr(id);
	timer_dev->irq_request_type = TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3;
	timer_init(timer_dev);

	for (phase = 0; phase < PHASE_MAX; phase++) {
		pwm_init(pwm_param->high[phase]);
		pwm_init(pwm_param->low[phase]);
		pwm_enabledisable(timer_dev->base_addr, false);
	}

	return 0;
}

int bldc_device_init(void)
{
	int ret = 0;
	int id = 0;
	struct bldc_device *dev = bldc_dev;

	ret = bldc_manager_device_register(&device_interface);
	if (ret < 0)
		goto dev_register_fail;

	for (id = 0; id < MAX_DEV_NUM; id++) {
		dev[id].to_step = 0;
		dev[id].status = STATUS_IDLE;
		dev[id].freq = BLDC_PWM_FREQ_HZ;
		dev[id].pwm_param = &bldc_pwm_param[id];
		bldc_device_pwm_init(id, BLDC_PWM_FREQ_HZ);
	}

	return 0;

dev_register_fail:
	return ret;
}