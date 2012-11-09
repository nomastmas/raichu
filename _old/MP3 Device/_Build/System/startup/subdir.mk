################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../System/startup/exceptionHandlers.c 

S_UPPER_SRCS += \
../System/startup/Arm7CStartup.S \
../System/startup/Lpc2000ExceptionVectors.S 

OBJS += \
./System/startup/Arm7CStartup.o \
./System/startup/Lpc2000ExceptionVectors.o \
./System/startup/exceptionHandlers.o 

C_DEPS += \
./System/startup/exceptionHandlers.d 

S_UPPER_DEPS += \
./System/startup/Arm7CStartup.d \
./System/startup/Lpc2000ExceptionVectors.d 


# Each subdirectory must supply rules for building sources it contributes
System/startup/%.o: ../System/startup/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Windows GCC Assembler'
	arm-elf-gcc -x assembler-with-cpp -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=arm7tdmi-s -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

System/startup/%.o: ../System/startup/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Windows GCC C Compiler'
	arm-elf-gcc -I"C:\Users\User\Desktop\Spring2012\CMPE146\CmpE146Package\workspace\FinalProject_5_09" -I"C:\Users\User\Desktop\Spring2012\CMPE146\CmpE146Package\workspace\FinalProject_5_09\FreeRTOS" -Os -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=arm7tdmi-s -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


