#ifndef __LED_H__
#define __LED_H__

#ifdef __cplusplus
extern "c" {
#endif

#ifdef CFG_ST_BOARD_SUPPORT
#include <st/st_board.h>
#include <st/st_gpio.h>
#endif

enum LED_PIN {
	LED_GREEN_PIN =	CHIP_PIN_33,
	LED_RED_PIN = CHIP_PIN_40,
};

void led_init(void);

#ifdef __cplusplus
}
#endif
#endif