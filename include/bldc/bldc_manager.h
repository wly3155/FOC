#ifndef __BLDC_H__
#define __BLDC_H__

#include <stdbool.h>
#include <stdint.h>
#include "bldc_resource.h"

#ifdef __cplusplus
extern "c" {
#endif

enum BLDC_MODE {
    BLDC_PWM_ON_MODE,
    BLDC_MAX_SUPPORT_MODE,
};

enum BLDC_EVENT {
    EVENT_PUBLIC_START = 0,
    EVENT_UPDATE = EVENT_PUBLIC_START,
    EVENT_DISABLE,
    EVENT_ENABLE,
    EVENT_CALIBRATE,
    EVENT_CONFIG,
    EVENT_PUBLIC_END,

    EVENT_PRIVATE_START = 128,
    EVENT_PRIVATE_END = 160,
};

enum BLDC_STATUS {
    STATUS_UNINIT = 0,
    STATUS_IDLE,
    STATUS_STARTUP,
    STATUS_RUNNING,
    STATUS_ERROR,
    STATUS_END,
};

enum BLDC_DIR {
    DIR_CW,
    DIR_CCW,
};

enum BLDC_CFG_CMD {
    DIRECTION,
    MAX_CFG_CMD,
};

struct bldc_header {
    uint8_t id;
    uint8_t padding;
    uint8_t reserved;
};

struct bldc_control {
    uint8_t dir;
    float duty;
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

struct bldc_device_interface {
    int (*update_step)(struct bldc_event *);
    int (*enable)(struct bldc_event *);
    int (*disable)(struct bldc_event *);
    int (*config)(struct bldc_event *);
};

/* BLDC Interface for Users */
int bldc_enable(uint8_t id, float duty);
int bldc_disble(uint8_t id);
int bldc_config(uint8_t id, uint8_t cmd, void *data, uint8_t len);

void bldc_manager_init(void);


int bldc_event_enqueue(struct bldc_event *event);
int bldc_manager_device_register(struct bldc_device_interface *interface);

#ifdef __cplusplus
}
#endif
#endif
