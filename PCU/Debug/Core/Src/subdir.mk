################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/FEB_CAN.c \
../Core/Src/FEB_CAN_BMS.c \
../Core/Src/FEB_CAN_BSPD.c \
../Core/Src/FEB_CAN_ICS.c \
../Core/Src/FEB_CAN_RMS.c \
../Core/Src/FEB_CAN_RMS_UNIT_TEST.c \
../Core/Src/FEB_HECS.c \
../Core/Src/FEB_Main.c \
../Core/Src/FEB_Normalized.c \
../Core/Src/FEB_TPS2482.c \
../Core/Src/FEB_Timer.c \
../Core/Src/main.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c 

OBJS += \
./Core/Src/FEB_CAN.o \
./Core/Src/FEB_CAN_BMS.o \
./Core/Src/FEB_CAN_BSPD.o \
./Core/Src/FEB_CAN_ICS.o \
./Core/Src/FEB_CAN_RMS.o \
./Core/Src/FEB_CAN_RMS_UNIT_TEST.o \
./Core/Src/FEB_HECS.o \
./Core/Src/FEB_Main.o \
./Core/Src/FEB_Normalized.o \
./Core/Src/FEB_TPS2482.o \
./Core/Src/FEB_Timer.o \
./Core/Src/main.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o 

C_DEPS += \
./Core/Src/FEB_CAN.d \
./Core/Src/FEB_CAN_BMS.d \
./Core/Src/FEB_CAN_BSPD.d \
./Core/Src/FEB_CAN_ICS.d \
./Core/Src/FEB_CAN_RMS.d \
./Core/Src/FEB_CAN_RMS_UNIT_TEST.d \
./Core/Src/FEB_HECS.d \
./Core/Src/FEB_Main.d \
./Core/Src/FEB_Normalized.d \
./Core/Src/FEB_TPS2482.d \
./Core/Src/FEB_Timer.d \
./Core/Src/main.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/FEB_CAN.cyclo ./Core/Src/FEB_CAN.d ./Core/Src/FEB_CAN.o ./Core/Src/FEB_CAN.su ./Core/Src/FEB_CAN_BMS.cyclo ./Core/Src/FEB_CAN_BMS.d ./Core/Src/FEB_CAN_BMS.o ./Core/Src/FEB_CAN_BMS.su ./Core/Src/FEB_CAN_BSPD.cyclo ./Core/Src/FEB_CAN_BSPD.d ./Core/Src/FEB_CAN_BSPD.o ./Core/Src/FEB_CAN_BSPD.su ./Core/Src/FEB_CAN_ICS.cyclo ./Core/Src/FEB_CAN_ICS.d ./Core/Src/FEB_CAN_ICS.o ./Core/Src/FEB_CAN_ICS.su ./Core/Src/FEB_CAN_RMS.cyclo ./Core/Src/FEB_CAN_RMS.d ./Core/Src/FEB_CAN_RMS.o ./Core/Src/FEB_CAN_RMS.su ./Core/Src/FEB_CAN_RMS_UNIT_TEST.cyclo ./Core/Src/FEB_CAN_RMS_UNIT_TEST.d ./Core/Src/FEB_CAN_RMS_UNIT_TEST.o ./Core/Src/FEB_CAN_RMS_UNIT_TEST.su ./Core/Src/FEB_HECS.cyclo ./Core/Src/FEB_HECS.d ./Core/Src/FEB_HECS.o ./Core/Src/FEB_HECS.su ./Core/Src/FEB_Main.cyclo ./Core/Src/FEB_Main.d ./Core/Src/FEB_Main.o ./Core/Src/FEB_Main.su ./Core/Src/FEB_Normalized.cyclo ./Core/Src/FEB_Normalized.d ./Core/Src/FEB_Normalized.o ./Core/Src/FEB_Normalized.su ./Core/Src/FEB_TPS2482.cyclo ./Core/Src/FEB_TPS2482.d ./Core/Src/FEB_TPS2482.o ./Core/Src/FEB_TPS2482.su ./Core/Src/FEB_Timer.cyclo ./Core/Src/FEB_Timer.d ./Core/Src/FEB_Timer.o ./Core/Src/FEB_Timer.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su

.PHONY: clean-Core-2f-Src

