#include <errno.h>

#include "FreeRTOS.h"
#include "task.h"

#include <st/st_gpio.h>
#include <st/st_timer.h>
#include <st/st_input_capture.h>

struct input_capture_t {
	bool timer_inited;
	uint8_t timer_id;
	struct timer_device *timer_dev;
	uint32_t bind_pin[MAX_CHANNEL];
	int (*irq_handler)(void *private_data);
	void *private_data;
};

struct input_capture_t support_input_capture[MAX_INPUT_CAPTURE_GROUP];

static int get_alter_mode(uint8_t timer_id)
{
	switch (timer_id) {
	case TIMER1 ... TIMER2:
		return GPIO_ALTER_1;
	case TIMER3 ... TIMER5:
		return GPIO_ALTER_2;
	case TIMER8 ... TIMER11:
		return GPIO_ALTER_3;
	}

	return -EINVAL;
}

static int input_capture_irq_handler(void *private_data)
{
	return 0;
}

int input_capture_enable(uint8_t group, uint8_t channel)
{
	struct timer_device *timer_dev = NULL;

	timer_dev = support_input_capture[group].timer_dev;
	TIM_ITConfig(timer_dev->base_addr, 2 << channel, true);
	TIM_Cmd(timer_dev->base_addr, true);
	return 0;
}

int input_capture_disable(uint8_t group, uint8_t channel)
{
	struct timer_device *timer_dev = NULL;

	timer_dev = support_input_capture[group].timer_dev;
	TIM_Cmd(timer_dev->base_addr, false);
	TIM_ITConfig(timer_dev->base_addr, 2 << channel, false);
	return 0;
}

int input_capture_irq_register(uint8_t group, int (*irq_handler)(void *private_data), void *private_data)
{
	struct input_capture_t *input_capture = NULL;

	input_capture = &support_input_capture[group];
	input_capture->irq_handler = irq_handler;
	input_capture->private_data = private_data;
	return 0;
}

int input_capture_init(uint8_t group, uint8_t channel)
{
	struct input_capture_t *input_capture = NULL;
	struct timer_device *timer_dev = NULL;
	uint8_t alter_mode = 0;
	TIM_ICInitTypeDef TIM_ICInitStructure;

	input_capture = &support_input_capture[group];
	alter_mode = get_alter_mode(input_capture->timer_id);
	gpio_set_mode(input_capture->bind_pin[channel], MODE_ATTR(alter_mode, MODE_ALTER));

	input_capture->timer_dev = request_timer_device(input_capture->timer_id);
	configASSERT(input_capture->timer_dev);
	if (!input_capture->timer_inited) {
		timer_dev = input_capture->timer_dev;
		timer_dev->freq_hz = 1;
		timer_dev->irq_enable = true;
		timer_dev->irq_request_type = TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4;
		timer_dev->irq_handler = input_capture_irq_handler;
		timer_init(timer_dev);
		input_capture->timer_inited = true;
	}

	TIM_ICInitStructure.TIM_Channel = 2 << channel;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0x0;
	TIM_ICInit(TIM1, &TIM_ICInitStructure);
	return 0;
}

int input_capture_bind_timer(uint8_t group, uint8_t timer_id)
{
	struct input_capture_t *input_capture = NULL;

	input_capture = &support_input_capture[group];
	input_capture->timer_id = timer_id;
	return 0;
}

int input_capture_bind_chip_pin(uint8_t group, uint8_t channel, enum chip_pin pin)
{
	struct input_capture_t *input_capture = NULL;

	input_capture = &support_input_capture[group];
	input_capture->bind_pin[channel] = pin;
	return 0;
}