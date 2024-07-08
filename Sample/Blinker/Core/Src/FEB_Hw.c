#include "FEB_Hw.h"

void FEB_Hw_LED(GPIO_PinState pin_state) {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, pin_state);
}
