#ifndef __BLDC_RESOURCE_H__
#define __BLDC_RESOURCE_H__

#ifdef __cplusplus
extern "c" {
#endif

#ifdef CFG_ST_BOARD_SUPPORT
#include <st/st_board.h>
#endif

#ifdef CFG_ST_BOARD_SUPPORT
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
#else
#define AXIS0_INH_A_PIN			(0)
#define AXIS0_INH_B_PIN			(0)
#define AXIS0_INH_C_PIN			(0)
#define AXIS0_INL_A_PIN			(0)
#define AXIS0_INL_B_PIN			(0)
#define AXIS0_INL_C_PIN			(0)

#define AXIS1_INH_A_PIN			(0)
#define AXIS1_INH_B_PIN			(0)
#define AXIS1_INH_C_PIN			(0)
#define AXIS1_INL_A_PIN			(0)
#define AXIS1_INL_B_PIN			(0)
#define AXIS1_INL_C_PIN			(0)
#endif

enum {
	BLDC_ID_0,
	BLDC_ID_1,
	MAX_DEV_NUM,
};

#ifdef __cplusplus
}
#endif
#endif