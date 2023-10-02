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
Hello-World.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
Hello-World.list \

OBJCOPY_BIN += \
Hello-World.bin \


# All Target
all: Hello-World.elf secondary-outputs

# Tool invocations
Hello-World.elf: $(OBJS) $(USER_OBJS) G:\STM32-learnings\Hello-World\STM32F401RETX_FLASH.ld
	arm-none-eabi-gcc -o "Hello-World.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"G:\STM32-learnings\Hello-World\STM32F401RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="Hello-World.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

Hello-World.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "Hello-World.list"
	@echo 'Finished building: $@'
	@echo ' '

Hello-World.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "Hello-World.bin"
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
