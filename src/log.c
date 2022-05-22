#include <stdio.h>
#include <stdint.h>

#include <st/st_usart.h>

int _writsadfase(int fd, char *ptr, int len)
{
	int i = 0;

	for (i = 0; i < len; i++) {
		usart_send_wait_for_complete();
		usart_send((uint8_t)ptr[i]);
	}

	return 0;
}