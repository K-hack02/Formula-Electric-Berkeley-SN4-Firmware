################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/lvgl/src/extra/widgets/msgbox/lv_msgbox.c 

OBJS += \
./Drivers/lvgl/src/extra/widgets/msgbox/lv_msgbox.o 

C_DEPS += \
./Drivers/lvgl/src/extra/widgets/msgbox/lv_msgbox.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/lvgl/src/extra/widgets/msgbox/%.o Drivers/lvgl/src/extra/widgets/msgbox/%.su Drivers/lvgl/src/extra/widgets/msgbox/%.cyclo: ../Drivers/lvgl/src/extra/widgets/msgbox/%.c Drivers/lvgl/src/extra/widgets/msgbox/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F469xx -c -I"/Users/zacharyberthillier/FEB/Code/DASH/Drivers/squareline_ui" -I/Users/raadwan/Developer/FEB_Firmware_SN3/ICS/Drivers/lvgl -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/BSP/Common -I../Drivers/BSP/STM32469I-Discovery -I"/Users/zacharyberthillier/FEB/Code/DASH/Drivers/lvgl" -I"/Users/zacharyberthillier/FEB/Code/DASH/Drivers/hal_stm_lvgl" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-lvgl-2f-src-2f-extra-2f-widgets-2f-msgbox

clean-Drivers-2f-lvgl-2f-src-2f-extra-2f-widgets-2f-msgbox:
	-$(RM) ./Drivers/lvgl/src/extra/widgets/msgbox/lv_msgbox.cyclo ./Drivers/lvgl/src/extra/widgets/msgbox/lv_msgbox.d ./Drivers/lvgl/src/extra/widgets/msgbox/lv_msgbox.o ./Drivers/lvgl/src/extra/widgets/msgbox/lv_msgbox.su

.PHONY: clean-Drivers-2f-lvgl-2f-src-2f-extra-2f-widgets-2f-msgbox

