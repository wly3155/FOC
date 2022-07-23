/*
 * Copyright (C) <2022>  <wuliyong3155@163.com>

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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

#define BLDC_PWM_FREQ_HZ                (20000)
#define BLDC_PWM_MAX_DUTY               (100)

#define ptr_to_bldc_id(ptr)             ((uint8_t)((uint32_t)ptr))
#define bldc_id_to_ptr(id)              ((void *)((uint32_t)id))
#define phase_to_channel(phase)         (phase + 1)

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

struct bldc_device {
    uint8_t status;
    int8_t error_num;
    uint8_t direction;
    uint8_t to_step;
    uint32_t freq;
    float duty;

    uint8_t base_timer_id;
	uint8_t pwm_group;
    enum chip_pin pwm_high[PHASE_MAX];
    enum chip_pin pwm_low[PHASE_MAX];
};

static struct bldc_device bldc_dev[MAX_DEV_NUM] = {
    {
        .base_timer_id = TIMER1,
		.pwm_group = PWM_GROUP0,
        .pwm_high[PHASE_A] = AXIS0_INH_A_PIN,
        .pwm_high[PHASE_B] = AXIS0_INH_B_PIN,
        .pwm_high[PHASE_C] = AXIS0_INH_C_PIN,
        .pwm_low[PHASE_A] = AXIS0_INL_A_PIN,
        .pwm_low[PHASE_B] = AXIS0_INL_B_PIN,
        .pwm_low[PHASE_C] = AXIS0_INL_C_PIN,
    },
    {
        .base_timer_id = TIMER8,
        .pwm_group = PWM_GROUP1,
        .pwm_high[PHASE_A] = AXIS1_INH_A_PIN,
        .pwm_high[PHASE_B] = AXIS1_INH_B_PIN,
        .pwm_high[PHASE_C] = AXIS1_INH_C_PIN,
        .pwm_low[PHASE_A] = AXIS1_INL_A_PIN,
        .pwm_low[PHASE_B] = AXIS1_INL_B_PIN,
        .pwm_low[PHASE_C] = AXIS1_INL_C_PIN,
    },
};

static int bldc_set_phase_encoder(uint8_t bldc, enum phase p, enum phase_encoder e, float duty_percent)
{
    struct bldc_device *dev = &bldc_dev[bldc];

    switch (e) {
    case encoder_high:
    case encoder_low:
        pwm_set_complementary_output(dev->pwm_group, phase_to_channel(p), e, ~e);
        break;
    case encoder_pwm:
        pwm_set_duty(dev->pwm_group, phase_to_channel(p), duty_percent, true);
        break;
    }

    return 0;
}

static int bldc_pwm_enabledisable(uint8_t bldc, bool en)
{
    struct bldc_device *dev = &bldc_dev[bldc];

    logi("%s id %u en %u\n", __func__, bldc, en);
    pwm_enabledisable(dev->pwm_group, en);
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
    struct bldc_device *dev = &bldc_dev[id];
    int phase = 0;

    pwm_bind_timer(dev->pwm_group, dev->base_timer_id);
	pwm_setup_timer_interrupt(dev->pwm_group, bldc_pwm_irq_handler,
		bldc_id_to_ptr(id), TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3);
    pwm_bind_pin(dev->pwm_group, PWM_CHANNEL0, dev->pwm_high[PHASE_A]);
    pwm_bind_pin(dev->pwm_group, PWM_CHANNEL1, dev->pwm_low[PHASE_A]);
    pwm_bind_pin(dev->pwm_group, PWM_CHANNEL2, dev->pwm_high[PHASE_B]);
    pwm_bind_pin(dev->pwm_group, PWM_CHANNEL3, dev->pwm_low[PHASE_B]);
    pwm_bind_pin(dev->pwm_group, PWM_CHANNEL4, dev->pwm_high[PHASE_C]);
    pwm_bind_pin(dev->pwm_group, PWM_CHANNEL5, dev->pwm_low[PHASE_C]);
    pwm_init(dev->pwm_group, PWM_CHANNEL0);
    pwm_init(dev->pwm_group, PWM_CHANNEL1);
    pwm_init(dev->pwm_group, PWM_CHANNEL2);
    pwm_init(dev->pwm_group, PWM_CHANNEL3);
    pwm_init(dev->pwm_group, PWM_CHANNEL4);
    pwm_init(dev->pwm_group, PWM_CHANNEL5);


    for (phase = 0; phase < PHASE_MAX; phase++)
        pwm_enabledisable(dev->pwm_group, false);

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
        bldc_device_pwm_init(dev[id].pwm_group, BLDC_PWM_FREQ_HZ);
    }

    return 0;

dev_register_fail:
    return ret;
}
