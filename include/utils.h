#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>

#ifdef __cplusplus
extern "c" {
#endif

#define max(x, y)			((x) > (y) ? (x) : (y))
#define min(x, y)			((x) < (y) ? (x) : (y))

#define tick_to_ms(tick)		((uint64_t)tick * configTICK_RATE_HZ / 1000)
#define ms_to_us(time)			((uint64_t)time * 1000)
#define us_to_ns(time)			((uint64_t)time * 1000)
#define ms_to_ns(time)			(us_to_ns(ms_to_us(time)))

#define ARRAY_SIZE(arr)			(sizeof(arr) / sizeof(arr[0]))
#define fatal(x)			configASSERT(x)

#ifdef __cplusplus
}
#endif
#endif