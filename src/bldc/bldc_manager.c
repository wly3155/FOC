#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include <st/st_timer.h>

#include "irq.h"
#include "log.h"
#include "utils.h"
#include "bldc_resource.h"
#include "bldc_manager.h"

#define config_BLDC_QUEUE_EVENT			(128)
#define config_BLDC_SEND_QUEUE_WAIT		(10)
#define config_BLDC_TASK_STACK_SIZE		(2048)
#define config_BLDC_TASK_PRI			(configMAX_PRIORITIES - 1)

struct bldc_manager {
	QueueHandle_t event_queue;
	struct bldc_device_interface *dev_interface;
};

static struct bldc_manager bldc_mgr;

static int bldc_event_enqueue(struct bldc_event *event)
{
	int ret = 0;
	BaseType_t xHigherPriorityTaskWoken;
	struct bldc_manager *mgr = &bldc_mgr;
	bool in_isr = false;
	if (is_in_isr()) {
		in_isr = true;
		ret = xQueueSendFromISR(mgr->event_queue, event, &xHigherPriorityTaskWoken);
 		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	} else {
		in_isr = false;
		ret = xQueueSend(mgr->event_queue, event, config_BLDC_SEND_QUEUE_WAIT);
	}

	configASSERT(ret);
	logi("%s %p event type %u %d %d\n", __func__, mgr->event_queue, event->event_type, in_isr, ret);
	return 0;
}

static int bldc_handle_event(struct bldc_manager *mgr, struct bldc_event *event)
{
	int ret = 0;
	struct bldc_device_interface *dev_interface = mgr->dev_interface;

	logi("%s event type %u\n", __func__, event->event_type);

	switch (event->event_type) {
	case EVENT_UPDATE:
		if (dev_interface->update_step)
			dev_interface->update_step(event);
		break;
	case EVENT_DISABLE:
		if (dev_interface->disable)
			dev_interface->disable(event);
		break;
	case EVENT_ENABLE:
		if (dev_interface->enable)
			dev_interface->enable(event);
		break;
	case EVENT_CONFIG:
		if (dev_interface->config)
			dev_interface->config(event);
		break;
	case EVENT_CALIBRATE:
		break;
	default:
		loge("%s unhandle event %u\n", __func__, event->event_type);
		break;
	}

	return ret;
}

static void bldc_task(void *param)
{
	int ret = 0;
	struct bldc_manager *mgr = (struct bldc_manager *)param;
	struct bldc_event event;

	while (1) {
		ret = xQueueReceive(mgr->event_queue, &event, portMAX_DELAY);
		if (!ret)
			continue;

		bldc_handle_event(mgr, &event);
	}
}

int bldc_update(uint8_t id)
{
	struct bldc_event event;

	event.event_type = EVENT_UPDATE;
	event.header.id = id;
	return bldc_event_enqueue(&event);
}

int bldc_enable(uint8_t id, float duty)
{
	struct bldc_event event;

	event.event_type = EVENT_ENABLE;
	event.header.id = id;
	event.ctrl.duty = duty;
	return bldc_event_enqueue(&event);
}

int bldc_disble(uint8_t id)
{
	struct bldc_event event;

	event.event_type = EVENT_DISABLE;
	event.header.id = id;
	event.ctrl.duty = 0;
	return bldc_event_enqueue(&event);
}

int bldc_config(uint8_t id, uint8_t cmd, void *data, uint8_t len)
{
	struct bldc_event event;

	event.event_type = EVENT_CONFIG;
	event.header.id = id;
	event.cfg.cmd = cmd;
	memcpy(event.cfg.data, data, min(sizeof(event.cfg.data), len));
	return bldc_event_enqueue(&event);
}

int bldc_manager_device_register(struct bldc_device_interface *interface)
{
	struct bldc_manager *mgr = &bldc_mgr;

	if (mgr->dev_interface)
		return -EEXIST;

	mgr->dev_interface = interface;
	return 0;
}

void bldc_manager_init(void)
{
	struct bldc_manager *mgr = &bldc_mgr;

	mgr->event_queue = xQueueCreate(config_BLDC_QUEUE_EVENT, sizeof(struct bldc_event));
	configASSERT(mgr->event_queue);
	logi("%s %p\n", __func__, mgr->event_queue);
	xTaskCreate(bldc_task, "bldc", config_BLDC_TASK_STACK_SIZE, (void *)mgr, config_BLDC_TASK_PRI, NULL);
}
