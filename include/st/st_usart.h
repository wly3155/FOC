
#ifndef __ST_USART_H__
#define __ST_USART_H__

#ifdef __cplusplus
extern "c" {
#endif

int usart_printf(char *ptr, int len);
uint16_t usart_receive(void);
void usart_init(uint8_t pin_tx, uint8_t pin_rx);

#ifdef __cplusplus
}
#endif
#endif