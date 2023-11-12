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

#include <FreeRTOS.h>
#include <stm32f4xx.h>
#include <stm32f4xx_syscfg.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_exti.h>
#include <misc.h>

#include "st/st_gpio.h"

enum {
    spi_id0,
    spi_id1,
    spi_id2,
};

static SPI_TypeDef *st_spi_get_base_addr(uint8_t spi_id)
{
    switch (spi_id) {
    case spi_id2:
        return SPI2;
    default:
        return NULL;
    }

    return NULL;
}

int st_spi_write(uint8_t spi_id, uint8_t addr, uint8_t data)
{
    return 0;
}

int st_spi_read(uint8_t spi_id, uint8_t addr, uint8_t data, uint8_t len)
{
    return 0;
}

void st_spi_init(uint8_t spi_id)
{
    SPI_InitTypeDef  SPI_InitStructure;
    SPI_TypeDef *spi_base_addr = st_spi_get_base_addr(spi_id);

    /*!< SPI configuration */
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;

    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(spi_base_addr, &SPI_InitStructure);

    /*!< Enable the sFLASH_SPI  */
    SPI_Cmd(spi_base_addr, ENABLE);
}