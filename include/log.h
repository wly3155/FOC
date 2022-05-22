#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>

#ifdef __cplusplus
extern "c" {
#endif

enum {
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARN,
	LOG_ERROR,
};

#define DEFAULT_LOG_LEVEL		(LOG_INFO)
#define logd(format, ...)		if (LOG_DEBUG >= DEFAULT_LOG_LEVEL) printf(format, ##__VA_ARGS__)
#define logi(format, ...)		if (LOG_INFO >= DEFAULT_LOG_LEVEL) printf(format, ##__VA_ARGS__)
#define logw(format, ...)		if (LOG_WARN >= DEFAULT_LOG_LEVEL) printf(format, ##__VA_ARGS__)
#define loge(format, ...)		if (LOG_ERROR >= DEFAULT_LOG_LEVEL) printf(format, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif
#endif