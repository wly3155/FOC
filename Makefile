include build.config
include foc.mk

OUTPUT_DIR=$(PROJECT)_OUT
$(warning )

all:
	@echo "Start to compile"
	mkdir -p $(OUTPUT_DIR)
	$(_CROSS_COMPILER_GCC_) -o $(OUTPUT_DIR)/$(PROJECT).elf $(C_FILES) \
	$(C_INCLUDES) $(C_FLAGS) $(LD_LIBS) -T$(LD_SCRIPT)

.phony: clean

clean: 
	rm -rf $(OUTPUT_DIR)/*