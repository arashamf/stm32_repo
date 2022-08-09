################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/MCU_ini.c \
../src/UART_lib.c \
../src/delay_lib.c \
../src/ini_can.c \
../src/main.c \
../src/syscalls.c \
../src/system_stm32f4xx.c 

OBJS += \
./src/MCU_ini.o \
./src/UART_lib.o \
./src/delay_lib.o \
./src/ini_can.o \
./src/main.o \
./src/syscalls.o \
./src/system_stm32f4xx.o 

C_DEPS += \
./src/MCU_ini.d \
./src/UART_lib.d \
./src/delay_lib.d \
./src/ini_can.d \
./src/main.d \
./src/syscalls.d \
./src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32 -DSTM32F4 -DSTM32F405RGTx -DSTM32F40XX -DSTM32F40_41xxx -DUSE_STDPERIPH_DRIVER -I"D:/projects/stm32/can_hub_f405/StdPeriph_Driver/inc" -I"D:/projects/stm32/can_hub_f405/inc" -I"D:/projects/stm32/can_hub_f405/CMSIS/device" -I"D:/projects/stm32/can_hub_f405/CMSIS/core" -O3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


