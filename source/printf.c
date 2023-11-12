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

#define PRINTF_MAX_BUF			(4096)
#define PRINTF_MAX_PRE_LINE		(256)

struct logger_struct {
	uint32_t size;
	uint32_t wp;
	char buffer[PRINTF_MAX_BUF];
};

static struct logger_struct logger_str;

static void log_buffer_ring_write(struct logger_struct *logger, char *data)
{
	uint8_t first = 0, second = 0;

	first = min(strlen(data), logger->size - logger->wp);
	second = strlen(data) - first;
	memcpy(logger->buffer + logger->wp, data, first);
	memcpy(logger->buffer, data + first, second);
	logger->wp += strlen(data);
	logger->wp &= (logger->size - 1);
}

static int log_header_format(struct logger_struct *logger)
{
	char timestamp[16] = {0};
	uint64_t now = get_boot_time_ns();
	uint32_t sec = now / 1000000000;
	uint32_t mini_sec = now % 1000000000 / 1000000;

	snprintf(timestamp, sizeof(timestamp), "[%lu.%03lu] ", sec, mini_sec);
	log_buffer_ring_write(logger, timestamp);
#ifdef CFG_USART_SUPPORT
	usart_printf(timestamp, strlen(timestamp));
#endif
        return 0;
}

int __wrap_printf(const char *fmt, ...)
{
	struct logger_struct *logger = &logger_str;
	char buf[PRINTF_MAX_PRE_LINE] = {0};
	va_list args;

	taskDISABLE_INTERRUPTS();
	log_header_format(logger);
	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	log_buffer_ring_write(logger, buf);
#ifdef CFG_USART_SUPPORT
	usart_printf(buf, strlen(buf));
#endif
	va_end(args);
	taskENABLE_INTERRUPTS();
	return 0;
}

int log_init()
{
	struct logger_struct *logger = &logger_str;

	logger->wp = 0;
	logger->size = PRINTF_MAX_BUF;
	memset(logger->buffer, 0x00, sizeof(logger->buffer));
#ifdef CFG_USART_SUPPORT
	usart_printf_init();
#endif
	return 0;
}
