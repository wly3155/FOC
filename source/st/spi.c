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


#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_spi.h"

#define SPIx				SPI3
#define SPIx_CLK			RCC_APB1Periph_SPI3
#define SPIx_CLK_INIT			RCC_APB1PeriphClockCmd

#define SPIx_SCK_PIN			GPIO_Pin_10
#define SPIx_SCK_GPIO_PORT		GPIOC
#define SPIx_SCK_GPIO_CLK		RCC_AHB1Periph_GPIOC
#define SPIx_SCK_SOURCE			GPIO_PinSource10
#define SPIx_SCK_AF			GPIO_AF_SPI3

#define SPIx_MISO_PIN			GPIO_Pin_11
#define SPIx_MISO_GPIO_PORT		GPIOC
#define SPIx_MISO_GPIO_CLK		RCC_AHB1Periph_GPIOC
#define SPIx_MISO_SOURCE		GPIO_PinSource11
#define SPIx_MISO_AF			GPIO_AF_SPI3

#define SPIx_MOSI_PIN			GPIO_Pin_12
#define SPIx_MOSI_GPIO_PORT		GPIOC
#define SPIx_MOSI_GPIO_CLK		RCC_AHB1Periph_GPIOC
#define SPIx_MOSI_SOURCE		GPIO_PinSource12
#define SPIx_MOSI_AF			GPIO_AF_SPI3

struct spi_device {
	uint8_t *tx_buf;
	uint8_t *rx_buf;
};

static struct spi_device spi_dev;

static int spi_write_once(uint8_t spi_id, uint8_t data)
{
	while (SPI_I2S_GetITStatus(SPIx, SPI_I2S_IT_TXE) == RESET);
      	SPI_I2S_SendData(SPIx, data);
	while (SPI_I2S_GetITStatus(SPIx, SPI_I2S_IT_TXE) == RESET);
	return 0;
}

static uint8_t spi_read_once(uint8_t spi_id)
{
	while (SPI_I2S_GetITStatus(SPIx, SPI_I2S_IT_RXNE) == RESET);
	return SPI_I2S_ReceiveData(SPIx);
}

int spi_write(uint8_t spi_id, uint8_t reg, uint8_t data)
{
	SPI_Cmd(SPIx, ENABLE);
	spi_write_once(spi_id, reg);
	spi_write_once(spi_id, data);
	SPI_Cmd(SPIx, DISABLE);
	return 0;
}

int spi_read(uint8_t spi_id, uint8_t reg, uint8_t **data)
{
	SPI_Cmd(SPIx, ENABLE);
	spi_write_once(spi_id, reg);
	spi_dev->rx_buf[0] = spi_read_once(spi_id);
	SPI_Cmd(SPIx, DISABLE);
	*data = spi_dev.rx_buf;
	return 0;
}

int spi_wrapper_multi_read(uint8_t spi_id, uint8_t reg, uint8_t **data, uint8_t len)
{
	uint8_t i = 0;

	SPI_Cmd(SPIx, ENABLE);
	spi_write_once(spi_id, reg);
	for (i = 0; i < len; i++)
		spi_dev.rx_buf[i] = spi_read_once(spi_id);
	SPI_Cmd(SPIx, DISABLE);
	*data = spi_dev.rx_buf;
	return 0;
}

int spi_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Peripheral Clock Enable -------------------------------------------------*/
	/* Enable the SPI clock */
	SPIx_CLK_INIT(SPIx_CLK, ENABLE);

	/* Enable GPIO clocks */
	RCC_AHB1PeriphClockCmd(SPIx_SCK_GPIO_CLK | SPIx_MISO_GPIO_CLK | SPIx_MOSI_GPIO_CLK, ENABLE);

	/* SPI GPIO Configuration --------------------------------------------------*/
	/* GPIO Deinitialisation */
	GPIO_DeInit(SPIx_SCK_GPIO_PORT);
	GPIO_DeInit(SPIx_MISO_GPIO_PORT);
	GPIO_DeInit(SPIx_MOSI_GPIO_PORT);

	/* Connect SPI pins to AF5 */
	GPIO_PinAFConfig(SPIx_SCK_GPIO_PORT, SPIx_SCK_SOURCE, SPIx_SCK_AF);
	GPIO_PinAFConfig(SPIx_MISO_GPIO_PORT, SPIx_MISO_SOURCE, SPIx_MISO_AF);   
	GPIO_PinAFConfig(SPIx_MOSI_GPIO_PORT, SPIx_MOSI_SOURCE, SPIx_MOSI_AF);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

	/* SPI SCK pin configuration */
	GPIO_InitStructure.GPIO_Pin = SPIx_SCK_PIN;
	GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStructure);

	/* SPI  MISO pin configuration */
	GPIO_InitStructure.GPIO_Pin =  SPIx_MISO_PIN;
	GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStructure);

	/* SPI  MOSI pin configuration */
	GPIO_InitStructure.GPIO_Pin =  SPIx_MOSI_PIN;
	GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStructure);

	/* SPI configuration -------------------------------------------------------*/
	SPI_I2S_DeInit(SPIx);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_Init(SPIx, &SPI_InitStructure);
}
