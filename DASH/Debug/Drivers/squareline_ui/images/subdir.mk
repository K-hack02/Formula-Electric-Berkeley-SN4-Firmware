################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/squareline_ui/images/DR_REGEN.c \
../Drivers/squareline_ui/images/DR_STDBY.c \
../Drivers/squareline_ui/images/FAULT.c \
../Drivers/squareline_ui/images/STDBY.c \
../Drivers/squareline_ui/images/STRTUP.c \
../Drivers/squareline_ui/images/balance.c \
../Drivers/squareline_ui/images/charge.c \
../Drivers/squareline_ui/images/decimal.c \
../Drivers/squareline_ui/images/degrees.c \
../Drivers/squareline_ui/images/drive.c \
../Drivers/squareline_ui/images/eight.c \
../Drivers/squareline_ui/images/five.c \
../Drivers/squareline_ui/images/four.c \
../Drivers/squareline_ui/images/hvoff.c \
../Drivers/squareline_ui/images/hvon.c \
../Drivers/squareline_ui/images/io_label.c \
../Drivers/squareline_ui/images/nine.c \
../Drivers/squareline_ui/images/null.c \
../Drivers/squareline_ui/images/one.c \
../Drivers/squareline_ui/images/percent.c \
../Drivers/squareline_ui/images/precharge.c \
../Drivers/squareline_ui/images/regen_off.c \
../Drivers/squareline_ui/images/regen_ready.c \
../Drivers/squareline_ui/images/seven.c \
../Drivers/squareline_ui/images/shutdown.c \
../Drivers/squareline_ui/images/six.c \
../Drivers/squareline_ui/images/three.c \
../Drivers/squareline_ui/images/tsalmphbms_label.c \
../Drivers/squareline_ui/images/two.c \
../Drivers/squareline_ui/images/ui_img_1348218614.c \
../Drivers/squareline_ui/images/v.c \
../Drivers/squareline_ui/images/zero.c 

OBJS += \
./Drivers/squareline_ui/images/DR_REGEN.o \
./Drivers/squareline_ui/images/DR_STDBY.o \
./Drivers/squareline_ui/images/FAULT.o \
./Drivers/squareline_ui/images/STDBY.o \
./Drivers/squareline_ui/images/STRTUP.o \
./Drivers/squareline_ui/images/balance.o \
./Drivers/squareline_ui/images/charge.o \
./Drivers/squareline_ui/images/decimal.o \
./Drivers/squareline_ui/images/degrees.o \
./Drivers/squareline_ui/images/drive.o \
./Drivers/squareline_ui/images/eight.o \
./Drivers/squareline_ui/images/five.o \
./Drivers/squareline_ui/images/four.o \
./Drivers/squareline_ui/images/hvoff.o \
./Drivers/squareline_ui/images/hvon.o \
./Drivers/squareline_ui/images/io_label.o \
./Drivers/squareline_ui/images/nine.o \
./Drivers/squareline_ui/images/null.o \
./Drivers/squareline_ui/images/one.o \
./Drivers/squareline_ui/images/percent.o \
./Drivers/squareline_ui/images/precharge.o \
./Drivers/squareline_ui/images/regen_off.o \
./Drivers/squareline_ui/images/regen_ready.o \
./Drivers/squareline_ui/images/seven.o \
./Drivers/squareline_ui/images/shutdown.o \
./Drivers/squareline_ui/images/six.o \
./Drivers/squareline_ui/images/three.o \
./Drivers/squareline_ui/images/tsalmphbms_label.o \
./Drivers/squareline_ui/images/two.o \
./Drivers/squareline_ui/images/ui_img_1348218614.o \
./Drivers/squareline_ui/images/v.o \
./Drivers/squareline_ui/images/zero.o 

