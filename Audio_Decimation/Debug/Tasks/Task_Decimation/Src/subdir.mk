################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Tasks/Task_Decimation/Src/decimation_task.c \
../Tasks/Task_Decimation/Src/fir_coeffes.c 

OBJS += \
./Tasks/Task_Decimation/Src/decimation_task.o \
./Tasks/Task_Decimation/Src/fir_coeffes.o 

C_DEPS += \
./Tasks/Task_Decimation/Src/decimation_task.d \
./Tasks/Task_Decimation/Src/fir_coeffes.d 


# Each subdirectory must supply rules for building sources it contributes
Tasks/Task_Decimation/Src/%.o Tasks/Task_Decimation/Src/%.su: ../Tasks/Task_Decimation/Src/%.c Tasks/Task_Decimation/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -DARM_MATH_CM4 -c -I../Core/Inc -I"C:/Users/21624/Desktop/dsp_ws/Audio_Decimation/Tasks/Task_Decimation/Inc" -I"C:/Users/21624/Desktop/dsp_ws/Audio_Decimation/BSP/Microphone/Inc" -I"C:/Users/21624/Desktop/dsp_ws/Audio_Decimation/Drivers/CMSIS/DSP/Include" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Tasks-2f-Task_Decimation-2f-Src

clean-Tasks-2f-Task_Decimation-2f-Src:
	-$(RM) ./Tasks/Task_Decimation/Src/decimation_task.d ./Tasks/Task_Decimation/Src/decimation_task.o ./Tasks/Task_Decimation/Src/decimation_task.su ./Tasks/Task_Decimation/Src/fir_coeffes.d ./Tasks/Task_Decimation/Src/fir_coeffes.o ./Tasks/Task_Decimation/Src/fir_coeffes.su

.PHONY: clean-Tasks-2f-Task_Decimation-2f-Src

