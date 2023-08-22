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
Code_Data_memory.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
Code_Data_memory.list \

OBJCOPY_HEX += \
Code_Data_memory.hex \

OBJCOPY_BIN += \
Code_Data_memory.bin \


# All Target
all: Code_Data_memory.elf secondary-outputs

# Tool invocations
Code_Data_memory.elf: $(OBJS) $(USER_OBJS) A:\E.C.E\ONLINE\ COURSES\Udemy\ STM32\Embedded\ C\My_Workspace\target\Code_Data_memory\STM32F401RETX_FLASH.ld
	arm-none-eabi-gcc -o "Code_Data_memory.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"A:\E.C.E\ONLINE COURSES\Udemy STM32\Embedded C\My_Workspace\target\Code_Data_memory\STM32F401RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="Code_Data_memory.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

Code_Data_memory.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "Code_Data_memory.list"
	@echo 'Finished building: $@'
	@echo ' '

Code_Data_memory.hex: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "Code_Data_memory.hex"
	@echo 'Finished building: $@'
	@echo ' '

Code_Data_memory.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "Code_Data_memory.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_HEX) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
