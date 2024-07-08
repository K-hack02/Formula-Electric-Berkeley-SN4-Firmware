#ifndef INC_FEB_HW_H_
#define INC_FEB_HW_H_

#include "stm32f4xx_hal.h"

#define FEB_HW_LED_ON 	((GPIO_PinState) GPIO_PIN_SET)
#define FEB_HW_LED_OFF 	((GPIO_PinState) GPIO_PIN_RESET)

void FEB_Hw_LED(GPIO_PinState pin_state);

#endif /* INC_FEB_HW_H_ */
