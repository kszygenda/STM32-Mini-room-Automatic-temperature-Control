################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Components/Src/LCD.c \
../Components/Src/bmp2.c \
../Components/Src/bmp2_config.c \
../Components/Src/pid_controller.c \
../Components/Src/pid_controller_config.c 

OBJS += \
./Components/Src/LCD.o \
./Components/Src/bmp2.o \
./Components/Src/bmp2_config.o \
./Components/Src/pid_controller.o \
./Components/Src/pid_controller_config.o 

C_DEPS += \
./Components/Src/LCD.d \
./Components/Src/bmp2.d \
./Components/Src/bmp2_config.d \
./Components/Src/pid_controller.d \
./Components/Src/pid_controller_config.d 


# Each subdirectory must supply rules for building sources it contributes
Components/Src/%.o Components/Src/%.su Components/Src/%.cyclo: ../Components/Src/%.c Components/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F746xx -c -I../Core/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/ARM_CMSIS/CMSIS/Core/Include/ -I../Middlewares/Third_Party/ARM_CMSIS/CMSIS/Core_A/Include/ -I../Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Components-2f-Src

clean-Components-2f-Src:
	-$(RM) ./Components/Src/LCD.cyclo ./Components/Src/LCD.d ./Components/Src/LCD.o ./Components/Src/LCD.su ./Components/Src/bmp2.cyclo ./Components/Src/bmp2.d ./Components/Src/bmp2.o ./Components/Src/bmp2.su ./Components/Src/bmp2_config.cyclo ./Components/Src/bmp2_config.d ./Components/Src/bmp2_config.o ./Components/Src/bmp2_config.su ./Components/Src/pid_controller.cyclo ./Components/Src/pid_controller.d ./Components/Src/pid_controller.o ./Components/Src/pid_controller.su ./Components/Src/pid_controller_config.cyclo ./Components/Src/pid_controller_config.d ./Components/Src/pid_controller_config.o ./Components/Src/pid_controller_config.su

.PHONY: clean-Components-2f-Src

