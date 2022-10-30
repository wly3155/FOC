#ifndef __ST_BOARD_H__
#define __ST_BOARD_H__

#include "stm32f4xx.h"

#ifdef __cplusplus
extern "c" {
#endif

enum chip_pin {
    CHIP_PIN_14 = 14,
    CHIP_PIN_15 = 15,
    CHIP_PIN_16 = 16,
    CHIP_PIN_17 = 17,
    CHIP_PIN_23 = 23,
    CHIP_PIN_24 = 24,
    CHIP_PIN_25 = 25,
    CHIP_PIN_26 = 26,
    CHIP_PIN_27 = 27,
    CHIP_PIN_33 = 33,
    CHIP_PIN_34 = 34,
    CHIP_PIN_35 = 35,
    CHIP_PIN_36 = 36,
    CHIP_PIN_37 = 37,
    CHIP_PIN_38 = 38,
    CHIP_PIN_39 = 39,
    CHIP_PIN_40 = 40,
    CHIP_PIN_41 = 41,
    CHIP_PIN_42 = 42,
    CHIP_PIN_43 = 43,
    CHIP_PIN_58 = 58,
    CHIP_PIN_59 = 59,
    CHIP_PIN_MAX,
};

enum {
    RCC_APB1Periph,
    RCC_APB2Periph,
};

void prvSetupHardware(void);

#ifdef __cplusplus
}
#endif
#endif