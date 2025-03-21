################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/lvgl/examples/widgets/tabview/lv_example_tabview_1.c \
../Drivers/lvgl/examples/widgets/tabview/lv_example_tabview_2.c 

OBJS += \
./Drivers/lvgl/examples/widgets/tabview/lv_example_tabview_1.o \
./Drivers/lvgl/examples/widgets/tabview/lv_example_tabview_2.o 

C_DEPS += \
./Drivers/lvgl/examples/widgets/tabview/lv_example_tabview_1.d \
./Drivers/lvgl/examples/widgets/tabview/lv_example_tabview_2.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/lvgl/examples/widgets/tabview/%.o Drivers/lvgl/examples/widgets/tabview/%.su Drivers/lvgl/examples/widgets/tabview/%.cyclo: ../Drivers/lvgl/examples/widgets/tabview/%.c Drivers/lvgl/examples/widgets/tabview/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F469xx -c -I"/Users/zacharyberthillier/FEB/Code/DASH/Drivers/squareline_ui" -I/Users/raadwan/Developer/FEB_Firmware_SN3/ICS/Drivers/lvgl -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/BSP/Common -I../Drivers/BSP/STM32469I-Discovery -I"/Users/zacharyberthillier/FEB/Code/DASH/Drivers/lvgl" -I"/Users/zacharyberthillier/FEB/Code/DASH/Drivers/hal_stm_lvgl" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-lvgl-2f-examples-2f-widgets-2f-tabview

clean-Drivers-2f-lvgl-2f-examples-2f-widgets-2f-tabview:
	-$(RM) ./Drivers/lvgl/examples/widgets/tabview/lv_example_tabview_1.cyclo ./Drivers/lvgl/examples/widgets/tabview/lv_example_tabview_1.d ./Drivers/lvgl/examples/widgets/tabview/lv_example_tabview_1.o ./Drivers/lvgl/examples/widgets/tabview/lv_example_tabview_1.su ./Drivers/lvgl/examples/widgets/tabview/lv_example_tabview_2.cyclo ./Drivers/lvgl/examples/widgets/tabview/lv_example_tabview_2.d ./Drivers/lvgl/examples/widgets/tabview/lv_example_tabview_2.o ./Drivers/lvgl/examples/widgets/tabview/lv_example_tabview_2.su

.PHONY: clean-Drivers-2f-lvgl-2f-examples-2f-widgets-2f-tabview

