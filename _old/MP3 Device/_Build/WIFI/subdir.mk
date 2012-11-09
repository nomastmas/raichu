################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../WIFI/wifi.cpp 

OBJS += \
./WIFI/wifi.o 

CPP_DEPS += \
./WIFI/wifi.d 


# Each subdirectory must supply rules for building sources it contributes
WIFI/%.o: ../WIFI/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Windows GCC C++ Compiler'
	arm-elf-g++ -I"C:\Users\User\Desktop\Spring2012\CMPE146\CmpE146Package\workspace\FinalProject_5_09" -I"C:\Users\User\Desktop\Spring2012\CMPE146\CmpE146Package\workspace\FinalProject_5_09\FreeRTOS" -Os -Wall -Wa,-adhlns="$@.lst" -fno-exceptions -fno-rtti -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=arm7tdmi-s -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


