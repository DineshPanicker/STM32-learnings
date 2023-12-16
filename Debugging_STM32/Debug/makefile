################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Startup/subdir.mk
-include Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
Debugging_STM32.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
Debugging_STM32.list \

OBJCOPY_BIN += \
Debugging_STM32.bin \


# All Target
all: Debugging_STM32.elf secondary-outputs

# Tool invocations
Debugging_STM32.elf: $(OBJS) $(USER_OBJS) G:\STM32-learnings\Debugging_STM32\STM32F401RETX_FLASH.ld
	arm-none-eabi-gcc -o "Debugging_STM32.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"G:\STM32-learnings\Debugging_STM32\STM32F401RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="Debugging_STM32.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

Debugging_STM32.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "Debugging_STM32.list"
	@echo 'Finished building: $@'
	@echo ' '

Debugging_STM32.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "Debugging_STM32.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
