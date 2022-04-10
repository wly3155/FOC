include foc.config

PROJECT=FOC

ifeq ($(CFG_STM32F405RGT6_SUPPORT),yes)
STM32_DIR=../../../STM32
C_FLAGS += -DSTM32F40_41xxx
FPU = -mfloat-abi=softfp -mfpu=fpv4-sp-d16
MCU = -mcpu=cortex-m3 -mthumb $(FPU) $(FLOAT_ABI)
C_FLAGS += $(MCU)
C_FLAGS += -Wno-misleading-indentation
LD_SCRIPT_DIR=$(STM32_DIR)/STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Project/STM32F4xx_StdPeriph_Templates
LD_SCRIPT=$(LD_SCRIPT_DIR)/SW4STM32/STM32F40_41xxx/STM32F417IGHx_FLASH.ld
STM32_CMSIS_DIR=$(STM32_DIR)/STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Libraries/CMSIS
STM32_PERIPH_DIR=$(STM32_DIR)/STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Libraries/STM32F4xx_StdPeriph_Driver
STM32_TEMPLATES_DIR=$(STM32_DIR)/STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Project/STM32F4xx_StdPeriph_Templates
C_INCLUDES += -I$(STM32_CMSIS_DIR)/Include
C_INCLUDES += -I$(STM32_CMSIS_DIR)/Device/ST/STM32F4xx/Include
C_INCLUDES += -I$(STM32_PERIPH_DIR)/inc
C_INCLUDES += -I$(STM32_TEMPLATES_DIR)
ASM_FILES += $(STM32_CMSIS_DIR)/Device/ST/STM32F4xx/Source/Templates/gcc_ride7/startup_stm32f40_41xxx.s
#C_FILES += $(STM32_CMSIS_DIR)/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c
C_FILES += $(STM32_TEMPLATES_DIR)/stm32f4xx_it.c
C_FILES += $(STM32_TEMPLATES_DIR)/system_stm32f4xx.c
C_FILES += $(STM32_PERIPH_DIR)/src/stm32f4xx_rcc.c
C_FILES += $(STM32_PERIPH_DIR)/src/stm32f4xx_exti.c
C_FILES += $(STM32_PERIPH_DIR)/src/stm32f4xx_gpio.c
C_FILES += $(STM32_PERIPH_DIR)/src/stm32f4xx_usart.c
C_FILES += $(STM32_PERIPH_DIR)/src/misc.c
LD_LIBS +=
FLASH_TOOL_DIR = /usr/local/bin
FLASH_TOOL = $(FLASH_TOOL_DIR)/st-flash
FLASH_ADDR = 0x8000000
endif

ifeq ($(CFG_FREERTOS_SUPPORT),yes)
FREERTOS_DIR=../../../freeRTOS
FREERTOS_SOURCE_DIR=$(FREERTOS_DIR)/FreeRTOS-Kernel-10.1.1/FreeRTOS/Source
FREERTOS_PORTABLE_DIR=$(FREERTOS_SOURCE_DIR)/portable
C_INCLUDES += -I$(FREERTOS_PORTABLE_DIR)/GCC/ARM_CM4F
C_FILES += $(FREERTOS_PORTABLE_DIR)/GCC/ARM_CM4F/port.c
C_FILES += $(FREERTOS_PORTABLE_DIR)/MemMang/heap_4.c
C_INCLUDES += -I$(FREERTOS_SOURCE_DIR)/include
C_FILES += $(FREERTOS_SOURCE_DIR)/tasks.c
C_FILES += $(FREERTOS_SOURCE_DIR)/queue.c
C_FILES += $(FREERTOS_SOURCE_DIR)/list.c
C_FILES += $(FREERTOS_SOURCE_DIR)/timers.c
endif

FOC_DIR=.
C_INCLUDES += -I$(FOC_DIR)/include
C_FILES += $(FOC_DIR)/src/main.c
C_FILES += $(FOC_DIR)/src/led.c
ifeq ($(CFG_FOC_TEST_SUPPORT),yes)
C_FILES += test/main.c
endif