C_DEPS += \
./Drivers/squareline_ui/images/DR_REGEN.d \
./Drivers/squareline_ui/images/DR_STDBY.d \
./Drivers/squareline_ui/images/FAULT.d \
./Drivers/squareline_ui/images/STDBY.d \
./Drivers/squareline_ui/images/STRTUP.d \
./Drivers/squareline_ui/images/balance.d \
./Drivers/squareline_ui/images/charge.d \
./Drivers/squareline_ui/images/decimal.d \
./Drivers/squareline_ui/images/degrees.d \
./Drivers/squareline_ui/images/drive.d \
./Drivers/squareline_ui/images/eight.d \
./Drivers/squareline_ui/images/five.d \
./Drivers/squareline_ui/images/four.d \
./Drivers/squareline_ui/images/hvoff.d \
./Drivers/squareline_ui/images/hvon.d \
./Drivers/squareline_ui/images/io_label.d \
./Drivers/squareline_ui/images/nine.d \
./Drivers/squareline_ui/images/null.d \
./Drivers/squareline_ui/images/one.d \
./Drivers/squareline_ui/images/percent.d \
./Drivers/squareline_ui/images/precharge.d \
./Drivers/squareline_ui/images/regen_off.d \
./Drivers/squareline_ui/images/regen_ready.d \
./Drivers/squareline_ui/images/seven.d \
./Drivers/squareline_ui/images/shutdown.d \
./Drivers/squareline_ui/images/six.d \
./Drivers/squareline_ui/images/three.d \
./Drivers/squareline_ui/images/tsalmphbms_label.d \
./Drivers/squareline_ui/images/two.d \
./Drivers/squareline_ui/images/ui_img_1348218614.d \
./Drivers/squareline_ui/images/v.d \
./Drivers/squareline_ui/images/zero.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/squareline_ui/images/%.o Drivers/squareline_ui/images/%.su Drivers/squareline_ui/images/%.cyclo: ../Drivers/squareline_ui/images/%.c Drivers/squareline_ui/images/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F469xx -c -I"/Users/zacharyberthillier/FEB/Code/DASH/Drivers/squareline_ui" -I/Users/raadwan/Developer/FEB_Firmware_SN3/ICS/Drivers/lvgl -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/BSP/Common -I../Drivers/BSP/STM32469I-Discovery -I"/Users/zacharyberthillier/FEB/Code/DASH/Drivers/lvgl" -I"/Users/zacharyberthillier/FEB/Code/DASH/Drivers/hal_stm_lvgl" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-squareline_ui-2f-images

