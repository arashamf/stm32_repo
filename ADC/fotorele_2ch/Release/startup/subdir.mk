################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../startup/startup_stm32.s 

OBJS += \
./startup/startup_stm32.o 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo $(PWD)
	arm-none-eabi-as -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -I"D:/my docs/projects/stm32/lesson_5_2_ADC_AWD/fotorele_2ch/StdPeriph_Driver/inc" -I"D:/my docs/projects/stm32/lesson_5_2_ADC_AWD/fotorele_2ch/inc" -I"D:/my docs/projects/stm32/lesson_5_2_ADC_AWD/fotorele_2ch/CMSIS/device" -I"D:/my docs/projects/stm32/lesson_5_2_ADC_AWD/fotorele_2ch/CMSIS/core" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


