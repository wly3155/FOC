include foc.config

PROJECT=FOC

ifeq ($(CFG_STM32F405RGT6_SUPPORT),yes)
C_FLAGS += -DSTM32F40_41xxx
STM32_DIR=../../../STM32
ISP_DOWNLOAD_DIR="none"
STM32_DIR=../../../STM32
LD_SCRIPT_DIR=$(STM32_DIR)/STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/Project/STM32F4xx_StdPeriph_Templates
LD_SCRIPT=$(LD_SCRIPT_DIR)/Linux/STM32F40_41xxx/STM32F417IGHx_FLASH.ld
STM32_CMSIS_DIR=$(STM32_DIR)/STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/Libraries/CMSIS
STM32_PERIPH_DIR=$(STM32_DIR)/STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/Libraries/STM32F4xx_StdPeriph_Driver
STM32_TEMPLATES_DIR=$(STM32_DIR)/STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/Project/STM32F4xx_StdPeriph_Templates
C_INCLUDES += -I$(STM32_CMSIS_DIR)/Include
C_INCLUDES += -I$(STM32_CMSIS_DIR)/Device/ST/STM32F4xx/Include
C_INCLUDES += -I$(STM32_PERIPH_DIR)/inc
C_INCLUDES += -I$(STM32_TEMPLATES_DIR)
C_FILES += $(STM32_CMSIS_DIR)/Device/ST/STM32F4xx/Source/Templates/gcc_ride7/startup_stm32f40_41xxx.s
#C_FILES += $(STM32_CMSIS_DIR)/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c
C_FILES += $(STM32_PERIPH_DIR)/src/stm32f4xx_rcc.c
C_FILES += $(STM32_PERIPH_DIR)/src/stm32f4xx_gpio.c
C_FILES += $(STM32_TEMPLATES_DIR)/*.c
LD_LIBS +=
$(warning $(C_INCLUDES))
$(warning $(C_FILES))
endif

ifeq ($(CFG_FOC_TEST_SUPPORT),yes)
C_FILES += test/main.c
endif

