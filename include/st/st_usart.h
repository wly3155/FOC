
#ifndef __ST_USART_H__
#define __ST_USART_H__

#ifdef __cplusplus
extern "c" {
#endif

void usart_send_wait_for_complete(void);
void usart_send(uint16_t data);
uint16_t usart_receive(void);
void usart_init(uint8_t pin_tx, uint8_t pin_rx);

#ifdef __cplusplus
}
#endif
#endif