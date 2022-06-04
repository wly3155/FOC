#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include <st/st_timer.h>
#include "irq.h"
#include "log.h"
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

struct bldc_manager {
	QueueHandle_t event_queue;
	struct timer_device *timer_dev;
	struct bldc_device bldc_dev[MAX_DEV_NUM];
};

static struct bldc_manager bldc_mgr;

static int bldc_update_step(struct bldc_manager *mgr, struct bldc_event *event)
{
	uint8_t id = event->header.id;
	struct bldc_device *dev = mgr->bldc_dev;

	dev[id].to_step = (dev->to_step + 1) % TOTAL_STEPS_PRE_CYCLE;
	return 0;
}

static int bldc_device_enable(struct bldc_manager *mgr, struct bldc_event *event)
{
	uint8_t id = 0;
	struct bldc_header *header = &event->header;
	struct bldc_control *ctrl = &event->ctrl;
	struct bldc_device *dev = mgr->bldc_dev;

	id = header->id;
	dev[id].status = STATUS_RUNNING;
	dev[id].duty = ctrl->duty;
	bldc_set_pwm_duty(id, ctrl->duty);
	bldc_pwm_enabledisable(id, true);
	return 0;
}

static int bldc_device_disable(struct bldc_manager *mgr, struct bldc_event *event)
{
	uint8_t id = 0;
	struct bldc_header *header = &event->header;
	struct bldc_control *ctrl = &event->ctrl;
	struct bldc_device *dev = mgr->bldc_dev;

	id = header->id;
	dev[id].status = STATUS_IDLE;
	dev[id].duty = ctrl->duty;
	bldc_pwm_enabledisable(id, false);
	bldc_set_pwm_duty(id, ctrl->duty);
	return 0;
}

static int bldc_handle_event(struct bldc_manager *mgr, struct bldc_event *event)
{
	int ret = 0;

	switch (event->event_type) {
	case EVENT_DISABLE:
		ret = bldc_device_disable(mgr, event);
		break;
	case EVENT_ENABLE:
		ret = bldc_device_enable(mgr, event);
		break;
	case EVENT_CALIBRATE:
		break;
	case EVENT_CONFIG:
		break;
	case EVENT_UPDATE:
		bldc_update_step(mgr, event);
		break;
	}
	return ret;
}

static int bldc_device_init(void)
{
	int id = 0;
	struct bldc_manager *mgr = &bldc_mgr;
	struct bldc_device *dev = mgr->bldc_dev;

	for (id = 0; id < MAX_DEV_NUM; id++) {
		dev[id].to_step = 0;
		dev[id].status = STATUS_IDLE;
		dev[id].freq = BLDC_PWM_FREQ_HZ;
		bldc_pwm_init(id, BLDC_PWM_FREQ_HZ);
	}

	return 0;
}

static int bldc_timer_handler(void *private_data)
{
	return 0;
}

static void bldc_task(void *param)
{
	struct bldc_manager *mgr = (struct bldc_manager *)param;
	struct timer_device *timer_dev = NULL;
	struct bldc_event event;

	logi("%s init\n", __func__);
	bldc_device_init();

	mgr->timer_dev = request_timer_device(TIMER2);
	configASSERT(mgr->timer_dev);

	timer_dev = mgr->timer_dev;
	timer_dev->freq_hz = 1000;
	timer_dev->irq_enable = true;
	timer_dev->irq_request_type = TIM_IT_Update;
	timer_dev->irq_handler = bldc_timer_handler;
	//timer_init(timer_dev);
	//timer_start(timer_dev);
	while (1) {
		xQueueReceive(mgr->event_queue, &event, portMAX_DELAY);
		bldc_handle_event(mgr, &event);
	}
}

static int bldc_enabledisable(uint8_t id, float duty, uint8_t en)
{
	struct bldc_event event;
	BaseType_t xHigherPriorityTaskWoken;
	struct bldc_manager *mgr = &bldc_mgr;

	event.event_type = en ? EVENT_ENABLE : EVENT_DISABLE;
	event.header.id = id;
	event.ctrl.duty = duty;

	if (is_in_isr()) {
		xQueueSendFromISR(mgr->event_queue, &event, &xHigherPriorityTaskWoken);
 		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	} else {
		xQueueSend(mgr->event_queue, &event, config_BLDC_SEND_QUEUE_WAIT);
	}

	return 0;
}

int bldc_enable(uint8_t id, float duty)
{
	return bldc_enabledisable(id, duty, true);
}

int bldc_disble(uint8_t id)
{
	return bldc_enabledisable(id, 0, false);
}

void bldc_init(void)
{
	struct bldc_manager *mgr = &bldc_mgr;

	mgr->event_queue = xQueueCreate(config_BLDC_QUEUE_EVENT, sizeof(struct bldc_event));
	configASSERT(mgr->event_queue);

	xTaskCreate(bldc_task, "bldc", config_BLDC_TASK_STACK_SIZE, ( void * ) mgr, config_BLDC_TASK_PRI, NULL);
}
