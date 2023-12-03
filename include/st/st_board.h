#ifndef __ST_BOARD_H__
#define __ST_BOARD_H__

#include "stm32f4xx.h"

#ifdef __cplusplus
extern "c" {
#endif

enum chip_pin {
    CHIP_PIN_1,
    CHIP_PIN_2,
    CHIP_PIN_3,
    CHIP_PIN_4,
    CHIP_PIN_5,
    CHIP_PIN_6,
    CHIP_PIN_7,
    CHIP_PIN_8,
    CHIP_PIN_9,
    CHIP_PIN_10,
    CHIP_PIN_11,
    CHIP_PIN_12,
    CHIP_PIN_13,
    CHIP_PIN_14,
    CHIP_PIN_15,
    CHIP_PIN_16,
    CHIP_PIN_17,
    CHIP_PIN_18,
    CHIP_PIN_19,
    CHIP_PIN_20,
    CHIP_PIN_21,
    CHIP_PIN_22,
    CHIP_PIN_23,
    CHIP_PIN_24,
    CHIP_PIN_25,
    CHIP_PIN_26,
    CHIP_PIN_27,
    CHIP_PIN_28,
    CHIP_PIN_29,
    CHIP_PIN_30,
    CHIP_PIN_31,
    CHIP_PIN_32,
    CHIP_PIN_33,
    CHIP_PIN_34,
    CHIP_PIN_35,
    CHIP_PIN_36,
    CHIP_PIN_37,
    CHIP_PIN_38,
    CHIP_PIN_39,
    CHIP_PIN_40,
    CHIP_PIN_41,
    CHIP_PIN_42,
    CHIP_PIN_43,
    CHIP_PIN_44,
    CHIP_PIN_45,
    CHIP_PIN_46,
    CHIP_PIN_47,
    CHIP_PIN_48,
    CHIP_PIN_49,
    CHIP_PIN_50,
    CHIP_PIN_51,
    CHIP_PIN_52,
    CHIP_PIN_53,
    CHIP_PIN_54,
    CHIP_PIN_55,
    CHIP_PIN_56,
    CHIP_PIN_57,
    CHIP_PIN_58,
    CHIP_PIN_59,
    CHIP_PIN_60,
    CHIP_PIN_61,
    CHIP_PIN_62,
    CHIP_PIN_63,
    CHIP_PIN_64,
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