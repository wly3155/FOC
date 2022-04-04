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
OBJECTS = $(addprefix $(OUTPUT_DIR)/,$(notdir $(C_FILES:.c=.o)))
vpath %.c $(sort $(dir $(C_FILES)))

# list of ASM program objects
OBJECTS += $(addprefix $(OUTPUT_DIR)/,$(notdir $(ASM_FILES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_FILES)))

$(OUTPUT_DIR)/%.o: %.c Makefile | $(OUTPUT_DIR)
	$(_CROSS_COMPILER_GCC_) -c $(CFLAGS) $(C_INCLUDES) \
		-Wa,-a,-ad,-alms=$(OUTPUT_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(OUTPUT_DIR)/%.o: %.s Makefile | $(OUTPUT_DIR)
	$(_CROSS_COMPILER_GCC_) -x assembler-with-cpp -c $(CFLAGS) $(C_INCLUDES) $< -o $@

$(OUTPUT_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(_CROSS_COMPILER_GCC_) $(OBJECTS) $(C_FLAGS) $(LD_LIBS) -T$(LD_SCRIPT) -o $@
	$(_CROSS_COMPILER_SIZE_) $@

$(OUTPUT_DIR):
	mkdir $@

.phony: clean
clean:
	-rm -fR .dep $(OUTPUT_DIR)

flash:
#for STM32 Usage Only
	$(FLASH_TOOL) write $(OUTPUT_DIR)/$(TARGET).bin $(FLASH_ADDR)

erase:
	$(FLASH_TOOL) erase

-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)