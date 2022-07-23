
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#include "FreeRTOS.h"
#include "task.h"
#include "time.h"
#include "foc_board.h"
#include <st/st_board.h>
#include <st/st_usart.h>

#define LOG_BUF_SIZE            256

static char log_buf[LOG_BUF_SIZE] = {'\0'};

#if 0
int _write(int fd, char *ptr, int len)
{
    usart_printf(ptr, len);
    return 0;
}

struct xPrintFlags
{
    int base;
    int width;
    int printLimit;
    unsigned
        pad : 8,
        letBase : 8,
        isSigned : 1,
        isNumber : 1,
        long32 : 1,
        long64 : 1;
};

struct SStringBuf
{
    char *str;
    const char *orgStr;
    const char *nulPos;
    int curLen;
    struct xPrintFlags flags;
};

static void strbuf_init( struct SStringBuf *apStr, char *apBuf, const char *apMaxStr )
{
    apStr->str = apBuf;
    apStr->orgStr = apBuf;
    apStr->nulPos = apMaxStr-1;
    apStr->curLen = 0;

    memset( &apStr->flags, '\0', sizeof( apStr->flags));
}

static __inline BaseType_t strbuf_printchar_inline( struct SStringBuf *apStr, int c )
{
    if( apStr->str == NULL )
    {
        vOutputChar( ( char ) c, xTicksToWait );
        if( c == 0 )
        {
            return pdFALSE;
        }
        apStr->curLen++;
        return pdTRUE;
    }
    if( apStr->str < apStr->nulPos )
    {
        *(apStr->str++) = c;
        if( c == 0 )
        {
            return pdFALSE;
        }
        apStr->curLen++;
        return pdTRUE;
    }
    if( apStr->str == apStr->nulPos )
    {
        *( apStr->str++ ) = '\0';
    }
    return pdFALSE;
}

static void tiny_print( struct SStringBuf *apBuf, const char *format, va_list args )
{
    char scr[2];

    for( ; ; )
    {
        int ch = *( format++ );

        if( ch != '%' )
        {
            do
            {
                /* Put the most like flow in a small loop */
                if( strbuf_printchar_inline( apBuf, ch ) == 0 )
                {
                    return;
                }
                ch = *( format++ );
            } while( ch != '%' );
        }
        ch = *( format++ );
        /* Now ch has character after '%', format pointing to next */

        if( ch == '\0' )
        {
            break;
        }
        if( ch == '%' )
        {
            if( strbuf_printchar( apBuf, ch ) == 0 )
            {
                return;
            }
            continue;
        }
        memset( &apBuf->flags, '\0', sizeof( apBuf->flags ) );

        if( ch == '-' )
        {
            ch = *( format++ );
            apBuf->flags.pad = PAD_RIGHT;
        }
        while( ch == '0' )
        {
            ch = *( format++ );
            apBuf->flags.pad |= PAD_ZERO;
        }
        if( ch == '*' )
        {
            ch = *( format++ );
            apBuf->flags.width = va_arg( args, int );
        }
        else
        {
            while( ch >= '0' && ch <= '9' )
            {
                apBuf->flags.width *= 10;
                apBuf->flags.width += ch - '0';
                ch = *( format++ );
            }
        }
        if( ch == '.' )
        {
            ch = *( format++ );
            if( ch == '*' )
            {
                apBuf->flags.printLimit = va_arg( args, int );
                ch = *( format++ );
            }
            else
            {
                while( ch >= '0' && ch <= '9' )
                {
                    apBuf->flags.printLimit *= 10;
                    apBuf->flags.printLimit += ch - '0';
                    ch = *( format++ );
                }
            }
        }
        if( apBuf->flags.printLimit == 0 )
        {
            apBuf->flags.printLimit--;  /* -1: make it unlimited */
        }
        if( ch == 's' )
        {
            register char *s = ( char * )va_arg( args, int );
            if( prints( apBuf, s ? s : "(null)" ) == 0 )
            {
                break;
            }
            continue;
        }
        if( ch == 'c' )
        {
            /* char are converted to int then pushed on the stack */
            scr[0] = ( char ) va_arg( args, int );

            if( strbuf_printchar( apBuf, scr[0] )  == 0 )
            {
                return;
            }

            continue;
        }
        if( ch == 'l' )
        {
            ch = *( format++ );
            apBuf->flags.long32 = 1;
            /* Makes not difference as u32 == long */
        }
        if( ch == 'L' )
        {
            ch = *( format++ );
            apBuf->flags.long64 = 1;
            /* Does make a difference */
        }
        apBuf->flags.base = 10;
        apBuf->flags.letBase = 'a';

        if( ch == 'd' || ch == 'u' )
        {
            apBuf->flags.isSigned = ( ch == 'd' );
#if    SPRINTF_LONG_LONG
            if( apBuf->flags.long64 != pdFALSE )
            {
                if( printll( apBuf, va_arg( args, long long ) ) == 0 )
                {
                    break;
                }
            } else
#endif    /* SPRINTF_LONG_LONG */
            if( printi( apBuf, va_arg( args, int ) ) == 0 )
            {
                break;
            }
            continue;
        }

        apBuf->flags.base = 16;        /* From here all hexadecimal */

        if( ch == 'x' && format[0] == 'i' && format[1] == 'p' )
        {
            format += 2;    /* eat the "xi" of "xip" */
            /* Will use base 10 again */
            if( printIp( apBuf, va_arg( args, int ) ) == 0 )
            {
                break;
            }
            continue;
        }
        if( ch == 'x' || ch == 'X' || ch == 'p' || ch == 'o' )
        {
            if( ch == 'X' )
            {
                apBuf->flags.letBase = 'A';
            }
            else if( ch == 'o' )
            {
                apBuf->flags.base = 8;
            }
#if    SPRINTF_LONG_LONG
            if( apBuf->flags.long64 != pdFALSE )
            {
                if( printll( apBuf, va_arg( args, long long ) ) == 0 )
                {
                    break;
                }
            } else
#endif    /* SPRINTF_LONG_LONG */
            if( printi( apBuf, va_arg( args, int ) ) == 0 )
            {
                break;
            }
            continue;
        }
    }
    strbuf_printchar( apBuf, '\0' );
}
/*-----------------------------------------------------------*/

int vsnprintf( char *apBuf, size_t aMaxLen, const char *apFmt, va_list args )
{
    struct SStringBuf strBuf;
    strbuf_init( &strBuf, apBuf, ( const char* )apBuf + aMaxLen );
    tiny_print( &strBuf, apFmt, args );

    return strBuf.curLen;
}
/*-----------------------------------------------------------*/

int snprintf( char *apBuf, size_t aMaxLen, const char *apFmt, ... )
{
    va_list args;

    va_start( args,  apFmt );
    struct SStringBuf strBuf;
    strbuf_init( &strBuf, apBuf, ( const char* )apBuf + aMaxLen );
    tiny_print( &strBuf, apFmt, args );
    va_end( args );

    return strBuf.curLen;
}
#endif

static void log_header_format()
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
