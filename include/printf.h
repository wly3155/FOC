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

#ifndef __PRINTF_H__
#define __PRINTF_H__

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void _putchar(char character);
#define sprintf sprintf_
int sprintf_(char* buffer, const char* format, ...);
#define snprintf  snprintf_
#define vsnprintf vsnprintf_
int  snprintf_(char* buffer, size_t count, const char* format, ...);
int vsnprintf_(char* buffer, size_t count, const char* format, va_list va);
#define vprintf vprintf_
int vprintf_(const char* format, va_list va);
int fctprintf(void (*out)(char character, void* arg), void* arg, const char* format, ...);

#define PRINTF_SUPPORT
#define DEFAULT_PRINTF_LEVEL        (PRINTF_INFO)

enum {
	PRINTF_DEBUG,
	PRINTF_INFO,
	PRINTF_WARN,
	PRINTF_ERROR,
};

#ifdef PRINTF_SUPPORT
#define pr_debug(format, ...) \
	if (PRINTF_DEBUG >= DEFAULT_PRINTF_LEVEL) printf(format, ##__VA_ARGS__)
#define pr_info(format, ...) \
	if (PRINTF_INFO >= DEFAULT_PRINTF_LEVEL) printf(format, ##__VA_ARGS__)
#define pr_warn(format, ...) \
	if (PRINTF_WARN >= DEFAULT_PRINTF_LEVEL) printf(format, ##__VA_ARGS__)
#define pr_err(format, ...) \
	if (PRINTF_ERROR >= DEFAULT_PRINTF_LEVEL) printf(format, ##__VA_ARGS__)
#else
#define pr_debug(format, ...) {}
#define pr_info(format, ...) {}
#define pr_warn(format, ...) {}
#define pr_err(format, ...) {}
#endif

int printf_init();

#ifdef __cplusplus
}
#endif
#endif