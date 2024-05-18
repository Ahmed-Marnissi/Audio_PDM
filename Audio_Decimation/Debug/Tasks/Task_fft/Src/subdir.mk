################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Tasks/Task_fft/Src/task_fft.c 

OBJS += \
./Tasks/Task_fft/Src/task_fft.o 

C_DEPS += \
./Tasks/Task_fft/Src/task_fft.d 


# Each subdirectory must supply rules for building sources it contributes
Tasks/Task_fft/Src/%.o Tasks/Task_fft/Src/%.su: ../Tasks/Task_fft/Src/%.c Tasks/Task_fft/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -DARM_MATH_CM4 -c -I../Core/Inc -I"C:/Users/21624/Desktop/dsp_ws/Audio_PDM/Audio_Decimation/DebugUtils" -I"C:/Users/21624/Desktop/dsp_ws/Audio_PDM/Audio_Decimation/Tasks/Task_fft/Inc" -I"C:/Users/21624/Desktop/dsp_ws/Audio_PDM/Audio_Decimation/Tasks/Task_Decimation/Inc" -I"C:/Users/21624/Desktop/dsp_ws/Audio_PDM/Audio_Decimation/BSP/Microphone/Inc" -I"C:/Users/21624/Desktop/dsp_ws/Audio_PDM/Audio_Decimation/Drivers/CMSIS/DSP/Include" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Tasks-2f-Task_fft-2f-Src

clean-Tasks-2f-Task_fft-2f-Src:
	-$(RM) ./Tasks/Task_fft/Src/task_fft.d ./Tasks/Task_fft/Src/task_fft.o ./Tasks/Task_fft/Src/task_fft.su

.PHONY: clean-Tasks-2f-Task_fft-2f-Src

