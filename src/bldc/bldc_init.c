#include "bldc_manager.h"
#include "bldc_device.h"

void bldc_init(void)
{
	bldc_manager_init();
	bldc_device_init();
}