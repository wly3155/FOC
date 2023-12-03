/*
 * This file is licensed under the Apache License, Version 2.0.
 *
 * Copyright (c) 2023 wuliyong3155@163.com
 *
 * A copy of the license can be obtained at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#include <FreeRTOS.h>
#include <task.h>

#include <printf.h>
#include <time.h>
//#include <uart.h>
#include <utils.h>

#ifdef CFG_SEGGER_RTT_SUPPORT
#include "RTT/SEGGER_RTT.h"
#endif

#define PRINTF_MAX_BUF			(4096)
#define PRINTF_MAX_PRE_LINE		(256)

struct printf_buffer {
	char buffer[PRINTF_MAX_BUF];
	uint32_t size;
	uint32_t wp;
};

static struct printf_buffer printf_buf;

static void printf_buffer_ring_write(struct printf_buffer *pb, char *data)
{
	uint8_t first = 0, second = 0;

	taskDISABLE_INTERRUPTS();
	first = min(strlen(data), pb->size - pb->wp);
	second = strlen(data) - first;
	memcpy(pb->buffer + pb->wp, data, first);
	memcpy(pb->buffer, data + first, second);
	pb->wp += strlen(data);
	pb->wp &= (pb->size - 1);
	taskENABLE_INTERRUPTS();
}

static int printf_header_format(struct printf_buffer *pb)
{
	char timestamp[16] = {0};
	uint64_t now = get_boot_time_ns();
	uint32_t sec = now / 1000000000;
	uint32_t mini_sec = (uint32_t)(now / 1000000 % 1000);

	snprintf(timestamp, sizeof(timestamp), "[%lu.%03lu] ", sec, mini_sec);
	printf_buffer_ring_write(pb, timestamp);
#ifdef CFG_USART_SUPPORT
        usart_printf(timestamp, strlen(timestamp));
#endif

#ifdef CFG_SEGGER_RTT_SUPPORT
        SEGGER_RTT_WriteString(0, timestamp);
#endif
        return 0;
}

int __wrap_printf(const char *fmt, ...)
{
	struct printf_buffer *pb = &printf_buf;
	char buf[PRINTF_MAX_PRE_LINE] = {0};
	va_list args;

	taskDISABLE_INTERRUPTS();
	printf_header_format(pb);
	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	printf_buffer_ring_write(pb, buf);
#ifdef CFG_USART_SUPPORT
	usart_printf(buf, strlen(buf));
#endif

#ifdef CFG_SEGGER_RTT_SUPPORT
        SEGGER_RTT_WriteString(0, buf);
#endif
	va_end(args);
	taskENABLE_INTERRUPTS();
	return 0;
}

int printf_init()
{
	struct printf_buffer *pb = &printf_buf;

	pb->wp = 0;
	pb->size = PRINTF_MAX_BUF;
	memset(pb->buffer, 0x00, sizeof(pb->buffer));
#ifdef CFG_USART_SUPPORT
	usart_printf_init();
#endif

#ifdef CFG_SEGGER_RTT_SUPPORT
        SEGGER_RTT_Init();
#endif
	return 0;
}