clean-Drivers-2f-squareline_ui-2f-images:
	-$(RM) ./Drivers/squareline_ui/images/DR_REGEN.cyclo ./Drivers/squareline_ui/images/DR_REGEN.d ./Drivers/squareline_ui/images/DR_REGEN.o ./Drivers/squareline_ui/images/DR_REGEN.su ./Drivers/squareline_ui/images/DR_STDBY.cyclo ./Drivers/squareline_ui/images/DR_STDBY.d ./Drivers/squareline_ui/images/DR_STDBY.o ./Drivers/squareline_ui/images/DR_STDBY.su ./Drivers/squareline_ui/images/FAULT.cyclo ./Drivers/squareline_ui/images/FAULT.d ./Drivers/squareline_ui/images/FAULT.o ./Drivers/squareline_ui/images/FAULT.su ./Drivers/squareline_ui/images/STDBY.cyclo ./Drivers/squareline_ui/images/STDBY.d ./Drivers/squareline_ui/images/STDBY.o ./Drivers/squareline_ui/images/STDBY.su ./Drivers/squareline_ui/images/STRTUP.cyclo ./Drivers/squareline_ui/images/STRTUP.d ./Drivers/squareline_ui/images/STRTUP.o ./Drivers/squareline_ui/images/STRTUP.su ./Drivers/squareline_ui/images/balance.cyclo ./Drivers/squareline_ui/images/balance.d ./Drivers/squareline_ui/images/balance.o ./Drivers/squareline_ui/images/balance.su ./Drivers/squareline_ui/images/charge.cyclo ./Drivers/squareline_ui/images/charge.d ./Drivers/squareline_ui/images/charge.o ./Drivers/squareline_ui/images/charge.su ./Drivers/squareline_ui/images/decimal.cyclo ./Drivers/squareline_ui/images/decimal.d ./Drivers/squareline_ui/images/decimal.o ./Drivers/squareline_ui/images/decimal.su ./Drivers/squareline_ui/images/degrees.cyclo ./Drivers/squareline_ui/images/degrees.d ./Drivers/squareline_ui/images/degrees.o ./Drivers/squareline_ui/images/degrees.su ./Drivers/squareline_ui/images/drive.cyclo ./Drivers/squareline_ui/images/drive.d ./Drivers/squareline_ui/images/drive.o ./Drivers/squareline_ui/images/drive.su ./Drivers/squareline_ui/images/eight.cyclo ./Drivers/squareline_ui/images/eight.d ./Drivers/squareline_ui/images/eight.o ./Drivers/squareline_ui/images/eight.su ./Drivers/squareline_ui/images/five.cyclo ./Drivers/squareline_ui/images/five.d ./Drivers/squareline_ui/images/five.o ./Drivers/squareline_ui/images/five.su ./Drivers/squareline_ui/images/four.cyclo ./Drivers/squareline_ui/images/four.d ./Drivers/squareline_ui/images/four.o ./Drivers/squareline_ui/images/four.su ./Drivers/squareline_ui/images/hvoff.cyclo ./Drivers/squareline_ui/images/hvoff.d ./Drivers/squareline_ui/images/hvoff.o ./Drivers/squareline_ui/images/hvoff.su ./Drivers/squareline_ui/images/hvon.cyclo ./Drivers/squareline_ui/images/hvon.d ./Drivers/squareline_ui/images/hvon.o ./Drivers/squareline_ui/images/hvon.su ./Drivers/squareline_ui/images/io_label.cyclo ./Drivers/squareline_ui/images/io_label.d ./Drivers/squareline_ui/images/io_label.o ./Drivers/squareline_ui/images/io_label.su ./Drivers/squareline_ui/images/nine.cyclo ./Drivers/squareline_ui/images/nine.d ./Drivers/squareline_ui/images/nine.o ./Drivers/squareline_ui/images/nine.su ./Drivers/squareline_ui/images/null.cyclo ./Drivers/squareline_ui/images/null.d ./Drivers/squareline_ui/images/null.o ./Drivers/squareline_ui/images/null.su ./Drivers/squareline_ui/images/one.cyclo ./Drivers/squareline_ui/images/one.d ./Drivers/squareline_ui/images/one.o ./Drivers/squareline_ui/images/one.su ./Drivers/squareline_ui/images/percent.cyclo ./Drivers/squareline_ui/images/percent.d ./Drivers/squareline_ui/images/percent.o ./Drivers/squareline_ui/images/percent.su ./Drivers/squareline_ui/images/precharge.cyclo ./Drivers/squareline_ui/images/precharge.d ./Drivers/squareline_ui/images/precharge.o ./Drivers/squareline_ui/images/precharge.su ./Drivers/squareline_ui/images/regen_off.cyclo ./Drivers/squareline_ui/images/regen_off.d ./Drivers/squareline_ui/images/regen_off.o ./Drivers/squareline_ui/images/regen_off.su ./Drivers/squareline_ui/images/regen_ready.cyclo ./Drivers/squareline_ui/images/regen_ready.d ./Drivers/squareline_ui/images/regen_ready.o ./Drivers/squareline_ui/images/regen_ready.su ./Drivers/squareline_ui/images/seven.cyclo ./Drivers/squareline_ui/images/seven.d ./Drivers/squareline_ui/images/seven.o ./Drivers/squareline_ui/images/seven.su ./Drivers/squareline_ui/images/shutdown.cyclo ./Drivers/squareline_ui/images/shutdown.d ./Drivers/squareline_ui/images/shutdown.o ./Drivers/squareline_ui/images/shutdown.su ./Drivers/squareline_ui/images/six.cyclo ./Drivers/squareline_ui/images/six.d ./Drivers/squareline_ui/images/six.o ./Drivers/squareline_ui/images/six.su ./Drivers/squareline_ui/images/three.cyclo ./Drivers/squareline_ui/images/three.d ./Drivers/squareline_ui/images/three.o ./Drivers/squareline_ui/images/three.su ./Drivers/squareline_ui/images/tsalmphbms_label.cyclo ./Drivers/squareline_ui/images/tsalmphbms_label.d ./Drivers/squareline_ui/images/tsalmphbms_label.o ./Drivers/squareline_ui/images/tsalmphbms_label.su ./Drivers/squareline_ui/images/two.cyclo ./Drivers/squareline_ui/images/two.d ./Drivers/squareline_ui/images/two.o ./Drivers/squareline_ui/images/two.su ./Drivers/squareline_ui/images/ui_img_1348218614.cyclo ./Drivers/squareline_ui/images/ui_img_1348218614.d ./Drivers/squareline_ui/images/ui_img_1348218614.o ./Drivers/squareline_ui/images/ui_img_1348218614.su ./Drivers/squareline_ui/images/v.cyclo ./Drivers/squareline_ui/images/v.d ./Drivers/squareline_ui/images/v.o ./Drivers/squareline_ui/images/v.su ./Drivers/squareline_ui/images/zero.cyclo ./Drivers/squareline_ui/images/zero.d ./Drivers/squareline_ui/images/zero.o ./Drivers/squareline_ui/images/zero.su

.PHONY: clean-Drivers-2f-squareline_ui-2f-images

