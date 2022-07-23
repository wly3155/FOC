#ifndef __ST_REG_H__
#define __ST_REG_H__

#include <stdint.h>
#include <math.h>

#ifdef __cplusplus
extern "c" {
#endif

#define reg_readl(reg)							(uint32_t)(*((volatile uint32_t *)(reg)))
#define reg_writel(reg, val)					(*(volatile uint32_t *)(reg) = (uint32_t)(val))

static uint32_t reg_bitwise_write(volatile uint32_t reg, uint32_t bitwise, uint32_t value)
{
	uint32_t tmp = reg_readl(reg);

	tmp &= ~bitwise;
	tmp |= value;
	reg_writel(reg, tmp);
	return tmp;
}

/* stm32f405 gpio registers */
#define GPIO_BASE_REG					(0x40020000)
#define GPIOx_GROUP_BASE_OFFSET			(0x400)
#define GPIOx_MODE_OFFSET				(0x00)
#define GPIOx_OUT_TYPE_OFFSET			(0x04)
#define GPIOx_OUT_SPEED_OFFSET			(0x08)
#define GPIOx_PULL_UPDOWN_OFFSET		(0x0C)
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

#define OUT_TYPE_DEFAULT				(0x00) //PUSH_PULL_MODE
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

/* stm32f405 timer registers */
#define TIM1_BASE_REG					(0x40010000)
#define TIM2_BASE_REG					(0x40000000)
#define TIM3_BASE_REG					(0x40000400)
#define TIM4_BASE_REG					(0x40000800)
#define TIM5_BASE_REG					(0x40000C00)
#define TIM8_BASE_REG					(0x40010400)
#define TIMX_CR1_OFFSET					(0x00)
#define TIMX_CR2_OFFSET					(0x04)
#define TIMX_SMCR_OFFSET				(0x08)
#define TIMX_DIER_OFFSET				(0x0C)
#define TIMX_SR_OFFSET					(0x10)
#define TIMX_EGR_OFFSET					(0x14)
#define TIMX_CCMR1_OFFSET				(0x18)
#define TIMX_CCMR2_OFFSET				(0x1C)
#define TIMX_CCER_OFFSET				(0x20)
#define TIMX_CNT_OFFSET					(0x24)
#define TIMX_PSC_OFFSET					(0x28)
#define TIMX_ARR_OFFSET					(0x2C)
#define TIMX_CCR_OFFSET					(0x34)
#define TIMX_DCR_OFFSET					(0x48)
#define TIMX_DMAR_OFFSET				(0x4C)
#define TIMX_REG_WIDTH					(0x10)

#define TIMX_CCMR_CH_OFFSET				(0x04)
#define TIMX_CCR_CH_OFFSET				(0x04)

/* stm32f405 timer cr1 setting */
#define UDIS_SHIFT_BITS					(0x01)
#define UDIS_BIT_WIDTH					(0x01)
#define UDIS_MASK_MASK					((1 << UDIS_BIT_WIDTH) - 1)
#define UDIS_ENABLE						(0x00)
#define UDIS_DISABLE					(0x01)

/* stm32f405 timer ccmr setting */
#define CCMR_CH_SHIFT_BITS				(0x08)
#define CCXS_SHIFT_BITS					(0x00)
#define CCXS_BIT_WIDTH					(0x02)
#define CCXS_MASK						((1 << CCXS_BIT_WIDTH) - 1)
#define CCXS_DIRCT_INPUT				(0x01)

#define ICXPSC_SHIFT_BITS				(0x02)
#define ICXPSC_BIT_WIDTH				(0x02)
#define ICXPSC_MASK						((1 << ICXPSC_BIT_WIDTH) - 1)
#define ICXPSC_NO_PSC					(0x00)

#define ICXF_SHIFT_BITS					(0x04)
#define ICXF_BIT_WIDTH					(0x04)
#define ICXF_MASK						((1 << ICXF_SHIFT_BITS) - 1)
#define ICXF_NO_FILTER					(0x00)

/* stm32f405 timer ccer setting */
#define CCER_CH_SHIFT_BITS				(0x04)
#define CCXNP_SHIFT_BITS				(0x01)
#define CCXNP_BIT_WIDTH					(0x03)
#define CCXNP_MASK						((1 << CCXNP_BIT_WIDTH) - 1)
#define CCXP_BOTH_EDGE					(0x0a)

#define DIER_CH_SHIFT_BITS				(0x01)
#define CCXIE_SHIFT_BITS				(0x01)
#define CCXIE_BIT_WIDTH					(0x01)
#define CCXIE_MASK						((1 << CCXIE_BIT_WIDTH) - 1)
#define CCXIE_ENABLE					(0x01)
#define CCXIE_DISABLE					(0x00)

#define TIMX_CEN_TEST_VALUE				((uint8_t)0x1)
#define CEN_SHIFT_BITS					(0x00)
#define CEN_BIT_WIDTH					(0x01)
#define CEN_MASK						((1 << CEN_BIT_WIDTH) - 1)
#define CEN_ENABLE						(0x01)

#define OIS1_SHIFT_BIT					(0x08)

#ifdef __cplusplus
}
#endif
#endif