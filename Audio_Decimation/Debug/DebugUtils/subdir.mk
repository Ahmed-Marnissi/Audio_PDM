################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../DebugUtils/debug.c 

OBJS += \
./DebugUtils/debug.o 

C_DEPS += \
./DebugUtils/debug.d 


# Each subdirectory must supply rules for building sources it contributes
DebugUtils/%.o DebugUtils/%.su DebugUtils/%.cyclo: ../DebugUtils/%.c DebugUtils/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -DARM_MATH_CM4 -c -I../Core/Inc -I"D:/git/Audio_PDM/Audio_Decimation/DebugUtils" -I"D:/git/Audio_PDM/Audio_Decimation/Tasks/Task_fft/Inc" -I"D:/git/Audio_PDM/Audio_Decimation/Tasks/Task_Decimation/Inc" -I"D:/git/Audio_PDM/Audio_Decimation/BSP/Microphone/Inc" -I"D:/git/Audio_PDM/Audio_Decimation/Drivers/CMSIS/DSP/Include" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-DebugUtils

clean-DebugUtils:
	-$(RM) ./DebugUtils/debug.cyclo ./DebugUtils/debug.d ./DebugUtils/debug.o ./DebugUtils/debug.su

.PHONY: clean-DebugUtils

