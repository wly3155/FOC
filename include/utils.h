#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdint.h>

#ifdef __cplusplus
extern "c" {
#endif

#define REG32_READ(reg)				(*((volatile uint32_t *)reg))
#define REG32_WRITE(reg, value)		(*((volatile uint32_t *)reg) = value)

#define tick_to_ms(tick)			((uint64_t)tick * configTICK_RATE_HZ / 1000)

#define ms_to_ns(time)				((uint64_t)time * 1000000)

#ifdef __cplusplus
}
#endif
#endif