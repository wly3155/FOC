/*
 * Copyright (C) <2022>  <wuliyong3155@163.com>

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include <string.h>

#include <st/st_usart.h>
#include <st/st_gpio.h>

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"

static USART_TypeDef* usart_num = USART1;

static void usart_send_wait_for_complete()
{
    while (!USART_GetFlagStatus(usart_num, USART_FLAG_TC));
}

static void usart_send(uint16_t data)
{
    USART_SendData(usart_num, data);
}

int usart_printf(char *ptr, int len)
{
    int i = 0;

    for (i = 0; i < len; i++) {
        usart_send_wait_for_complete();
        usart_send((uint8_t)ptr[i]);
    }

    return 0;
}

uint16_t usart_receive(void)
{
    return USART_ReceiveData(usart_num);
}

void usart_init(uint8_t pin_tx, uint8_t pin_rx)
{
    USART_InitTypeDef USART_InitStructure;

    gpio_set_mode(pin_tx, MODE_ATTR(GPIO_AF_USART1, MODE_ALTER));
    gpio_set_mode(pin_rx, MODE_ATTR(GPIO_AF_USART1, MODE_ALTER));

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    USART_Cmd(USART1, ENABLE);
}
