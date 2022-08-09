################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/DELAY_TIM7.c \
../src/UART_STM32.c \
../src/main.c \
../src/syscalls.c \
../src/system_stm32f4xx.c 

OBJS += \
./src/DELAY_TIM7.o \
./src/UART_STM32.o \
./src/main.o \
./src/syscalls.o \
./src/system_stm32f4xx.o 

C_DEPS += \
./src/DELAY_TIM7.d \
./src/UART_STM32.d \
./src/main.d \
./src/syscalls.d \
./src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32 -DSTM32F4 -DSTM32F407VGTx -DSTM32F407G_DISC1 -DSTM32F40XX -DSTM32F40_41xxx -DUSE_STDPERIPH_DRIVER -I"D:/my docs/projects/stm32/ADC_injected_after_regul/StdPeriph_Driver/inc" -I"D:/my docs/projects/stm32/ADC_injected_after_regul/inc" -I"D:/my docs/projects/stm32/ADC_injected_after_regul/CMSIS/device" -I"D:/my docs/projects/stm32/ADC_injected_after_regul/CMSIS/core" -O3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


