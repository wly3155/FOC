#ifndef __BLDC_H__
#define __BLDC_H__

#include <stdbool.h>
#include <stdint.h>
#include "bldc_resource.h"

#ifdef __cplusplus
extern "c" {
#endif

enum {
	BLDC_PWM_ON_MODE,
	BLDC_MAX_SUPPORT_MODE,
};

enum {
	EVENT_PUBLIC_START = 0,
	EVENT_DISABLE = EVENT_PUBLIC_START,
	EVENT_ENABLE,
	EVENT_CALIBRATE,
	EVENT_CONFIG,
	EVENT_PUBLIC_END,
	EVENT_PRIVATE_START = 128,
	EVENT_PRIVATE_END = 160,
};

enum {
	STATUS_UNINIT = 0,
	STATUS_IDLE,
	STATUS_RUNNING,
	STATUS_ERROR,
};

struct bldc_header {
	uint8_t id;
	uint8_t padding;
	uint8_t reserved;
};

struct bldc_control {
	float duty;
	uint32_t freq;
};

struct bldc_config {
	uint8_t cmd;
	uint8_t data[6];
	uint8_t len;
};

struct bldc_event {
	uint8_t event_type;
	struct bldc_header header;
	union {
		struct bldc_control ctrl;
		struct bldc_config cfg;
	};
};

struct bldc_device {
	uint8_t status;
	uint8_t to_step;
	float duty;
	uint32_t freq;
};

void bldc_init(void);
int bldc_enable(uint8_t id, float duty);
int bldc_disble(uint8_t id);

#ifdef __cplusplus
}
#endif
#endif
