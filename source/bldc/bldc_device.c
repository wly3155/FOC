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

#include <errno.h>
#include "FreeRTOS.h"
#include "task.h"

#include "bldc_device.h"
#include "bldc_manager.h"
#include "bldc_resource.h"
#include "foc_board.h"
#include "irq.h"
#include "time.h"
#include "log.h"

#include <st/st_timer.h>
#include <st/st_pwm.h>
#include <st/st_exti.h>

#define BLDC_PWM_FREQ_HZ                (20000)

#define ptr_to_bldc_id(ptr)             ((uint8_t)((uint32_t)ptr))
#define bldc_id_to_ptr(id)              ((void *)((uint32_t)id))
#define phase_to_channel(phase)         (phase + 1)

enum BLDC_STEP_ENCODER {
    BLDC_HLZ, /* step1, ab */
    BLDC_HZL, /* step2, ac */
    BLDC_ZHL, /* step3, bc */
    BLDC_LHZ, /* step4, ba */
    BLDC_LZH, /* step5, ca */
    BLDC_ZLH, /* step6, cb */
    BLDC_STEPS_PRE_CYCLE,
};

struct bldc_device {
    uint8_t status;
    int8_t error_num;
    uint8_t direction;
    uint8_t to_step;
	uint8_t encoder[PHASE_MAX];
    uint32_t freq;
    float duty;

    uint8_t base_timer_id;
    uint8_t pwm_group;
    enum chip_pin pwm_high[PHASE_MAX];
    enum chip_pin pwm_low[PHASE_MAX];
	enum chip_pin zcd;
    uint8_t zcd_irq_num;
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
        .zcd = AXIS0_ZCD_PIN,
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
        .zcd = AXIS1_ZCD_PIN,
    },
};

static int bldc_update(uint8_t id)
{
    struct bldc_event event;

    event.event_type = EVENT_UPDATE;
    event.header.id = id;
    return bldc_event_enqueue(&event);
}

static int bldc_set_phase_encoder(uint8_t bldc, uint8_t phase, uint8_t encoder)
{
    struct bldc_device *dev = &bldc_dev[bldc];
    uint8_t group = dev->pwm_group;
    uint8_t channel = phase_to_channel(phase);

	if (dev->encoder[phase] == encoder)
		return 0;

	dev->encoder[phase] = encoder;

    switch (encoder) {
    case encoder_idle:
        pwm_config_mode(group, channel, PWM_BOTH_LOW_MODE);
        break;
    case encoder_pwm:
        pwm_config_mode(group, channel, PWM_MODE);
        break;
    case encoder_low:
        pwm_config_mode(group, channel, PWM_DC_LOW_MODE);
        break;
    case encoder_high:
        pwm_config_mode(group, channel, PWM_DC_HIGH_MODE);
        break;
    }

    return 0;
}

static int bldc_pwm_enabledisable(uint8_t bldc, bool en)
{
    logi("%s id %u en %u\n", __func__, bldc, en);
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
    case BLDC_HLZ:
        bldc_set_phase_encoder(header->id, PHASE_A, encoder_pwm);
        bldc_set_phase_encoder(header->id, PHASE_B, encoder_low);
        bldc_set_phase_encoder(header->id, PHASE_C, encoder_idle);
        break;
    case BLDC_HZL:
        bldc_set_phase_encoder(header->id, PHASE_A, encoder_pwm);
        bldc_set_phase_encoder(header->id, PHASE_B, encoder_idle);
        bldc_set_phase_encoder(header->id, PHASE_C, encoder_low);
        break;
    case BLDC_ZHL:
        bldc_set_phase_encoder(header->id, PHASE_A, encoder_low);
        bldc_set_phase_encoder(header->id, PHASE_B, encoder_low);
        bldc_set_phase_encoder(header->id, PHASE_C, encoder_pwm);
        break;
    case BLDC_LHZ:
        bldc_set_phase_encoder(header->id, PHASE_A, encoder_low);
        bldc_set_phase_encoder(header->id, PHASE_B, encoder_pwm);
        bldc_set_phase_encoder(header->id, PHASE_C, encoder_idle);
        break;
    case BLDC_LZH:
        bldc_set_phase_encoder(header->id, PHASE_A, encoder_low);
        bldc_set_phase_encoder(header->id, PHASE_B, encoder_idle);
        bldc_set_phase_encoder(header->id, PHASE_C, encoder_pwm);
        break;
    case BLDC_ZLH:
        bldc_set_phase_encoder(header->id, PHASE_A, encoder_idle);
        bldc_set_phase_encoder(header->id, PHASE_B, encoder_low);
        bldc_set_phase_encoder(header->id, PHASE_C, encoder_pwm);
        break;
    }

    return 0;
}

static int bldc_device_startup(uint8_t id)
{
    /* TBD */
    struct bldc_device *dev = &bldc_dev[id];
    uint8_t group = dev->pwm_group;

    pwm_config_mode(group, PWM_CHANNEL1, PWM_MODE);
    pwm_config_mode(group, PWM_CHANNEL2, PWM_MODE);
    pwm_config_mode(group, PWM_CHANNEL3, PWM_MODE);
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

    dev->status = STATUS_IDLE;
    return bldc_device_stop(header->id);
}

static int bldc_device_enable(struct bldc_event *event)
{
    struct bldc_header *header = &event->header;
    struct bldc_device *dev = &bldc_dev[header->id];
    struct bldc_control *ctrl = &event->ctrl;

    dev->duty = ctrl->duty;
    if (dev->status == STATUS_IDLE) {
        pwm_set_duty(header->id, ctrl->duty);
        bldc_device_startup(header->id);
    }

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

    pwm_bind_timer(dev->pwm_group, dev->base_timer_id, freq_hz);
    pwm_bind_pin(dev->pwm_group, PWM_CHANNEL1, dev->pwm_high[PHASE_A], dev->pwm_low[PHASE_A]);
    pwm_bind_pin(dev->pwm_group, PWM_CHANNEL2, dev->pwm_high[PHASE_B], dev->pwm_low[PHASE_B]);
    pwm_bind_pin(dev->pwm_group, PWM_CHANNEL3, dev->pwm_high[PHASE_C], dev->pwm_low[PHASE_C]);

    pwm_init(dev->pwm_group, PWM_CHANNEL1);
    pwm_init(dev->pwm_group, PWM_CHANNEL2);
    pwm_init(dev->pwm_group, PWM_CHANNEL3);
    return 0;
}

#ifdef CFG_BLDC_ZERO_CROSS_DETECTOR_SUPPORT
static int bldc_device_zcd_irq_handler(void *private_data)
{
    return bldc_update(ptr_to_bldc_id(private_data));
}

static int bldc_device_zcd_init(uint8_t id)
{
    struct bldc_device *dev = &bldc_dev[id];
    uint8_t irq_num = 0;

    gpio_set_input_pupd(dev->zcd, NO_PULL);
    gpio_set_mode(dev->zcd, MODE_INPUT);
    irq_num = exti_init(dev->zcd);
    if (irq_num < 0)
        return -EINVAL;

    irq_register(irq_num, bldc_device_zcd_irq_handler, (void *)irq_num);
    return 0;
}
#endif

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
        bldc_device_pwm_init(id, BLDC_PWM_FREQ_HZ);
		//bldc_device_zcd_init(id);
    }

    return 0;

dev_register_fail:
    return ret;
}

