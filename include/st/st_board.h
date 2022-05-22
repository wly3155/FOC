#ifndef __FOC_BOARD_H__
#define __FOC_BOARD_H__

#ifdef __cplusplus
extern "c" {
#endif

enum chip_pin {
	CHIP_PIN_23 = 23,
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

#define LED_G_PIN				(CHIP_PIN_33)
#define LED_R_PIN				(CHIP_PIN_40)

void prvSetupHardware(void);

#ifdef __cplusplus
}
#endif
#endif