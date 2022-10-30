#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef __cplusplus
extern "c" {
#endif

#ifdef CFG_ST_BOARD_SUPPORT
#include <st/st_board.h>
#include <st/st_gpio.h>
#endif

void assert_failed(uint8_t* file, uint32_t line);

#ifdef __cplusplus
}
#endif
#endif
