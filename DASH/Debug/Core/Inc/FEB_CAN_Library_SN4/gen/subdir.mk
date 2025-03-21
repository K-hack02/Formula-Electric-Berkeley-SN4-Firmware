################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Inc/FEB_CAN_Library_SN4/gen/feb_can.c 

OBJS += \
./Core/Inc/FEB_CAN_Library_SN4/gen/feb_can.o 

C_DEPS += \
./Core/Inc/FEB_CAN_Library_SN4/gen/feb_can.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Inc/FEB_CAN_Library_SN4/gen/%.o Core/Inc/FEB_CAN_Library_SN4/gen/%.su Core/Inc/FEB_CAN_Library_SN4/gen/%.cyclo: ../Core/Inc/FEB_CAN_Library_SN4/gen/%.c Core/Inc/FEB_CAN_Library_SN4/gen/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F469xx -c -I"/Users/zacharyberthillier/FEB/Code/DASH/Drivers/squareline_ui" -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/BSP/Common -I../Drivers/BSP/STM32469I-Discovery -I"/Users/zacharyberthillier/FEB/Code/DASH/Drivers/lvgl" -I"/Users/zacharyberthillier/FEB/Code/DASH/Drivers/hal_stm_lvgl" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Inc-2f-FEB_CAN_Library_SN4-2f-gen

clean-Core-2f-Inc-2f-FEB_CAN_Library_SN4-2f-gen:
	-$(RM) ./Core/Inc/FEB_CAN_Library_SN4/gen/feb_can.cyclo ./Core/Inc/FEB_CAN_Library_SN4/gen/feb_can.d ./Core/Inc/FEB_CAN_Library_SN4/gen/feb_can.o ./Core/Inc/FEB_CAN_Library_SN4/gen/feb_can.su

.PHONY: clean-Core-2f-Inc-2f-FEB_CAN_Library_SN4-2f-gen

