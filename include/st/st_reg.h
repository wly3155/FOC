#ifndef __ST_REG_H__
#define __ST_REG_H__

#include <stdint.h>
#include <math.h>

#ifdef __cplusplus
extern "c" {
#endif

#define reg_readl(reg)							(uint32_t)(*((volatile uint32_t *)(reg)))
#define reg_writel(reg, val)					(*(volatile unsigned int *)(reg) = (val))

static inline uint32_t reg_bitwise_write(volatile uint32_t reg, uint32_t bitwise, uint32_t value)
{
	uint32_t tmp = reg_readl(reg);

	tmp &= ~bitwise;
	tmp |= value;
	reg_writel(reg, tmp);
	return tmp;
}

#define OIS1_SHIFT_BIT					(8)

#define GPIO_BASE_REG					(0x40020000)
#define GPIOx_GROUP_BASE_OFFSET			(0x400)
#define GPIOx_MODE_OFFSET				(0x00)
#define GPIOx_OUT_TYPE_OFFSET			(0x04)
#define GPIOx_OUT_SPEED_OFFSET			(0x08)
#define GPIOx_PULL_UPDOWN_OFFSET		(0x0c)
#define GPIOx_IN_DATA_OFFSET			(0x10)
#define GPIOx_OUT_DATA_OFFSET			(0x14)
#define GPIOx_ALTER_LOW_OFFSET			(0x20)
#define GPIOx_ALTER_HIGH_OFFSET			(0x24)

#define GPIO_PERIPH_CLOCK_OFFSET		(0x01)
#define PERIPH_CLOCK_BIT_SHIFT			(0x02)

#define MODE_DEFAULT					(0x00)
#define MODE_SHIFT_BITS					(0x02)
#define MODE_MASK						((1 << MODE_SHIFT_BITS) - 1)

#define OUT_VALUE_DEFAULT				(0x00)
#define OUT_VALUE_SHIFT_BITS			(0x01)
#define OUT_VALUE_MASK					((1 << OUT_VALUE_SHIFT_BITS) - 1)

#define OUT_TYPE_DEFAULT				(0x00)
#define OUT_TYPE_SHIFT_BITS				(0x01)
#define OUT_TYPE_MASK					((1 << OUT_TYPE_SHIFT_BITS) - 1)

#define OUT_SPEED_DEFAULT				(0x00)
#define OUT_SPEED_SHIFT_BITS			(0x02)
#define OUT_SPEED_MASK					((1 << OUT_SPEED_SHIFT_BITS) - 1)

#define PULL_UPDOWN_DEFAULT				(0x00)
#define PULL_UPDOWN_SHIFT_BITS			(0x02)
#define PULL_UPDOWN_MASK				((1 << PULL_UPDOWN_SHIFT_BITS) - 1)

#define ALTER_MODE_PORT_BITS			(0x08)
#define ALTER_MODE_SHIFT_BITS			(0x04)
#define ALTER_MODE_MASK					((1 << ALTER_MODE_SHIFT_BITS) - 1)

#ifdef __cplusplus
}
#endif
#endif