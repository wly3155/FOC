include build.config
include foc.mk

OUTPUT_DIR=$(PROJECT)_OUT
TARGET = $(PROJECT)

#C_FLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"

# default action: build all
all: $(OUTPUT_DIR)/$(TARGET).elf
	$(_CROSS_COMPILER_OBJCOPY_) -O ihex $(OUTPUT_DIR)/$(TARGET).elf $(OUTPUT_DIR)/$(TARGET).hex
	$(_CROSS_COMPILER_OBJCOPY_) -O binary $(OUTPUT_DIR)/$(TARGET).elf $(OUTPUT_DIR)/$(TARGET).bin
	$(_CROSS_COMPILER_OBJDUMP_) -S -d $(OUTPUT_DIR)/$(TARGET).elf > $(OUTPUT_DIR)/$(TARGET).asm
	@echo "Sucessfuly build" $(PROJECT)

#######################################
# build the application
#######################################
# list of objects
C_FILES := $(patsubst ./%,%,$(C_FILES))
vpath %.c $(sort $(dir $(C_FILES)))
OBJECTS := $(sort $(C_FILES:%.c=$(OUTPUT_DIR)/%.o))

# list of ASM program objects
ASM_FILES := $(patsubst ./%,%,$(ASM_FILES))
vpath %.s $(sort $(dir $(ASM_FILES)))
OBJECTS += $(sort $(ASM_FILES:%.s=$(OUTPUT_DIR)/%.o))

$(OUTPUT_DIR)/%.o : %.c Makefile | $(OUTPUT_DIR)
	@echo 'compile $< to $@'
	@mkdir -p $(dir $@)
	$(_CROSS_COMPILER_GCC_) -c $(C_FLAGS) $(C_INCLUDES) \
	-Wa,-a,-ad,-alms=$(OUTPUT_DIR)/$(<:.c=.lst) $< -o $@

$(OUTPUT_DIR)/%.o : %.s Makefile | $(OUTPUT_DIR)
	@echo 'compile $< to $@'
	@mkdir -p $(dir $@)
	$(_CROSS_COMPILER_GCC_) -x assembler-with-cpp -c $(C_FLAGS) $(C_INCLUDES) $< -o $@

$(OUTPUT_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(_CROSS_COMPILER_GCC_) $(OBJECTS) $(C_FLAGS) $(LD_LIBS) -T$(LD_SCRIPT) -o $@
	$(_CROSS_COMPILER_SIZE_) $@

$(OUTPUT_DIR):
	mkdir -p $@

.phony: clean
clean:
	-rm -fR .dep $(OUTPUT_DIR)

flash:
#for STM32 Usage Only
	$(FLASH_TOOL) write $(OUTPUT_DIR)/$(TARGET).bin $(FLASH_ADDR)

flash_and_run:
#for STM32 Usage Only
	$(FLASH_TOOL) --reset write $(OUTPUT_DIR)/$(TARGET).bin $(FLASH_ADDR)

erase:
	$(FLASH_TOOL) erase

-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)