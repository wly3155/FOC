
#ifndef __ST_INPUT_CAPTURE_H__
#define __ST_INPUT_CAPTURE_H__

#include <stdint.h>
#include <st/st_board.h>

#ifdef __cplusplus
extern "c" {
#endif

enum {
	INPUT_CAPTURE_GROUP0,
	MAX_INPUT_CAPTURE_GROUP,
};

enum {
	CHANNEL_1 = 0,
	CHANNEL_2,
	CHANNEL_3,
	CHANNEL_4,
	MAX_CHANNEL,
};

int input_capture_enable(uint8_t group, uint8_t channel);
int input_capture_disable(uint8_t group, uint8_t channel);
int input_capture_init(uint8_t group, uint8_t channel);
int input_capture_irq_register(uint8_t group, int (*irq_handler)(void *private_data), void *private_data);
int input_capture_bind_timer(uint8_t group, uint8_t timer_id);
int input_capture_bind_chip_pin(uint8_t group, uint8_t channel, enum chip_pin pin);

#ifdef __cplusplus
}
#endif
#endif