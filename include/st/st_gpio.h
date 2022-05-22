
#ifndef __ST_GPIO_H__
#define __ST_GPIO_H__

#include <st/st_board.h>
#include "stm32f4xx_gpio.h"

#ifdef __cplusplus
extern "c" {
#endif

int gpio_set_mode(enum chip_pin pin, GPIOMode_TypeDef mode);
int gpio_set_output_type(enum chip_pin pin, GPIOOType_TypeDef otype);
int gpio_set_output_pupd(enum chip_pin pin, GPIOPuPd_TypeDef pupd);
int gpio_set_output_value(enum chip_pin pin, BitAction val);
int gpio_set_af_mode(enum chip_pin pin, uint8_t mode);

#ifdef __cplusplus
}
#endif
#endif