#include <stdint.h>
#include <string.h>

#include <st/st_usart.h>
#include <st/st_gpio.h>

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"

static USART_TypeDef* usart_num = USART1;

static inline uint32_t chip_pin_to_usart_periphal_clock(enum chip_pin pin)
{
	switch (pin) {
	case CHIP_PIN_58:
		return RCC_APB2Periph_USART1;
	case CHIP_PIN_59:
		return RCC_APB2Periph_USART1;
	default:
		return 0;
	}

	return 0;
}

static inline USART_TypeDef* chip_pin_to_usart_num(enum chip_pin pin)
{
	switch (pin) {
	case CHIP_PIN_58:
		usart_num = USART1;
		return USART1;
	case CHIP_PIN_59:
		usart_num = USART1;
		return USART1;
	default:
		return NULL;
	}

	return NULL;
}

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

USART_TypeDef *usart = NULL;

void usart_init(uint8_t pin_tx, uint8_t pin_rx)
{
#if 0
	uint32_t periph_clock = 0;

	USART_InitTypeDef USART_InitStructure;

	gpio_set_af_mode(pin_tx, GPIO_AF_USART1);
	gpio_set_af_mode(pin_rx, GPIO_AF_USART1);

	gpio_set_mode(pin_tx, GPIO_Mode_AF);
	gpio_set_output_type(pin_tx, GPIO_OType_PP);
	gpio_set_output_pupd(pin_tx, GPIO_PuPd_UP);

	gpio_set_mode(pin_rx, GPIO_Mode_AF);
	gpio_set_output_type(pin_rx, GPIO_OType_PP);
	gpio_set_output_pupd(pin_rx, GPIO_PuPd_UP);

	periph_clock = chip_pin_to_usart_periphal_clock(pin_tx);
	RCC_AHB1PeriphClockCmd(periph_clock, ENABLE);

	usart = chip_pin_to_usart_num(pin_tx);
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(usart, &USART_InitStructure);
	USART_Cmd(usart, ENABLE);
#endif

	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	/* Enable UART clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* Connect PXx to USARTx_Tx*/
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);

	/* Connect PXx to USARTx_Rx*/
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

	/* Configure USART Tx as alternate function  */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure USART Rx as alternate function  */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* USART configuration */
	USART_Init(USART1, &USART_InitStructure);

	/* Enable USART */
	USART_Cmd(USART1, ENABLE);
}
