#ifndef __FOC_BOARD_H__
#define __FOC_BOARD_H__

#ifdef __cplusplus
extern "c" {
#endif

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#define LED_G_PIN				(GPIO_Pin_9)
#define LED_G_PIN_GROUP			(GPIOC)
#define LED_R_PIN				(GPIO_Pin_12)
#define LED_R_PIN_GROUP			(GPIOB)

#ifdef __cplusplus
}
#endif
#endif