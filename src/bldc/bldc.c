#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "bldc_resource.h"
#include "bldc_pwm.h"
#include "bldc.h"

#define EVENT_UPDATE					(EVENT_PRIVATE_START + 1)

#define config_BLDC_QUEUE_EVENT			(128)
#define config_BLDC_SEND_QUEUE_WAIT		(10)
#define config_BLDC_TASK_STACK_SIZE		(1024)
#define config_BLDC_TASK_PRI			(configMAX_PRIORITIES - 1)

#define TOTAL_STEPS_PRE_CYCLE			(6)
#define BLDC_PWM_FREQ_HZ				(20000)
#define BLDC_PWM_MAX_DUTY				(100)

static QueueHandle_t bldc_event_queue;
static struct bldc_device dev_list[BLDC_AXES];

static int bldc_update_step(struct bldc_event *event)
{
	uint8_t id = event->header.id;
	struct bldc_device *dev = &dev_list[id];

	dev->to_step = (dev->to_step + 1) % TOTAL_STEPS_PRE_CYCLE;
	return 0;
}

static int bldc_device_enable(struct bldc_event *event)
{
	uint8_t id = 0;
	struct bldc_header *header = &event->header;
	struct bldc_control *ctrl = &event->ctrl;
	struct bldc_device *dev = NULL;

	id = header->id;
	dev = &dev_list[id];
	dev->status = EVENT_ENABLE;
	dev->duty = ctrl->duty;
	dev->freq = ctrl->freq;
	bldc_set_pwm_freq(id, ctrl->freq);
	bldc_set_pwm_duty(id, ctrl->duty);
	bldc_pwm_enabledisable(id, true);
	return 0;
}

static int bldc_device_disable(struct bldc_event *event)
{
	uint8_t id = 0;
	struct bldc_header *header = &event->header;
	struct bldc_control *ctrl = &event->ctrl;
	struct bldc_device *dev = NULL;

	id = header->id;
	dev = &dev_list[id];
	dev->status = EVENT_DISABLE;
	dev->duty = ctrl->duty;
	dev->freq = ctrl->freq;
	bldc_pwm_enabledisable(id, false);
	bldc_set_pwm_freq(id, ctrl->freq);
	bldc_set_pwm_duty(id, ctrl->duty);
	return 0;
}

static int bldc_handle_event(struct bldc_event *event)
{
	int ret = 0;

	switch (event->event_type) {
	case EVENT_DISABLE:
		ret = bldc_device_disable(event);
		break;
	case EVENT_ENABLE:
		ret = bldc_device_enable(event);
		break;
	case EVENT_CALIBRATE:
		break;
	case EVENT_CONFIG:
		break;
	case EVENT_UPDATE:
		bldc_update_step(event);
		break;
	}
	return ret;
}

static int bldc_device_init(void)
{
	int axis = 0;
	struct bldc_device *dev = NULL;

	for (axis = 0; axis < BLDC_AXES; axis++) {
		dev = &dev_list[axis];
		dev->to_step = 0;
		bldc_pwm_init(axis, BLDC_PWM_FREQ_HZ);
	}
	return 0;
}

static void bldc_task(void *param)
{
	struct bldc_event event;

	bldc_device_init();

	while (1) {
		memset(&event, 0x00, sizeof(event));
		xQueueReceive(bldc_event_queue, &event, portMAX_DELAY);
		bldc_handle_event(&event);
	}
}

void bldc_init(void)
{
	bldc_event_queue = xQueueCreate(config_BLDC_QUEUE_EVENT, sizeof(struct bldc_event));
	configASSERT(bldc_event_queue);

	xTaskCreate(bldc_task, "bldc", config_BLDC_TASK_STACK_SIZE, ( void * ) NULL, config_BLDC_TASK_PRI, NULL);
}

static int bldc_enabledisable(uint8_t id, uint32_t duty, uint32_t freq, uint8_t en)
{
	struct bldc_event event;
	BaseType_t xHigherPriorityTaskWoken;

	memset(&event, 0x00, sizeof(event));
	event.event_type = en ? EVENT_ENABLE : EVENT_DISABLE;
	event.header.id = id;
	event.ctrl.duty = duty % BLDC_PWM_MAX_DUTY;
	event.ctrl.freq = freq;

	if (is_in_isr()) {
		xQueueSendFromISR(bldc_event_queue, &event, &xHigherPriorityTaskWoken);
 		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	} else {
		xQueueSend(bldc_event_queue, &event, config_BLDC_SEND_QUEUE_WAIT);
	}

	return 0;
}

int bldc_enable(uint8_t id, uint32_t duty, uint32_t freq)
{
	return bldc_enabledisable(id, duty, freq, true);
}

int bldc_disble(uint8_t id)
{
	return bldc_enabledisable(id, UINT32_MAX, UINT32_MAX, false);
}