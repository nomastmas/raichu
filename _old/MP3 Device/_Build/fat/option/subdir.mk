################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../fat/option/ccsbcs.c \
../fat/option/reentrant.c 

OBJS += \
./fat/option/ccsbcs.o \
./fat/option/reentrant.o 

C_DEPS += \
./fat/option/ccsbcs.d \
./fat/option/reentrant.d 


# Each subdirectory must supply rules for building sources it contributes
fat/option/%.o: ../fat/option/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Windows GCC C Compiler'
	arm-elf-gcc -I"C:\Users\User\Desktop\Spring2012\CMPE146\CmpE146Package\workspace\FinalProject_5_09" -I"C:\Users\User\Desktop\Spring2012\CMPE146\CmpE146Package\workspace\FinalProject_5_09\FreeRTOS" -Os -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=arm7tdmi-s -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


