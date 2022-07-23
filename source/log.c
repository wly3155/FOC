
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#include "FreeRTOS.h"
#include "task.h"
#include "time.h"
#include <irq.h>

#include "foc_board.h"
#include <st/st_board.h>
#include <st/st_usart.h>

#define LOG_BUF_SIZE            256

static char log_buf[LOG_BUF_SIZE] = {'\0'};

static void log_header_format(void)
{
    uint64_t now = get_boot_time_ns();
    uint32_t sec = now / 1000000000;
    uint32_t mini_sec = now % 1000000000 / 1000000;

    snprintf(log_buf, sizeof(log_buf), "[%lu.%03lu] ", sec, mini_sec);
}

int __wrap_printf(const char *fmt, ...)
{
    uint8_t log_buf_used = 0;
    char *log_buf_to_write = NULL;
    va_list args;

    log_header_format();
    log_buf_used = strlen(log_buf);
    log_buf_to_write = log_buf + log_buf_used;
    va_start(args, fmt);
    vsnprintf(log_buf_to_write, LOG_BUF_SIZE - log_buf_used, fmt, args);
#ifdef CFG_USART_SUPPORT
    usart_printf(log_buf, strlen(log_buf));
#endif
    va_end(args);
    return 0;
}

int log_init()
{
#ifdef CFG_USART_SUPPORT
    usart_init(USART_TX_PIN, USART_RX_PIN);
#endif
    return 0;
}
