#ifndef __FOC_BOARD_H__
#define __FOC_BOARD_H__

#include <st/st_board.h>

#ifdef __cplusplus
extern "c" {
#endif

#define AXIS0_INH_A_PIN			(CHIP_PIN_43)
#define AXIS0_INH_B_PIN			(CHIP_PIN_42)
#define AXIS0_INH_C_PIN			(CHIP_PIN_41)

#define AXIS0_INL_A_PIN			(CHIP_PIN_36)
#define AXIS0_INL_B_PIN			(CHIP_PIN_35)
#define AXIS0_INL_C_PIN			(CHIP_PIN_34)

#define AXIS1_INH_A_PIN			(CHIP_PIN_37)
#define AXIS1_INH_B_PIN			(CHIP_PIN_38)
#define AXIS1_INH_C_PIN			(CHIP_PIN_39)

#define AXIS1_INL_A_PIN			(CHIP_PIN_23)
#define AXIS1_INL_B_PIN			(CHIP_PIN_26)
#define AXIS1_INL_C_PIN			(CHIP_PIN_27)

#define AXIS0_CURRENT1_PIN		(CHIP_PIN_8)
#define AXIS0_CURRENT2_PIN		(CHIP_PIN_9)

#define AXIS1_CURRENT1_PIN		(CHIP_PIN_10)
#define AXIS2_CURRENT2_PIN		(CHIP_PIN_11)

#define AXIS0_ZCD_PIN			(CHIP_PIN_25)
#define AXIS1_ZCD_PIN			(CHIP_PIN_24)

#define USART_TX_PIN			(CHIP_PIN_58)
#define USART_RX_PIN			(CHIP_PIN_59)

#define INPUT_CAPTURE_A			(CHIP_PIN_14)
#define INPUT_CAPTURE_B			(CHIP_PIN_15)
#define INPUT_CAPTURE_C			(CHIP_PIN_16)
#define INPUT_CAPTURE_D			(CHIP_PIN_17)

enum LED_PIN {
    LED_GREEN_PIN = CHIP_PIN_33,
    LED_RED_PIN = CHIP_PIN_40,
};


#ifdef __cplusplus
}
#endif
#endif
