################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include utils/src/subdir.mk
-include fat/subdir.mk
-include fat/option/subdir.mk
-include drivers/subdir.mk
-include WIFI/subdir.mk
-include System/subdir.mk
-include System/startup/subdir.mk
-include MP3/subdir.mk
-include General/subdir.mk
-include FreeRTOS/src/subdir.mk
-include FreeRTOS/mem/subdir.mk
-include FreeRTOS/hooks/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
FinalProject_5_09.hex \

SECONDARY_LIST += \
FinalProject_5_09.lst \

SECONDARY_SIZE += \
FinalProject_5_09.siz \


# All Target
all: FinalProject_5_09.elf secondary-outputs

# Tool invocations
FinalProject_5_09.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Windows GCC C++ Linker'
	arm-elf-g++ -T"..\System\startup\FREERTOS_LPC2148.ld" -nostartfiles -Wl,-Map,FinalProject_5_09.map -mcpu=arm7tdmi-s -o"FinalProject_5_09.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

FinalProject_5_09.hex: FinalProject_5_09.elf
	@echo 'Invoking: ARM Windows GNU Create Flash Image'
	arm-elf-objcopy -O ihex FinalProject_5_09.elf "FinalProject_5_09.hex"
	@echo 'Finished building: $@'
	@echo ' '

FinalProject_5_09.lst: FinalProject_5_09.elf
	@echo 'Invoking: ARM Windows GNU Create Listing'
	arm-elf-objdump -h -S FinalProject_5_09.elf >"FinalProject_5_09.lst"
	@echo 'Finished building: $@'
	@echo ' '

FinalProject_5_09.siz: FinalProject_5_09.elf
	@echo 'Invoking: ARM Windows GNU Print Size'
	arm-elf-size  --format=berkeley -t FinalProject_5_09.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(SECONDARY_FLASH)$(CXX_DEPS)$(S_UPPER_DEPS)$(SECONDARY_LIST)$(C++_DEPS)$(SECONDARY_SIZE)$(ASM_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(C_UPPER_DEPS) FinalProject_5_09.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
