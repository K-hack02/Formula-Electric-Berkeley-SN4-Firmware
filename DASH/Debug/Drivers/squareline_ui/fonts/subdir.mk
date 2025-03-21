################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/squareline_ui/fonts/ui_font_intervar52.c \
../Drivers/squareline_ui/fonts/ui_font_robotf.c \
../Drivers/squareline_ui/fonts/ui_font_roboto20.c \
../Drivers/squareline_ui/fonts/ui_font_roboto42.c \
../Drivers/squareline_ui/fonts/ui_font_roboto52.c \
../Drivers/squareline_ui/fonts/ui_font_roboto60.c 

OBJS += \
./Drivers/squareline_ui/fonts/ui_font_intervar52.o \
./Drivers/squareline_ui/fonts/ui_font_robotf.o \
./Drivers/squareline_ui/fonts/ui_font_roboto20.o \
./Drivers/squareline_ui/fonts/ui_font_roboto42.o \
./Drivers/squareline_ui/fonts/ui_font_roboto52.o \
./Drivers/squareline_ui/fonts/ui_font_roboto60.o 

C_DEPS += \
./Drivers/squareline_ui/fonts/ui_font_intervar52.d \
./Drivers/squareline_ui/fonts/ui_font_robotf.d \
./Drivers/squareline_ui/fonts/ui_font_roboto20.d \
./Drivers/squareline_ui/fonts/ui_font_roboto42.d \
./Drivers/squareline_ui/fonts/ui_font_roboto52.d \
./Drivers/squareline_ui/fonts/ui_font_roboto60.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/squareline_ui/fonts/%.o Drivers/squareline_ui/fonts/%.su Drivers/squareline_ui/fonts/%.cyclo: ../Drivers/squareline_ui/fonts/%.c Drivers/squareline_ui/fonts/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F469xx -c -I"/Users/zacharyberthillier/FEB/Code/DASH/Drivers/squareline_ui" -I/Users/raadwan/Developer/FEB_Firmware_SN3/ICS/Drivers/lvgl -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/BSP/Common -I../Drivers/BSP/STM32469I-Discovery -I"/Users/zacharyberthillier/FEB/Code/DASH/Drivers/lvgl" -I"/Users/zacharyberthillier/FEB/Code/DASH/Drivers/hal_stm_lvgl" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-squareline_ui-2f-fonts

clean-Drivers-2f-squareline_ui-2f-fonts:
	-$(RM) ./Drivers/squareline_ui/fonts/ui_font_intervar52.cyclo ./Drivers/squareline_ui/fonts/ui_font_intervar52.d ./Drivers/squareline_ui/fonts/ui_font_intervar52.o ./Drivers/squareline_ui/fonts/ui_font_intervar52.su ./Drivers/squareline_ui/fonts/ui_font_robotf.cyclo ./Drivers/squareline_ui/fonts/ui_font_robotf.d ./Drivers/squareline_ui/fonts/ui_font_robotf.o ./Drivers/squareline_ui/fonts/ui_font_robotf.su ./Drivers/squareline_ui/fonts/ui_font_roboto20.cyclo ./Drivers/squareline_ui/fonts/ui_font_roboto20.d ./Drivers/squareline_ui/fonts/ui_font_roboto20.o ./Drivers/squareline_ui/fonts/ui_font_roboto20.su ./Drivers/squareline_ui/fonts/ui_font_roboto42.cyclo ./Drivers/squareline_ui/fonts/ui_font_roboto42.d ./Drivers/squareline_ui/fonts/ui_font_roboto42.o ./Drivers/squareline_ui/fonts/ui_font_roboto42.su ./Drivers/squareline_ui/fonts/ui_font_roboto52.cyclo ./Drivers/squareline_ui/fonts/ui_font_roboto52.d ./Drivers/squareline_ui/fonts/ui_font_roboto52.o ./Drivers/squareline_ui/fonts/ui_font_roboto52.su ./Drivers/squareline_ui/fonts/ui_font_roboto60.cyclo ./Drivers/squareline_ui/fonts/ui_font_roboto60.d ./Drivers/squareline_ui/fonts/ui_font_roboto60.o ./Drivers/squareline_ui/fonts/ui_font_roboto60.su

.PHONY: clean-Drivers-2f-squareline_ui-2f-fonts

