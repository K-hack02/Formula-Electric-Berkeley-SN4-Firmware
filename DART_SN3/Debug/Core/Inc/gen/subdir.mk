################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Inc/gen/feb_can.c 

OBJS += \
./Core/Inc/gen/feb_can.o 

C_DEPS += \
./Core/Inc/gen/feb_can.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Inc/gen/%.o Core/Inc/gen/%.su Core/Inc/gen/%.cyclo: ../Core/Inc/gen/%.c Core/Inc/gen/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F042x6 -c -I../Core/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F0xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Inc-2f-gen

clean-Core-2f-Inc-2f-gen:
	-$(RM) ./Core/Inc/gen/feb_can.cyclo ./Core/Inc/gen/feb_can.d ./Core/Inc/gen/feb_can.o ./Core/Inc/gen/feb_can.su

.PHONY: clean-Core-2f-Inc-2f-gen

