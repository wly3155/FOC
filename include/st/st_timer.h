#ifndef __ST_TIMER_H__
#define __ST_TIMER_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "c" {
#endif

int timer_clock_enable(int channel, bool en);

#ifdef __cplusplus
}
#endif
#endif